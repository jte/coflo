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

#include "debug_utils/debug_utils.hpp"

#include "TranslationUnit.h"
#include "Block.h"
#include "Function.h"
#include "SuccessorTypes.h"

#include "controlflowgraph/statements/statements.h"
#include "controlflowgraph/edges/edge_types.h"
#include "controlflowgraph/ControlFlowGraph.h"
#include "controlflowgraph/BackEdgeFixupVisitor.h"

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
						dlog_block << "INFO: Found EXIT successor." << std::endl;
						(*s)->SetSuccessorBlockPtr(m_exit_block);
					}
					else if (NULL != dynamic_cast<SuccessorNoReturn*>(*s))
					{
						dlog_block << "INFO: Found NoReturn successor."
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

				boost::tie(function_call_out_edge, ok) = GetFirstOutEdgeOfType<CFGEdgeTypeFallthrough>(*vit, *m_cfg);
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
		std::cout << "    ";
		i--;
	};
}

static long filtered_in_degree(T_CFG_VERTEX_DESC v, const T_CFG &cfg, bool only_decision_predecessors = false)
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

		if(only_decision_predecessors)
		{
			// Is the predecessor a decision statement?
			if(!cfg[boost::source(*ieit,cfg)].m_statement->IsDecisionStatement())
			{
				continue;
			}
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

T_CFG_EDGE_DESC first_filtered_out_edge(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
{
	boost::graph_traits<T_CFG>::in_edge_iterator ieit, ieend;

	boost::tie(ieit, ieend) = boost::in_edges(v, cfg);

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
			return *ieit;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(cfg[*ieit].m_edge_type)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return *ieend;
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

using std::cerr;
using std::cout;
using std::endl;

/**
 * Visitor which prints out the control flow graph.
 */
class function_control_flow_graph_visitor: public CFGDFSVisitor
{
public:
	function_control_flow_graph_visitor(T_CFG &g,
			T_CFG_VERTEX_DESC last_statement,
			bool cfg_verbose) :
			CFGDFSVisitor(g)
	{
		m_last_statement = last_statement;
		m_cfg_verbose = cfg_verbose;
		m_next_function_call_resolved = NULL;
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
		// The very first vertex has been popped.

		// Indent a level.


		//indent(0);
		//std::cout << "{" << std::endl;

		/*IndentStackContext isc;
		isc.m_v = u;
		isc.m_number_of_unterminated_new_branches = filtered_out_degree(u, m_graph);
		isc.m_indent_level = 1;
		m_call_stack_2.top().m_indent_stack.push(isc);*/

		return vertex_return_value_t::ok;
	};

	vertex_return_value_t discover_vertex(T_CFG_VERTEX_DESC u)
	{
		// We found a new vertex.

		StatementBase *p = m_graph[u].m_statement;

		if(p->IsType<Entry>())
		{
			// We're at a function entry point.

			long indent_level;
			if(m_call_stack_2.size() == 0)
			{
				// We're at the first function entry point.
				indent_level = 1;
				m_next_function_call_resolved = NULL;
			}
			else
			{
				// We'll start at an indent level of whatever the calling context's was
				// plus one.
				indent_level = m_call_stack_2.top().m_indent_stack.top().m_indent_level + 1;
			}

			indent(indent_level-1);
			std::cout << "[" << std::endl;

			FunctionCallContext fcc(m_next_function_call_resolved, u, filtered_out_degree(u, m_graph), indent_level);
			m_call_stack_2.push(fcc);
		}


		// Check if this vertex starts a new branch of the cfg.
		long fid = filtered_in_degree(u, m_graph);
		T_CFG::in_edge_iterator iei,iee;
		boost::tie(iei, iee) = boost::in_edges(u, m_graph);
		if(fid==1 && iei != iee)
		{
			T_CFG::vertex_descriptor predecessor;
			predecessor = boost::source(*iei, m_graph);
			if(m_graph[predecessor].m_statement->IsDecisionStatement())
			{
				// Predecessor was a decision statement, so this vertex starts a new branch.
				// Print a block start marker and increment the current indent level.
				indent(m_call_stack_2.top().m_indent_stack.top().m_indent_level-1);
				std::cout << "{" /* << " v=" << m_indent_stack.top().m_v */ << std::endl;
			}
		}

		// Check if this vertex meets the criteria for printing the statement.
		if(m_cfg_verbose || (p->IsDecisionStatement() || (p->IsFunctionCall())))
		{
			// Indent and print the statement corresponding to this vertex.
			//std::cout << u << std::endl;
			indent(m_call_stack_2.top().m_indent_stack.top().m_indent_level);
			std::cout << p->GetIdentifierCFG() << " <" << p->GetLocation() << ">" << std::endl;
			//PrintInEdgeTypes(u, m_graph);
			if(p->IsDecisionStatement())
			{
				// There's more than one out edge.  Push our indent level info, we'll need it for each branch.
				PushIndentStack(u);
			}
		}

		if (u == m_last_statement)
		{
			std::clog << "INFO: Found last statement of function" << std::endl;
			// We've reached the end of the function, terminate the search.
			// We should never have to do this, the topological search should always
			// terminate on the EXIT vertex unless there is a branch which erroneously terminates.
			//return terminate_search;				
		}

		if (p->IsType<FunctionCallResolved>())
		{
			// This is a function call which has been resolved (i.e. has a link to the
			// actual Function that's being called).  Track the call context, and 
			// check if we're going recursive.
			FunctionCallResolved *fcr;

			fcr = dynamic_cast<FunctionCallResolved*>(p);

			// Let the next ENTRY know what function call it's coming from.
			m_next_function_call_resolved = fcr;

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
				std::cout << "RECURSION DETECTED: Function \"" << fcr->m_target_function << "\"" << std::endl;
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
			return edge_return_value_t::terminate_branch;
		}

		if (ret != NULL)
		{
			// This is a return edge.

			if(m_call_stack_2.empty())
			{
				// Should never get here.
				cout << "EMPTY" << endl;
			}

			if(m_call_stack_2.top().m_function_call_which_pushed_context == NULL)
			{
				// We're at the top of the call stack, and we're trying to return.
				cout << "NULL" << endl;
				//return edge_return_value_t::terminate_branch;
			}
			else if(ret->m_function_call != m_call_stack_2.top().m_function_call_which_pushed_context)
			{
				// This edge is a return, but not the one corresponding to the FunctionCall
				// that brought us here.  Or, the call stack is empty, indicating that we got here
				// by means other than tracing the control-flow graph (e.g. we started tracing the
				// graph at an internal vertex).
				// Skip it.
				/// @todo An empty call stack here could also be an error in the program.  We should maybe
				/// add a fake "call" when starting a cfg trace from an internal vertex.
				//cout << "t1" << endl;
				//cout << ret->m_function_call->GetIdentifier() << endl;
				//cout << m_call_stack_2.top().m_function_call_which_pushed_context->GetIdentifier() << endl;
				return edge_return_value_t::terminate_branch;
			}
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
			cout << "t3" << endl;
			return edge_return_value_t::terminate_branch;
		}

		return edge_return_value_t::ok;
	}

	edge_return_value_t tree_edge(T_CFG_EDGE_DESC ed)
	{
		edge_return_value_t retval = edge_return_value_t::ok;

		return retval;
	}

	vertex_return_value_t prior_to_push(T_CFG_VERTEX_DESC u, T_CFG_EDGE_DESC e)
	{
		// We're just about to push this vertex onto the topological sort stack.

		return vertex_return_value_t::ok;
	}

	void vertex_visit_complete(T_CFG_VERTEX_DESC u, long num_vertices_pushed, T_CFG_EDGE_DESC e)
	{
		// Check if the vertex we terminate on has more than just us coming in.
		if((num_vertices_pushed == 0) ||
				((num_vertices_pushed == 1) && (filtered_in_degree(boost::target(e, m_graph), m_graph) > 1)))
		{
			// This edge will end on a merge vertex.  Pop our indent stack.
			//std::cerr << "TERM vvc edge=" << e << std::endl;
			PopIndentStack();
		}

		StatementBase *p = m_graph[u].m_statement;

		if(p->IsType<Exit>())
		{
			// We're leaving the function we were in.

			// Remove it from the "functions we're in" set.
			m_call_set.erase(m_call_stack_2.top().m_function_call_which_pushed_context->m_target_function);

			m_call_stack_2.pop();
			//cout << "Exit" << endl;
			long indent_level;
			if(m_call_stack_2.size() == 0)
			{
				// We're at the first function entry point.
				indent_level = 0;
			}
			else
			{
				// We'll start at an indent level of whatever the calling context's was
				// plus one.
				indent_level = m_call_stack_2.top().m_indent_stack.top().m_indent_level;
			}

			indent(indent_level);
			std::cout << "]" << std::endl;
		}
	}

private:

	void PushIndentStack(T_CFG_VERTEX_DESC u)
	{
		IndentStackContext isc;
		isc.m_v = u;
		isc.m_number_of_unterminated_new_branches = filtered_out_degree(u, m_graph);
		if(m_call_stack_2.top().m_indent_stack.size() == 0) std::cerr << "ERROR: Indent stack empty." << std::endl;
		isc.m_indent_level = m_call_stack_2.top().m_indent_stack.top().m_indent_level + 1;
		m_call_stack_2.top().m_indent_stack.push(isc);
		//std::cout << "PUSH " << u << "," << isc.m_number_of_unterminated_new_branches << std::endl;
	}

	void PopIndentStack()
	{
		while(m_call_stack_2.top().m_indent_stack.size() > 1)
		{
			indent(m_call_stack_2.top().m_indent_stack.top().m_indent_level-1);
			std::cout << "}" /*<< " vvc " << u*/ << std::endl;

			m_call_stack_2.top().m_indent_stack.top().m_number_of_unterminated_new_branches--;
			if(m_call_stack_2.top().m_indent_stack.top().m_number_of_unterminated_new_branches == 0)
			{
				// The branches from this decision vertex have all been terminated.
				//std::cerr << "POP " << m_indent_stack.top().m_v << std::endl;
				m_call_stack_2.top().m_indent_stack.pop();
			}
			else
			{
				//std::cerr << "DEC " << m_indent_stack.top().m_v << std::endl;
				break;
			}
		}
		if(m_call_stack_2.top().m_indent_stack.size() == 0) std::cerr << "ERROR: Indent stack empty." << std::endl;
	}

	/// Vertex corresponding to the last statement of the function.
	/// We'll terminate the search when we find this.
	T_CFG_VERTEX_DESC m_last_statement;

	/// Flag indicating if we should only print function calls and flow control constructs.
	bool m_cfg_verbose;

	struct IndentStackContext
	{
		/// The vertex that pushed this context.
		T_CFG_VERTEX_DESC m_v;
		long m_number_of_unterminated_new_branches;
		long m_indent_level;
	};

	typedef std::stack<IndentStackContext> T_INDENT_STACK;

	struct FunctionCallContext
	{
		/// The FunctionCallResolved statement which resulted in this context being pushed.
		FunctionCallResolved* m_function_call_which_pushed_context;

		/// Each function call context gets its own indent stack.
		/// This is mainly so that the cases where we're going to pop
		/// the indent stack back to the "ground state" don't have to be handled specially
		/// (see PopIndentStack()).
		T_INDENT_STACK m_indent_stack;

		/**
		 * Constructor to make pushing new contexts a bit easier.
		 *
		 * @param function_call Pointer to the FunctionCall statement which is pushing this context.
		 */
		FunctionCallContext(FunctionCallResolved* function_call,
				T_CFG_VERTEX_DESC u,
				long number_of_unterminated_new_branches,
				long indent_level)
		{
			m_function_call_which_pushed_context = function_call;
			IndentStackContext isc;
			isc.m_v = u;
			isc.m_number_of_unterminated_new_branches = number_of_unterminated_new_branches;
			isc.m_indent_level = indent_level;
			m_indent_stack.push(isc);
		};
	};

	/// The FunctionCall call stack.
	std::stack<FunctionCallContext> m_call_stack_2;

	/// The Function call set.
	/// This is used only to determine if our call stack has gone recursive.
	typedef boost::unordered_set<Function*> T_FUNCTION_CALL_SET;
	T_FUNCTION_CALL_SET m_call_set;

	FunctionCallResolved *m_next_function_call_resolved;
	bool m_last_discovered_vertex_is_recursive;
};


void Function::PrintControlFlowGraph(bool cfg_verbose)
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
	function_control_flow_graph_visitor cfg_visitor(*m_cfg, m_last_statement, cfg_verbose);
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

	std::clog << "Creating " << dot_filename << std::endl;

	std::ofstream outfile(dot_filename.c_str());

	boost::write_graphviz(outfile, graph_of_this_function,
			cfg_vertex_property_writer(*m_cfg),
			cfg_edge_property_writer(*m_cfg), graph_property_writer());

	outfile.close();

	std::clog << "Compiling " << dot_filename << std::endl;
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
				std::cerr << "ERROR: No first block statement found." << std::endl;
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
	FixupBackEdges();



	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);
	vertex_filter_predicate the_vertex_filter(vpm, this);

	// Check CFG for inconsistencies and clean up the ones we can.
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

		{
			std::vector<T_CFG_EDGE_DESC> edges_to_remove;

			// Find any critical edges and split them by inserting NOOPs.
			boost::graph_traits<T_FILTERED_GRAPH>::edge_iterator eit, eend;

			boost::tie(eit, eend) = boost::edges(graph_of_this_function);
			for (; eit != eend; ++eit)
			{
				T_CFG_VERTEX_DESC source_vertex_desc, target_vertex_desc;
				long target_id, source_od;

				// Get the vertex descriptors.
				source_vertex_desc = boost::source(*eit, graph_of_this_function);
				target_vertex_desc = boost::target(*eit, graph_of_this_function);

				// Get the effective in and out degrees.
				source_od = filtered_out_degree(source_vertex_desc, *m_cfg);
				target_id = filtered_in_degree(target_vertex_desc, *m_cfg);

				// Check if they meet the criteria for a critical edge.
				if((source_od > 1) && (target_id > 1))
				{
					// They do, we've found a critical edge.
					edges_to_remove.push_back(*eit);
				}
			}

			// Remove the critical edges we found.
			BOOST_FOREACH(T_CFG_EDGE_DESC e, edges_to_remove)
			{
				T_CFG_VERTEX_DESC source_vertex_desc, target_vertex_desc, splitting_vertex;

				// Get the vertex descriptors.
				source_vertex_desc = boost::source(e, graph_of_this_function);
				target_vertex_desc = boost::target(e, graph_of_this_function);

				// Create the new NoOp vertex.
				splitting_vertex = boost::add_vertex(*m_cfg);
				(*m_cfg)[splitting_vertex].m_statement = new NoOp(Location("[UNKNOWN : 0]"));
				(*m_cfg)[splitting_vertex].m_containing_function = this;

				// Split the edge by pointing the old edge at the new vertex, and a new fallthrough
				// edge from the new vertex to the old target.
				T_CFG_EDGE_DESC new_edge_1, new_edge_2;
				boost::tie(new_edge_1, boost::tuples::ignore) = boost::add_edge(source_vertex_desc, splitting_vertex, *m_cfg);
				boost::tie(new_edge_2, boost::tuples::ignore) = boost::add_edge(splitting_vertex, target_vertex_desc, *m_cfg);
				(*m_cfg)[new_edge_1].m_edge_type = (*m_cfg)[e].m_edge_type;
				(*m_cfg)[e].m_edge_type = NULL;
				(*m_cfg)[new_edge_2].m_edge_type = new CFGEdgeTypeFallthrough();
				boost::remove_edge(e, *m_cfg);
			}
		}
	}

	return true;
}

