/*
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of CoFlo.
 *
 * CoFlo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * CoFlo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * CoFlo.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file */

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <stack>
#include <typeinfo>
#include <cstdlib>

#include <boost/concept/requires.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/unordered_set.hpp>

#include "TranslationUnit.h"
#include "Block.h"
#include "Function.h"
#include "SuccessorTypes.h"

#include "statements/Statement.h"
#include "statements/If.h"
#include "statements/Switch.h"
#include "statements/FunctionCallUnresolved.h"
#include "statements/FunctionCallResolved.h"

#include "controlflowgraph/CFGEdgeTypeFallthrough.h"
#include "controlflowgraph/CFGEdgeTypeFunctionCall.h"
#include "controlflowgraph/CFGEdgeTypeImpossible.h"
#include "controlflowgraph/CFGEdgeTypeReturn.h"
#include "controlflowgraph/CFGEdgeTypeFunctionCallBypass.h"
#include "ControlFlowGraph.h"
#include "CFGDFSVisitor.h"

#include "libexttools/ToolDot.h"

/// Property map typedef which allows us to get at the function pointer stored at
/// CFGVertexProperties::m_containing_function in the T_CFG.
typedef boost::property_map<T_CFG, Function* CFGVertexProperties::*>::type T_VERTEX_PROPERTY_MAP;

/// Property map typedef which allows us to get at the edge type pointer stored at
/// CFGEdgeProperties::m_edge_type in the T_CFG.
typedef boost::property_map<T_CFG, CFGEdgeTypeBase* CFGEdgeProperties::*>::type T_EDGE_TYPE_PROPERTY_MAP;

/**
 * Predicate for filtering the CFG for only the vertices of the given function.
 */
struct vertex_filter_predicate
{
	vertex_filter_predicate()
	{
	}
	;
	vertex_filter_predicate(T_VERTEX_PROPERTY_MAP vertex_prop_map,
			Function *parent_function) :
			m_vertex_prop_map(vertex_prop_map), m_parent_function(
					parent_function)
	{
	}
	;
	bool operator()(const T_CFG_VERTEX_DESC& vid) const
	{
		if (m_parent_function == get(m_vertex_prop_map, vid))
		{
			// This vertex belongs to the function we're concerned with.
			return true;
		}
		else
		{
			return false;
		}
	}
	;

	T_VERTEX_PROPERTY_MAP m_vertex_prop_map;
	Function *m_parent_function;
};

Function::Function(TranslationUnit *parent_tu, const std::string &function_id)
{
	// Save a pointer to our parent TranslationUnit for later reference.
	m_parent_tu = parent_tu;
	m_function_id = function_id;
	m_entry_block = NULL;
	m_exit_block = NULL;

	// Add the entry and exit blocks.
	m_entry_block = new Block(this, 0, 0);
	m_exit_block = new Block(this, 1, 0);
	m_block_list.push_back(m_entry_block);
	m_block_list.push_back(m_exit_block);
}

Function::~Function()
{
}

std::string Function::GetDefinitionFilePath() const
{
	return m_parent_tu->GetFilePath();
}

bool Function::IsCalled() const
{
	// Determine if this function is ever called.
	// If the first statement (the ENTRY block) has any in-edges,
	// it's called by something.
	return boost::in_degree(m_first_statement, *m_cfg) > 0;
}

void Function::AddBlock(Block *block)
{
	m_block_list.push_back(block);
}

void Function::LinkBlocks()
{
	// A map of the blocks constituting this function.
	T_BLOCK_LINK_MAP linkmap;
	std::vector<Successor*> successor_list;

	// Add successors to the entry pseudoblock.
	m_entry_block->AddSuccessors("2 (fallthru)");

	// Go through all the blocks and add them to the link map.
	BOOST_FOREACH(Block *bp, m_block_list)
			{
				// Add this block to the number->Block* map.
				linkmap[bp->GetBlockNumber()] = bp;

				// Collect all the blocks' Successors into a list.
				Block::T_BLOCK_SUCCESSOR_ITERATOR s;
				for (s = bp->successor_begin(); s != bp->successor_end(); s++)
				{
					if (NULL != dynamic_cast<SuccessorExit*>(*s))
					{
						std::cout << "INFO: Found EXIT successor." << std::endl;
						(*s)->SetSuccessorBlockPtr(m_exit_block);
					}
					else if (NULL != dynamic_cast<SuccessorNoReturn*>(*s))
					{
						std::cout << "INFO: Found NoReturn successor."
								<< std::endl;
						(*s)->SetSuccessorBlockPtr(m_exit_block);
					}
					else
					{
						successor_list.push_back(*s);
					}
				}
			}

	// Block 2 appears to always be the first block, start with it.
	/// @todo We really should check for "PRED: ENTRY" to make sure of this.
	T_BLOCK_LINK_MAP_ITERATOR lmit = linkmap.find(2);
	if (lmit == linkmap.end())
	{
		std::cerr << "ERROR: Can't find Block 2." << std::endl;
	}

	// Now go through all the Successors and link them to the Blocks they refer to.
	std::vector<Successor*>::iterator it2;
	for (it2 = successor_list.begin(); it2 != successor_list.end(); it2++)
	{
		long block_no;
		T_BLOCK_LINK_MAP_ITERATOR linkmap_it;

		block_no = (*it2)->GetSuccessorBlockNumber();

		// Look up the block in the map.
		linkmap_it = linkmap.find(block_no);

		if (linkmap_it == linkmap.end())
		{
			std::cerr << "ERROR: Can't find Block " << block_no
					<< " in linkmap." << std::endl;
		}
		else if (linkmap_it->second == NULL)
		{
			std::cerr << "ERROR: Found a Successor with no Block pointer."
					<< std::endl;
		}
		else
		{
			// Found the referenced block, assign a pointer directly to it.
			(*it2)->SetSuccessorBlockPtr(linkmap_it->second);
		}
	}
}

void Function::LinkIntoGraph()
{
	// Block*->VertexID map.
	typedef std::map<Block*, T_BLOCK_GRAPH_VERTEX_DESC> T_BLOCK_MAP;
	T_BLOCK_MAP block_map;

	// Add the function blocks into the m_block_graph.	
	BOOST_FOREACH(Block *bp, m_block_list)
			{
				// this_block_id is the new Vertex.
				T_BLOCK_GRAPH_VERTEX_DESC this_block_id = boost::add_vertex(
						m_block_graph);
				m_block_graph[this_block_id].m_block = bp;

				// Add the block ID to a temporary map for use in linking below.
				block_map[bp] = this_block_id;
			}

	// Iterate over each block again, this time adding the edges between blocks.
	BOOST_FOREACH(Block *bp, m_block_list)
			{
				T_BLOCK_GRAPH_VERTEX_DESC this_block = block_map[bp];
				T_BLOCK_GRAPH_VERTEX_DESC next_block;

				// Go through all the successors.
				Block::T_BLOCK_SUCCESSOR_ITERATOR s;
				for (s = bp->successor_begin(); s != bp->successor_end(); s++)
				{
					T_BLOCK_MAP::iterator it;
					it = block_map.find((*s)->GetSuccessorBlockPtr());
					if (it != block_map.end())
					{
						// Found the next block.
						next_block = it->second;
						T_BLOCK_GRAPH_EDGE_DESC edge;
						bool ok;
						boost::tie(edge, ok) = boost::add_edge(this_block,
								next_block, m_block_graph);
						// Check if there's no error, then set up the edge properties.
						if (ok)
						{
							// Label the edge with whatever the Successor object has
							// for a label.
							if ((*s)->HasEdgeLabel())
							{
								m_block_graph[edge].m_edge_text =
										(*s)->GetEdgeLabel();
							}
						}
					}
					else
					{
						// Couldn't find the next block, it's probably EXIT.
						std::cerr
								<< "ERROR: Couldn't find next_block in block_map"
								<< std::endl;
					}
				}
			}
}