void Function::FixupBackEdges()
{
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);
	vertex_filter_predicate the_vertex_filter(vpm, this);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
			T_FILTERED_GRAPH graph_of_this_function(*m_cfg, boost::keep_all(),
					the_vertex_filter);

	std::vector<BackEdgeFixupVisitor::BackEdgeFixupInfo> back_edges;

	// Define a filtered view of this function's CFG, which hides the back-edges
	// so that we can produce a topological sort.
	BackEdgeFixupVisitor back_edge_finder(back_edges);

	// Find all the back edges.
	boost::depth_first_search(*m_cfg, boost::visitor(back_edge_finder));

	// Mark them as back edges.
	BOOST_FOREACH(BackEdgeFixupVisitor::BackEdgeFixupInfo fixinfo, back_edges)
	{
		T_CFG_EDGE_DESC e = fixinfo.m_back_edge;

		// Change this edge type to a back edge.
		(*m_cfg)[e].m_edge_type->MarkAsBackEdge(true);

		// If the source node of this back edge now has no non-back-edge out-edges,
		// add a CFGEdgeTypeImpossible edge to it, so topological sorting works correctly.
		T_CFG_VERTEX_DESC src;
		src = boost::source(e, *m_cfg);
		if (boost::out_degree(src, *m_cfg) == 1)
		{
			T_CFG_EDGE_DESC newedge;
			boost::tie(newedge, boost::tuples::ignore) =
					boost::add_edge(src, fixinfo.m_impossible_target_vertex, *m_cfg);
			(*m_cfg)[newedge].m_edge_type = new CFGEdgeTypeImpossible;
		}
	}
}