void Function::Link(const std::map<std::string, Function*> &function_map,
T_UNRESOLVED_FUNCTION_CALL_MAP *unresolved_function_calls)
{
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);

	vertex_filter_predicate the_filter(vpm, this);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
			vertex_filter_predicate> T_FILTERED_CFG;
	T_FILTERED_CFG graph_of_this_function(*m_cfg, boost::keep_all(),
			the_filter);

	boost::graph_traits<T_FILTERED_CFG>::vertex_iterator vit, vend;
	boost::tie(vit, vend) = boost::vertices(graph_of_this_function);
	for (; vit != vend; vit++)
	{
		FunctionCallUnresolved *fcu =
				dynamic_cast<FunctionCallUnresolved*>((*m_cfg)[*vit].m_statement);
		if (fcu != NULL)
		{
			std::map<std::string, Function*>::const_iterator it;

			// We found an unresolved function call.  Try to resolve it.
			it = function_map.find(fcu->GetIdentifier());

			if (it == function_map.end())
			{
				// Couldn't resolve it.  Add it to the unresolved call list.
				/// @todo Remove this: std::cerr << "WARNING: Can't find function " << fcu->GetIdentifier() << " in link map." << std::endl;
				unresolved_function_calls->push_back(fcu);
			}
			else
			{
				// Found it.
				// Replace the FunctionCallUnresolved with a FunctionCallResolved.
				FunctionCallResolved *fcr = new FunctionCallResolved(it->second,
						fcu);
				// Delete the FunctionCallUnresolved object...
				delete fcu;
				// ...and replace it with the FunctionCallResolved object.
				(*m_cfg)[*vit].m_statement = fcr;

				// Now add the appropriate CFG edges.
				// The FunctionCall->Function->entrypoint edge.
				CFGEdgeTypeFunctionCall *call_edge_type =
						new CFGEdgeTypeFunctionCall(fcr);
				T_CFG_EDGE_DESC new_edge_desc;
				bool ok;

				boost::tie(new_edge_desc, ok) = boost::add_edge(*vit,
						it->second->GetEntryVertexDescriptor(), *m_cfg);
				if (ok)
				{
					// Edge was added OK, let's connect the edge properties.
					(*m_cfg)[new_edge_desc].m_edge_type = call_edge_type;
				}
				else
				{
					// We couldn't add the edge.  This should never happen.
					std::cerr << "ERROR: Can't add call edge." << std::endl;
				}

				// Add the return edge.
				// The return edge goes from the EXIT of the called function to
				// the node in the CFG which is after the FunctionCall.  There is
				// only ever one normal (i.e. fallthrough) edge from the FunctionCall
				// to the next statement in its containing function.
				T_CFG_EDGE_DESC function_call_out_edge;

				boost::tie(function_call_out_edge, ok) = GetFirstOutEdgeOfType<
						CFGEdgeTypeFallthrough>(*vit, *m_cfg);
				if (!ok)
				{
					// Couldn't find the return.
					std::cerr
							<< "ERROR: COULDN'T FIND OUT EDGE OF TYPE CFGEdgeTypeFallthrough"
							<< std::endl;
					std::cerr << "Edges found are:" << std::endl;
					PrintOutEdgeTypes(*vit, *m_cfg);
				}

				boost::tie(new_edge_desc, ok) = boost::add_edge(
						it->second->GetExitVertexDescriptor(),
						boost::target(function_call_out_edge, *m_cfg), *m_cfg);
				if (ok)
				{
					// Return edge was added OK.  Create and connect the edge's properties.
					CFGEdgeTypeReturn *return_edge_type = new CFGEdgeTypeReturn(
							fcr);
					(*m_cfg)[new_edge_desc].m_edge_type = return_edge_type;
					// Copy the fallthrough edge's properties to the newly-added return edge.
					/// @todo Find a cleaner way to do this.
					return_edge_type->MarkAsBackEdge(
							(*m_cfg)[function_call_out_edge].m_edge_type->IsBackEdge());

					// Change the type of FunctionCall's out edge to a "FunctionCallBypass".
					// For graphing just the function itself, we'll look at these edges and not the
					// call/return edges.
					CFGEdgeTypeFunctionCallBypass *fcbp =
							new CFGEdgeTypeFunctionCallBypass();
					// Copy the fallthrough edge's properties to its replacement.
					/// @todo Find a cleaner way to do this.
					fcbp->MarkAsBackEdge(
							(*m_cfg)[function_call_out_edge].m_edge_type->IsBackEdge());
					delete (*m_cfg)[function_call_out_edge].m_edge_type;
					(*m_cfg)[function_call_out_edge].m_edge_type = fcbp;
				}
				else
				{
					// We couldn't add the edge.  This should never happen.
					std::cerr << "ERROR: Can't add return edge." << std::endl;
				}
			}
		}
	}
}

/// Functor for writing GraphViz dot-compatible info for the function's entire CFG.
struct graph_property_writer
{
	void operator()(std::ostream& out) const
	{
		out << "graph [clusterrank=local colorscheme=svg]" << std::endl;
		out << "node [shape=rectangle fontname=\"Helvetica\"]" << std::endl;
		out << "edge [style=solid]" << std::endl;
	}
};

/**
 * Class for a vertex property writer, for use with write_graphviz().
 */
class cfg_vertex_property_writer
{
public:
	cfg_vertex_property_writer(T_CFG _g) :
			g(_g)
	{
	}

	void operator()(std::ostream& out, const T_CFG_VERTEX_DESC& v)
	{
		if (g[v].m_statement != NULL)
		{
			out << "[label=\"";
			out << g[v].m_statement->GetStatementTextDOT();
			out << "\\n" << g[v].m_statement->GetLocation() << "\"";
			out << ", color=" << g[v].m_statement->GetDotSVGColor();
			out << ", shape=" << g[v].m_statement->GetShapeTextDOT();
			out << "]";
		}
		else
		{
			out << "[label=\"NULL STMNT\"]";
		}
	}
private:

	/// The graph whose vertices we're writing the properties of.
	T_CFG& g;
};

/**
 * Class for an edge property writer, for use with write_graphviz().
 */
class cfg_edge_property_writer
{
public:
	cfg_edge_property_writer(T_CFG _g) :
			g(_g)
	{
	}
	void operator()(std::ostream& out, const T_CFG_EDGE_DESC& e)
	{
		// Set the edge attributes.
		out << "[";
		out << "label=\"" << g[e].m_edge_type->GetDotLabel() << "\"";
		out << ", color=" << g[e].m_edge_type->GetDotSVGColor();
		out << ", style=" << g[e].m_edge_type->GetDotStyle();
		out << "]";
	}
	;
private:

	/// The graph whose edges we're writing the properties of.
	T_CFG& g;
};

class dfs_back_edge_finder_visitor: public boost::default_dfs_visitor
{
public:
	dfs_back_edge_finder_visitor(std::vector<T_CFG_EDGE_DESC> &back_edges) :
			boost::default_dfs_visitor(), m_back_edges(back_edges)
	{
	}
	;

	void back_edge(T_CFG_EDGE_DESC e, const T_CFG &g) const
	{
		m_back_edges.push_back(e);
	}
private:
	/// External vector where we'll store the edges we'll mark later.
	std::vector<T_CFG_EDGE_DESC> &m_back_edges;
};

struct back_edge_filter_predicate
{
	/// Must be default constructible because such predicates are stored by-value.
	back_edge_filter_predicate()
	{
	}
	;
	back_edge_filter_predicate(T_EDGE_TYPE_PROPERTY_MAP &edge_type_property_map) :
			m_edge_type_property_map(edge_type_property_map)
	{
	}
	;
	bool operator()(const T_CFG_EDGE_DESC& eid) const
	{
		if (get(m_edge_type_property_map, eid)->IsBackEdge())
		{
			// This is a back edge, filter it out.
			return false;
		}
		else
		{
			// This is not a back edge.
			return true;
		}
	}
	;

	T_EDGE_TYPE_PROPERTY_MAP m_edge_type_property_map;
};

static void indent(long i)
{
	while (i > 0)
	{
		std::cout << " ";
		i--;
	};
}

static long filtered_in_degree(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
{
	boost::graph_traits<T_CFG>::in_edge_iterator ieit, ieend;

	boost::tie(ieit, ieend) = boost::in_edges(v, cfg);

	long i = 0;
	bool saw_function_call_already = false;
	for (; ieit != ieend; ++ieit)
	{
		if (cfg[*ieit].m_edge_type->IsBackEdge())
		{
			// Always skip anything marked as a back edge.
			continue;
		}

		// Count up all the incoming edges, with two exceptions:
		// - Ignore Return edges.  They will always have exactly one matching FunctionCallBypass, which
		//   is what we'll count instead.
		// - Ignore all but the first CFGEdgeTypeFunctionCall.  The situation here is that we'd be
		//   looking at a vertex v that's an ENTRY statement, with a predecessor of type FunctionCallResolved.
		//   Any particular instance of an ENTRY has at most only one valid FunctionCall edge.
		//   For our current purposes, we only care about this one.
		if ((dynamic_cast<CFGEdgeTypeReturn*>(cfg[*ieit].m_edge_type) == NULL)
				&& (saw_function_call_already == false))
		{
			i++;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(cfg[*ieit].m_edge_type)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return i;
}

static long filtered_out_degree(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
{
	boost::graph_traits<T_CFG>::out_edge_iterator eit, eend;

	boost::tie(eit, eend) = boost::out_edges(v, cfg);

	long i = 0;
	for (; eit != eend; ++eit)
	{
		if (cfg[*eit].m_edge_type->IsBackEdge())
		{
			// Skip anything marked as a back edge.
			continue;
		}
		i++;
	}
	return i;
}

/**
 * Visitor which does the actual creation of the control flow graph.
 */
class function_control_flow_graph_visitor: public CFGDFSVisitor
{
public:
	function_control_flow_graph_visitor(T_CFG &g,
			T_CFG_VERTEX_DESC last_statement) :
			CFGDFSVisitor(g)
	{
		m_last_statement = last_statement;
		m_current_indent_level = 0;
		m_last_discovered_vertex_is_recursive = false;
	}
	;
	function_control_flow_graph_visitor(
			function_control_flow_graph_visitor &original) :
			CFGDFSVisitor(original)
	{
	}
	;
	virtual ~function_control_flow_graph_visitor()
	{
	}
	;

	vertex_return_value_t start_subgraph_vertex(T_CFG_VERTEX_DESC u)
	{
		// Either the very first vertex or a converging vertex has been popped.
		// Pop the corresponding indent level.
		std::cout << m_graph[u].m_statement->GetIdentifierCFG()
				<< " = POPPED CONVERGING NODE" << std::endl;

		m_indent_level_map[u] = m_current_indent_level;

		return vertex_return_value_t::ok;
	}
	;

	vertex_return_value_t discover_vertex(T_CFG_VERTEX_DESC u)
	{
		// We found a new vertex.

		// Get the current indentation level of the vertex.
		m_current_indent_level = m_indent_level_map[u];

		// Indent and print the statement corresponding to this vertex.
		indent(m_current_indent_level);
		StatementBase *p = m_graph[u].m_statement;
		std::cout << p->GetIdentifierCFG() << " <" << p->GetLocation() << ">"
				<< std::endl;
		//PrintInEdgeTypes(u, m_graph);

		if (u == m_last_statement)
		{
			std::cout << "INFO: Found last statement of function" << std::endl;
			// We've reached the end of the function, terminate the search.
			// We should never have to do this, the topological search should always
			// terminate on the EXIT vertex unless there is a branch which erroneously terminates.
			//return terminate_search;				
		}

		// If this vertex results in a branch of the CFG,
		// indent the subsequent vertices another level.
		if (p->IsDecisionStatement())
		{
			indent(m_current_indent_level);
			std::cout << "{" << std::endl;
			m_current_indent_level++;
		}
		else if (dynamic_cast<FunctionCallResolved*>(p) != NULL)
		{
			// This is a function call which has been resolved (i.e. has a link to the
			// actual Function that's being called).  Track the call context, and 
			// check if we're going recursive.
			FunctionCallResolved *fcr;

			fcr = dynamic_cast<FunctionCallResolved*>(p);

			// Are we already within the calling context of the called Function?
			// I.e., are we recursing?
			bool wasnt_already_there;

			// Assume we're not.
			m_last_discovered_vertex_is_recursive = false;

			boost::tie(boost::tuples::ignore, wasnt_already_there) =
					m_call_set.insert(fcr->m_target_function);
			if (!wasnt_already_there)
			{
				// We're recursing, we need to treat this vertex as if it were a FunctionCallUnresolved.
				std::cout << "RECURSION DETECTED" << std::endl;
				m_last_discovered_vertex_is_recursive = true;
				//return vertex_return_value_t::terminate_branch;
			}
		}

		return vertex_return_value_t::ok;
	}

	edge_return_value_t examine_edge(T_CFG_EDGE_DESC ed)
	{
		// Filter out any edges that we want to pretend aren't even part of the
		// graph we're looking at.
		CFGEdgeTypeBase *edge_type;
		CFGEdgeTypeFunctionCall *fc;
		CFGEdgeTypeReturn *ret;
		CFGEdgeTypeFunctionCallBypass *fcb;

		edge_type = m_graph[ed].m_edge_type;

		// Attempt dynamic casts to call/return types to see if we need to handle
		// these specially.
		fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(edge_type);
		ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);
		fcb = dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(edge_type);

		if (edge_type->IsBackEdge())
		{
			// Skip all back edges.
			/// @todo We need to do something about vertices where the only out-edge
			/// is a back edge.  In these cases, the vertex looks like a second EXIT
			/// to the topological sort algorithm.  Maybe add a fake edge to the real
			/// EXIT node, which is ignored for everything but topological sort purposes?
			return edge_return_value_t::terminate_branch;
		}
		else if ((ret != NULL) && (ret->m_function_call != m_call_stack.back()))
		{
			// This edge is a return, but not the one corresponding to the FunctionCall
			// that brought us here.  Skip it.
			return edge_return_value_t::terminate_branch;
		}

		// Handle recursion.
		// We deal with recursion by deciding here which path to take out of a FunctionCallResolved vertex.
		// Note that this is currently the only vertex type which can result in recursion.
		if ((fcb != NULL) && (m_last_discovered_vertex_is_recursive == false))
		{
			// If we're not in danger of infinite recursion,
			// skip FunctionCallBypasses entirely.  Otherwise take them.
			return edge_return_value_t::terminate_branch;
		}
		else if ((fc != NULL)
				&& (m_last_discovered_vertex_is_recursive == true))
		{
			// If we are in danger of infinite recursion,
			// skip FunctionCalls entirely.  Otherwise take them.
			return edge_return_value_t::terminate_branch;
		}

		return edge_return_value_t::ok;
	}

	edge_return_value_t tree_edge(T_CFG_EDGE_DESC ed)
	{
		edge_return_value_t retval = edge_return_value_t::ok;

		// Attempt dynamic casts to call/return types to see if we need to handle
		// these specially.
		CFGEdgeTypeBase *edge_type;
		CFGEdgeTypeFunctionCall *fc;
		CFGEdgeTypeReturn *ret;
		edge_type = m_graph[ed].m_edge_type;
		fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(edge_type);
		ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);

		// If the edge is a function call or return, we have to:
		// - Indent/Outdent another level.
		// - Push/Pop this call onto/off the call stack.
		// - Shift to a new / back to the old "color context".  We have to do this
		//   because the CFG may pass through a function multiple times, and we have
		//   to have clear color maps for each call.
		if (fc != NULL)
		{
			// This edge is a function call.

			// Indent another level.
			m_current_indent_level++;
			//std::cout << "PUSHING CALL: " << fc->m_function_call->GetIdentifier() << std::endl;
			m_call_stack.push_back(fc->m_function_call);
			return edge_return_value_t::push_color_context;
		}
		else if (ret != NULL)
		{
			// This edge is a function return.
			// Pop the call off the m_call_stack and outdent a level.
			//std::cout << "POPPING CALL: " << ret->m_function_call->GetIdentifier() << std::endl;
			FunctionCallResolved *fcr;
			fcr = dynamic_cast<FunctionCallResolved*>(ret->m_function_call);
			if (fcr == NULL)
			{
				// Should never happen.
				std::cerr << "ERROR: Return from unresolved function call."
						<< std::endl;
			}
			m_call_set.erase(fcr->m_target_function);
			m_call_stack.pop_back();
			m_current_indent_level--;
			return edge_return_value_t::pop_color_context;
		}

		return retval;
	}

	vertex_return_value_t prior_to_push(T_CFG_VERTEX_DESC u)
	{
		// Check if this vertex terminates more than one branch of the graph.
		if (filtered_in_degree(u, m_graph) > 1)
		{
			// This edge terminates a branch.  Decrement the current indent level counter.
			m_current_indent_level--;
			indent(m_current_indent_level);
			std::cout << "}" << std::endl;
		}

		m_indent_level_map[u] = m_current_indent_level;
		return vertex_return_value_t::ok;
	}

private:

	/// Vertex corresponding to the last statement of the function.
	/// We'll terminate the search when we find this.
	T_CFG_VERTEX_DESC m_last_statement;

	/// The indent level.  This corresponds to the number of branching statements
	/// with unterminated branches between the starting node and the current node.
	long m_current_indent_level;

	/// The FunctionCall call stack.
	std::vector<FunctionCall*> m_call_stack;

	/// The Function call set.
	/// This is used only to determine if our call stack has gone recursive.
	typedef boost::unordered_set<Function*> T_FUNCTION_CALL_SET;
	T_FUNCTION_CALL_SET m_call_set;

	bool m_last_discovered_vertex_is_recursive;

	std::map<T_CFG_VERTEX_DESC, long> m_indent_level_map;
};

/**
 * Kahn's algorithm for topologically sorting (in this case visiting the nodes of) a graph.
 * 
 * @param graph
 * @param source The vertex to start the graph traversal from.
 * @param visitor
 */
template<typename Graph, class ImprovedDFSVisitor>
void topological_visit_kahn(Graph &graph,
		typename boost::graph_traits<Graph>::vertex_descriptor source,
		ImprovedDFSVisitor &visitor)
{
	// Some convenience typedefs.
	typedef typename boost::graph_traits<Graph>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<Graph>::out_edge_iterator T_OUT_EDGE_ITERATOR;

	// The local variables.
	T_VERTEX_DESC u, v;
	T_OUT_EDGE_ITERATOR ei, eend;
	vertex_return_value_t visitor_vertex_return_value;
	edge_return_value_t visitor_edge_return_value;

	// The set of all vertices with no incoming edges.
	std::stack<T_VERTEX_DESC> no_remaining_in_edges_set;

	// Map of in-degrees.
	typedef std::map<T_VERTEX_DESC, long> T_IN_DEGREE_MAP;
	T_IN_DEGREE_MAP in_degree_map;

	// Start at the source vertex.
	visitor.prior_to_push(source);
	no_remaining_in_edges_set.push(source);
	visitor.start_subgraph_vertex(source);

	while (!no_remaining_in_edges_set.empty())
	{
		// Remove a vertex from the set of in-degree == 0 vertices.
		u = no_remaining_in_edges_set.top();
		no_remaining_in_edges_set.pop();

		// Visit vertex u.  Vertices will be visited in the correct (i.e. not reverse-topologically-sorted) order.
		visitor_vertex_return_value = visitor.discover_vertex(u);
		if (visitor_vertex_return_value
				== vertex_return_value_t::terminate_branch)
		{
			// Visitor wants us to not explore the children of this vertex.
			continue;
		}

		//
		// Decrement the in-degrees of all vertices that are immediate descendants of vertex u.
		//

		// Get iterators to the out edges of vertex u.
		boost::tie(ei, eend) = boost::out_edges(u, graph);

		while (ei != eend)
		{
			// Let the visitor examine the edge *ei.
			visitor_edge_return_value = visitor.examine_edge(*ei);
			switch (visitor_edge_return_value.get_integral_constant_representation())
			{
			case edge_return_value_t::terminate_branch:
			{
				// The visitor wants us to skip this edge.
				// Skip it.  This will make it appear as if it was never in
				// the graph at all.
				++ei;
				continue;
				break;
			}
			case edge_return_value_t::terminate_search:
			{
				/// @todo Stop searching.
				break;
			}
			default:
				break;
			}

			// If the examine_edge() call didn't skip this edge or terminate the
			// graph traversal entirely, the edge is now part of
			// the topologically sorted search graph.  Let the visitor know.
			visitor_edge_return_value = visitor.tree_edge(*ei);
			/// @todo Handle return value.

			//
			// Look up the current in-degree of the target vertex of *ei in the
			// in-degree map.
			//

			// First let's get the target vertex.
			v = boost::target(*ei, graph);

			long id;

			typename T_IN_DEGREE_MAP::iterator it;
			it = in_degree_map.find(v);
			if (it == in_degree_map.end())
			{
				// The target vertex wasn't in the map, which means we haven't
				// encountered it before now.
				// Pretend it was in the map and add it with its original in-degree.
				id = filtered_in_degree(v, graph);
				//std::cout << "Start IND: " << id << " " << v << std::endl;
				//PrintInEdgeTypes(v, graph);
				in_degree_map[v] = id;
				it = in_degree_map.find(v);
			}
			else
			{
				// Vertex was in the map.
				id = it->second;
			}

			// We're "removing" this edge, so decrement the effective in-degree of
			// vertex v.
			--id;

			if (id == 0)
			{
				// This vertex now has an in-degree of zero, push it into the
				// input set.
				no_remaining_in_edges_set.push(v);
				visitor.prior_to_push(v);
				in_degree_map.erase(it);
				//std::cout << "Pushed: " << v << std::endl;
				//PrintInEdgeTypes(v, graph);
			}
			else
			{
				// Store the decremented value back to the map.
				it->second = id;
			}
			// Go to the next out-edge of u.
			++ei;
		}
	}
}

void Function::PrintControlFlowGraph()
{
#if 0
	// Set up the color map stack.
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef std::map< T_CFG_VERTEX_DESC, boost::default_color_type > T_COLOR_MAP;
	std::vector< T_COLOR_MAP* > color_map_stack;

	// Set up the visitor.
	function_control_flow_graph_visitor cfg_visitor(*m_cfg, m_last_statement);

	// Do a depth-first search of the control flow graph.
	improved_depth_first_visit(*m_cfg, m_first_statement, cfg_visitor, color_map_stack);
#else
	// Set up the visitor.
	function_control_flow_graph_visitor cfg_visitor(*m_cfg, m_last_statement);
	topological_visit_kahn(*m_cfg, m_first_statement, cfg_visitor);
#endif
}

void Function::PrintDotCFG(ToolDot *the_dot,
		const boost::filesystem::path& output_dir)
{
	std::string dot_filename;

	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);

	vertex_filter_predicate the_filter(vpm, this);
	boost::filtered_graph<T_CFG, boost::keep_all, vertex_filter_predicate> graph_of_this_function(
			*m_cfg, boost::keep_all(), the_filter);

	dot_filename = (output_dir / (m_function_id + ".dot")).generic_string();

	std::cerr << "Creating " << dot_filename << std::endl;

	std::ofstream outfile(dot_filename.c_str());

	boost::write_graphviz(outfile, graph_of_this_function,
			cfg_vertex_property_writer(*m_cfg),
			cfg_edge_property_writer(*m_cfg), graph_property_writer());

	outfile.close();

	std::cerr << "Compiling " << dot_filename << std::endl;
	the_dot->CompileDotToPNG(dot_filename);
}

bool Function::CreateControlFlowGraph(T_CFG & cfg)
{
	// We create the Control Flow Graph in two stages:
	//
	// - First we go through each basic block and add all Statements to m_cfg,
	//   adding an edge linking each to its predecessor as we go.  We exclude in-edges
	//   and out-edges for the first and last Statement in each block, resp.
	// - Second, we link the last Statement of each block to the first Statement(s) of its Successor(s).
	//
	// A third step is performed to mark all back-edges in the graph, so we can easily
	// identify them during traversals later.

	m_cfg = &cfg;

	std::map<T_BLOCK_GRAPH::vertex_descriptor, T_CFG_VERTEX_DESC> first_statement_of_block;
	std::vector<T_CFG_VERTEX_DESC> last_statement_of_block;
	bool ok;

	// Do the first step.
	T_BLOCK_GRAPH::vertex_iterator vit, vend;
	for (boost::tie(vit, vend) = boost::vertices(m_block_graph); vit != vend;
			vit++)
			{
		Block::T_STATEMENT_LIST_ITERATOR sit;
		T_CFG_VERTEX_DESC last_vid;
		bool is_first = true;

		// Iterate over all Statements in this Block.
		for (sit = m_block_graph[*vit].m_block->begin();
				sit != m_block_graph[*vit].m_block->end(); sit++)
				{
			// Add this Statement to the Control Flow Graph.
			T_CFG_VERTEX_DESC vid;
			vid = boost::add_vertex(*m_cfg);
			(*m_cfg)[vid].m_statement = *sit;
			(*m_cfg)[vid].m_containing_function = this;

			if (!is_first)
			{
				// Add an edge to its predecessor.
				T_CFG_EDGE_DESC eid;

				boost::tie(eid, ok) = boost::add_edge(last_vid, vid, *m_cfg);
				// Since this edge is within the block, it is just a fallthrough.
				(*m_cfg)[eid].m_edge_type = new CFGEdgeTypeFallthrough();
			}
			else
			{
				// This is the first statement from this block.  Save it for stage 2.
				first_statement_of_block[*vit] = vid;
				is_first = false;

				if (m_block_graph[*vit].m_block->IsENTRY())
				{
					// This is the first statement of the ENTRY block.  Save the
					// vertex_descriptor for use later.
					m_first_statement = vid;
				}
			}

			// It's OK to save the vertex descriptor for next time.  Per the Boost
			// docs, neither add_vertex() nor add_edge() invalidate vertex or edge
			// descriptors.
			last_vid = vid;
		}

		// Save the vertex descriptor of the last statement of this block for the next stage.
		last_statement_of_block.push_back(last_vid);

		if (m_block_graph[*vit].m_block->IsEXIT())
		{
			// This is the last statement of the EXIT block.
			m_last_statement = last_vid;
		}
	}

	// Do the second step.
	std::vector<T_CFG_VERTEX_DESC>::iterator last_statement_it;
	std::map<T_BLOCK_GRAPH::vertex_descriptor, T_CFG_VERTEX_DESC>::iterator first_statement_it;
	last_statement_it = last_statement_of_block.begin();
	for (boost::tie(vit, vend) = boost::vertices(m_block_graph); vit != vend;
			vit++)
			{
		T_BLOCK_GRAPH::out_edge_iterator eit, eend;
		for (boost::tie(eit, eend) = boost::out_edges(*vit, m_block_graph);
				eit != eend; eit++)
				{
			// Add an edge from the last statement of Block vit to the first statement
			// of the Block pointed to by eit.
			T_CFG_VERTEX_DESC target_vertex_descr = boost::target(*eit,
					m_block_graph);

			first_statement_it = first_statement_of_block.find(
					target_vertex_descr);

			if (first_statement_it == first_statement_of_block.end())
			{
				std::cerr << "ERROR: No first block statement found."
						<< std::endl;
			}
			else
			{
				// Add the edge.
				T_CFG_EDGE_DESC new_edge_desc;
				boost::tie(new_edge_desc, ok) = boost::add_edge(
						*last_statement_it, first_statement_it->second, *m_cfg);

				/// @todo Stubbing in fallthrough type for now, change this to add the real type.
				(*m_cfg)[new_edge_desc].m_edge_type =
						new CFGEdgeTypeFallthrough();
			}
		}
		last_statement_it++;
	}

	// Third step.  CFG is created.  Look for back edges and set their edge types appropriately.
	/// @todo We need to do something about vertices where the only out-edge
	/// is a back edge.  In these cases, the vertex looks like a second EXIT
	/// to the topological sort algorithm.  Maybe add a fake edge to the real
	/// EXIT node, which is ignored for everything but topological sort purposes?

	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);
	std::vector<T_CFG_EDGE_DESC> back_edges;

	// Define a filtered view of this function's CFG, which hides the back-edges
	// so that we can produce a topological sort.
	dfs_back_edge_finder_visitor back_edge_finder(back_edges);

	vertex_filter_predicate the_vertex_filter(vpm, this);

	// Find all the back edges.
	boost::depth_first_search(*m_cfg, boost::visitor(back_edge_finder));

	// Mark them as back edges.
	BOOST_FOREACH(T_CFG_EDGE_DESC e, back_edges)
	{
		// Change this edge type to a back edge.
		(*m_cfg)[e].m_edge_type->MarkAsBackEdge(true);

		// If the source node of this back edge now has no out-edges,
		// add a FalseExit edge to it, so topological sorting works correctly.
		T_CFG_VERTEX_DESC src;
		src = boost::source(e, *m_cfg);
		if (boost::out_degree(src, *m_cfg) == 1)
		{
			T_CFG_EDGE_DESC newedge;
			boost::tie(newedge, boost::tuples::ignore) =
					boost::add_edge(src, m_last_statement, *m_cfg);
			(*m_cfg)[newedge].m_edge_type = new CFGEdgeTypeImpossible;
		}
	}

	{
		typedef boost::filtered_graph<T_CFG, boost::keep_all,
				vertex_filter_predicate> T_FILTERED_GRAPH;
		T_FILTERED_GRAPH graph_of_this_function(*m_cfg, boost::keep_all(),
				the_vertex_filter);

		boost::graph_traits<T_FILTERED_GRAPH>::vertex_iterator vit, vend;
		boost::tie(vit, vend) = boost::vertices(graph_of_this_function);
		for (; vit != vend; vit++)
		{
			long id, od;
			id = boost::in_degree(*vit, graph_of_this_function);
			od = boost::out_degree(*vit, graph_of_this_function);

			if ((id == 0) && (*vit != m_first_statement))
			{
				T_CFG_EDGE_DESC newedge;
				std::cerr << "WARNING: Non-ENTRY vertex with in-degree of 0 in function \""
						<< GetIdentifier() << "\", adding Impossible edge to ENTRY." << std::endl;
				boost::tie(newedge, boost::tuples::ignore) =
						boost::add_edge(m_first_statement, *vit, *m_cfg);
				(*m_cfg)[newedge].m_edge_type = new CFGEdgeTypeImpossible;
			}

			if ((od == 0) && (*vit != m_last_statement))
			{
				std::cerr
						<< "ERROR: Non-EXIT vertex with out-degree of 0 in function \""
						<< GetIdentifier() << "\"" << std::endl;
			}
		}
	}

	return true;
}
