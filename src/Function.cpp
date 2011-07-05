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
 
#include <iostream>
#include <fstream>
#include <map>
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
#include "controlflowgraph/CFGEdgeTypeGotoBackEdge.h"
#include "controlflowgraph/CFGEdgeTypeReturn.h"
#include "controlflowgraph/CFGEdgeTypeFunctionCallBypass.h"
#include "ControlFlowGraph.h"
#include "CFGDFSVisitor.h"

/// Property map typedef which allows us to get at the function pointer stored at
/// CFGVertexProperties::m_containing_function in the T_CFG.
typedef boost::property_map< T_CFG, Function* CFGVertexProperties::* >::type T_VERTEX_PROPERTY_MAP;

/// Property map typedef which allows us to get at the edge type pointer stored at
/// CFGEdgeProperties::m_edge_type in the T_CFG.
typedef boost::property_map< T_CFG, CFGEdgeTypeBase* CFGEdgeProperties::* >::type T_EDGE_TYPE_PROPERTY_MAP;

struct vertex_filter_predicate
{
	vertex_filter_predicate() { };
	vertex_filter_predicate(T_VERTEX_PROPERTY_MAP vertex_prop_map, Function *parent_function)
		: m_vertex_prop_map(vertex_prop_map), m_parent_function(parent_function) {};
	bool operator()(const T_CFG_VERTEX_DESC& vid) const
	{
		if(m_parent_function == get(m_vertex_prop_map, vid))
		{
			// This vertex belongs to the function we're concerned with.
			return true;
		}
		else
		{
			return false;
		}
	};
	
	T_VERTEX_PROPERTY_MAP m_vertex_prop_map;
	Function *m_parent_function;
};
 
Function::Function(const std::string &function_id)
{
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

bool Function::IsCalled() const
{
	// Determine if this function is ever called.
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
	std::vector< Successor* > successor_list;
	
	// Add the entry pseudoblock.
	m_entry_block->AddSuccessors("2 (fallthru)");
	
	// Go through all the blocks and add them to the link map.
	BOOST_FOREACH(Block *bp, m_block_list)
	{
		// Add this block to the number->Block* map.
		linkmap[bp->GetBlockNumber()] = bp;

		// Collect all the blocks' Successors into a list.
		Block::T_BLOCK_SUCCESSOR_ITERATOR s;
		for(s = bp->successor_begin(); s != bp->successor_end(); s++)
		{
			if(NULL != dynamic_cast<SuccessorExit*>(*s))
			{
				std::cout << "INFO: Found EXIT successor." << std::endl;
				(*s)->SetSuccessorBlockPtr(m_exit_block);
			}
			else
			{
				successor_list.push_back(*s);
			}
		}
	}

	// Block 2 appears to always be the entry block, set it.
	/// \todo We really should check for "PRED: ENTRY" to make sure of this.
	T_BLOCK_LINK_MAP_ITERATOR lmit = linkmap.find(2);
	if(lmit == linkmap.end())
	{
		std::cerr << "ERROR: Can't find Block 2." << std::endl;
	}

	// Now go through all the Successors and link them to the Blocks they refer to.
	std::vector< Successor* >::iterator it2;
	for(it2 = successor_list.begin(); it2 != successor_list.end(); it2++)
	{
		long block_no;
		T_BLOCK_LINK_MAP_ITERATOR linkmap_it;

		block_no = (*it2)->GetSuccessorBlockNumber();

		// Look up the block in the map.
		linkmap_it = linkmap.find(block_no);

		if(linkmap_it == linkmap.end())
		{
			std::cerr << "ERROR: Can't find Block " << block_no << " in linkmap." << std::endl;
		}
		else if(linkmap_it->second == NULL)
		{
			std::cerr << "ERROR: Found a Successor with no Block pointer." << std::endl;
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
	typedef std::map< Block*, T_BLOCK_GRAPH_VERTEX_DESC > T_BLOCK_MAP;
	T_BLOCK_MAP block_map;	
	
	// Add the function blocks into the m_block_graph.	
	BOOST_FOREACH(Block *bp, m_block_list)
	{
		// this_block_id is the new Vertex.
		T_BLOCK_GRAPH_VERTEX_DESC this_block_id = boost::add_vertex(m_block_graph);
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
		for(s = bp->successor_begin(); s != bp->successor_end(); s++)
		{
			T_BLOCK_MAP::iterator it;
			it = block_map.find((*s)->GetSuccessorBlockPtr());
			if(it != block_map.end())
			{
				// Found the next block.
				next_block = it->second;
				T_BLOCK_GRAPH_EDGE_DESC edge;
				bool ok;
				boost::tie(edge, ok) = boost::add_edge(this_block, next_block, m_block_graph);
				// Check if there's no error, then set up the edge properties.
				if (ok)
				{
					// Label the edge with whatever the Successor object has
					// for a label.
					if((*s)->HasEdgeLabel())
					{
						m_block_graph[edge].m_edge_text = (*s)->GetEdgeLabel();
					}
				}
			}
			else
			{
				// Couldn't find the next block, it's probably EXIT.
				std::cerr << "ERROR: Couldn't find next_block in block_map" << std::endl;
			}
		}
	}
}


void PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg)
{
	T_CFG_OUT_EDGE_ITERATOR ei, eend;
	
	boost::tie(ei, eend) = boost::out_edges(vdesc, cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(cfg[*ei].m_edge_type)).name() << std::endl;
	}
}


void Function::Link(const std::map< std::string, Function* > &function_map,
		std::vector< FunctionCall* > *unresolved_function_calls)
{
	T_VERTEX_PROPERTY_MAP vpm = boost::get(&CFGVertexProperties::m_containing_function, *m_cfg);

	vertex_filter_predicate the_filter(vpm, this);
	typedef boost::filtered_graph<T_CFG, boost::keep_all, vertex_filter_predicate> T_FILTERED_CFG;
	T_FILTERED_CFG graph_of_this_function(*m_cfg, boost::keep_all(), the_filter);
	
	boost::graph_traits< T_FILTERED_CFG >::vertex_iterator vit, vend;
	boost::tie(vit, vend) = boost::vertices(graph_of_this_function);
	for(; vit != vend; vit++)
	{
		FunctionCallUnresolved *fcu = dynamic_cast<FunctionCallUnresolved*>((*m_cfg)[*vit].m_statement);
		if(fcu != NULL)
		{
			std::map< std::string, Function* >::const_iterator it;
				
			// We found an unresolved function call.  Try to resolve it.
			it = function_map.find(fcu->GetIdentifier());

			if(it == function_map.end())
			{
				// Couldn't resolve it.  Let the caller know.
				std::cerr << "WARNING: Can't find function " << fcu->GetIdentifier() << " in link map." << std::endl;
				unresolved_function_calls->push_back(fcu);
			}
			else
			{
				// Found it.
				// Replace the FunctionCallUnresolved with a FunctionCallResolved.
				FunctionCallResolved *fcr = new FunctionCallResolved(it->second, fcu);
				// Delete the FunctionCallUnresolved object...
				delete fcu;
				// ...and replace it with the FunctionCallResolved object.
				(*m_cfg)[*vit].m_statement = fcr;
				
				// Now add the appropriate CFG edges.
				// The FunctionCall->Function->entrypoint edge.
				CFGEdgeTypeFunctionCall *call_edge_type = new CFGEdgeTypeFunctionCall(fcr);
				T_CFG_EDGE_DESC new_edge_desc;
				bool ok;
				
				boost::tie(new_edge_desc, ok) =	boost::add_edge(*vit, it->second->GetEntryVertexDescriptor(), *m_cfg);
				if(ok)
				{
					// Edge was added OK, let's connect the edge properties.
					std::cerr << "INFO: Adding FunctionCall edge from " 
						<< GetIdentifier() << " [" << *fcr->GetLocation() << "] to "
						<< (*m_cfg)[it->second->GetEntryVertexDescriptor()].m_containing_function->GetIdentifier()
						<< std::endl;
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
				PrintOutEdgeTypes(*vit, *m_cfg);
				if(!ok)
				{
					// Couldn't find the return.
					std::cerr << "ERROR: COULDN'T FIND OUT EDGE OF TYPE CFGEdgeTypeFallthrough" << std::endl;			
				}
				
				boost::tie(new_edge_desc, ok) = boost::add_edge(it->second->GetExitVertexDescriptor(),
					boost::target(function_call_out_edge, *m_cfg),
					*m_cfg);
				if(ok)
				{
					// Return edge was added OK.  Create and connect the edge's properties.
					CFGEdgeTypeReturn *return_edge_type = new CFGEdgeTypeReturn(fcr);
					(*m_cfg)[new_edge_desc].m_edge_type = return_edge_type;
					
					// Change the type of FunctionCall's out edge to a "FunctionCallBypass".
					// For graphing just the function itself, we'll look at these edges and not the
					// call/return edges.
					delete (*m_cfg)[function_call_out_edge].m_edge_type;
					(*m_cfg)[function_call_out_edge].m_edge_type = new CFGEdgeTypeFunctionCallBypass();
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


/// Class for a vertex property writer, for use with write_graphviz().
class cfg_vertex_property_writer
{
public:
	cfg_vertex_property_writer(T_CFG _g) : g(_g) {}

	void operator()(std::ostream& out, const T_CFG_VERTEX_DESC& v) 
	{
		if(g[v].m_statement != NULL)
		{
			out << "[label=\"";
			out << g[v].m_statement->GetStatementTextDOT();
			out << "\\n" << *(g[v].m_statement->GetLocation()) << "\"";
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
	T_CFG& g;
};

/// Class for an edge property writer, for use with write_graphviz().
class cfg_edge_property_writer
{
public:
	cfg_edge_property_writer(T_CFG _g) : g(_g) {}
	void operator()(std::ostream& out, const T_CFG_EDGE_DESC& e) 
	{
		// Set the edge attributes.
		out << "[";
		out << "label=\"" << g[e].m_edge_type->GetDotLabel() << "\"";
		out << ", color=" << g[e].m_edge_type->GetDotSVGColor();
		out << "]";
	};
private:
	T_CFG& g;
};


class dfs_back_edge_finder_visitor : public boost::default_dfs_visitor
{
public:
	dfs_back_edge_finder_visitor(std::vector<T_CFG_EDGE_DESC> &back_edges) : boost::default_dfs_visitor(), m_back_edges(back_edges)
	{ };
	
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
	/// Must be default constructable because such predicates are stored by-value.
	back_edge_filter_predicate() {};
	back_edge_filter_predicate(T_EDGE_TYPE_PROPERTY_MAP &edge_type_property_map) : m_edge_type_property_map(edge_type_property_map) 
	{};
	bool operator()(const T_CFG_EDGE_DESC& eid) const
	{
		if(get(m_edge_type_property_map, eid)->IsBackEdge())
		{
			// This is a back edge, filter it out.
			return false;
		}
		else
		{
			// This is not a back edge.
			return true;
		}
	};
	
	T_EDGE_TYPE_PROPERTY_MAP m_edge_type_property_map;
};

void indent(long i)
{
	while(i>0)
	{
		std::cout << " ";
		i--;
	};
}

struct VertexInfo
{
	/**
	 * Set the VertexInfo's members to the given values.
     */
	void Set(T_CFG_VERTEX_DESC v,
		T_CFG_OUT_EDGE_ITERATOR ei,
		T_CFG_OUT_EDGE_ITERATOR eend//,
		/*long indent_level*/
		)
	{
		m_v = v;
		m_ei = ei;
		m_eend = eend;
		//m_indent_level = indent_level;
	};
	
	T_CFG_VERTEX_DESC m_v;
	T_CFG_OUT_EDGE_ITERATOR m_ei;
	T_CFG_OUT_EDGE_ITERATOR m_eend;
	//long m_indent_level;
} vertex_info;

long filtered_in_degree(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
{
	boost::graph_traits< T_CFG >::in_edge_iterator ieit, ieend;
	
	boost::tie(ieit, ieend) = boost::in_edges(v, cfg);
	
	long i = 0;
	bool saw_function_call_already = false;
	for(; ieit!=ieend; ++ieit)
	{
		if((dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(cfg[*ieit].m_edge_type) == NULL) &&
		 (saw_function_call_already == false))
		{
			i++;
		}
		
		if(dynamic_cast<CFGEdgeTypeFunctionCall*>(cfg[*ieit].m_edge_type) != NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}
	return i;
}

class function_control_flow_graph_visitor : public CFGDFSVisitor
{
public:
	function_control_flow_graph_visitor(T_CFG &g, T_CFG_VERTEX_DESC last_statement) : CFGDFSVisitor(g)
	{
		m_last_statement = last_statement;
		m_current_indent_level = 0;
		/// @todo do this somewhere else?
		m_indent_level_stack.push(0);
	};
	function_control_flow_graph_visitor(function_control_flow_graph_visitor &original) : CFGDFSVisitor(original)  {};
	virtual ~function_control_flow_graph_visitor() {};
	
	return_value_t start_subgraph_vertex(T_CFG_VERTEX_DESC u)
	{
		// Either the very first vertex or a converging vertex has been popped.
		// Pop the corresponding indent level.
		std::cout << m_graph[u].m_statement->GetIdentifierCFG() << " = POPPED CONVERGING NODE" << std::endl;
		m_current_subgraph_root = u;
		m_current_indent_level = m_indent_level_stack.top();
		m_indent_level_stack.pop();
		
		return ok; 
	};
	
	return_value_t discover_vertex(T_CFG_VERTEX_DESC u)
	{
		// We found a new vertex.  Let's see if we need to do anything special.

		if((m_current_subgraph_root != u) && (filtered_in_degree(u, m_graph) > 1))
		{
			// This isn't the root of the current subgraph, and is a vertex where two or more flows converge.
			// Make it terminate this path of the DFS, and push it on the
			// converging_node_stack for the next sub-search.
			indent(m_current_indent_level-1);
			std::cout << "}" << std::endl;
			m_indent_level_stack.push(m_current_indent_level);
			return terminate_branch;
		}
		
		// Print the statement corresponding to this vertex.
		indent(m_current_indent_level);
		std::cout << m_graph[u].m_statement->GetIdentifierCFG()
			<< " <"
			<< *m_graph[u].m_statement->GetLocation()
			<< ">"
			<< std::endl;
		
		if(u == m_last_statement)
		{
			std::cout << "INFO: Found last statement of function" << std::endl;
			// We've reached the end of the function, terminate the search.
			return terminate_search;				
		}
		
		// If the target vertex results in a branch of the CFG,
		// indent the subsequent vertex another level.
		Statement *p = m_graph[u].m_statement;
		if((dynamic_cast<If*>(p) != NULL) ||
		 (dynamic_cast<Switch*>(p) != NULL))
		{
			indent(m_current_indent_level);
			std::cout << "{" << std::endl;
			m_current_indent_level++;
		}
		
		return ok;
	}
	
	return_value_t tree_edge(T_CFG_EDGE_DESC ed)
	{
		return_value_t retval=ok;
		T_CFG_VERTEX_DESC v;
		CFGEdgeTypeBase *et;
		CFGEdgeTypeFunctionCall *fc;
		CFGEdgeTypeReturn *ret;
		et = m_graph[ed].m_edge_type;
		
		// Attempt dynamic casts to call/return types to see if we need to handle
		// these specially.
		fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(et);
		ret = dynamic_cast<CFGEdgeTypeReturn*>(et);
		
		// Get the target vertex.
		v = boost::target(ed, m_graph);
#if 0
		// If the edge is a function call or return, we have to:
		// - Indent/Outdent another level.
		// - Push/Pop this call onto/off the call stack.
		// - Shift to a new / back to the old "color context".  We have to do this
		//   because the CFG may pass through a function multiple times, and we have
		//   to have clear color maps for each call.
		if(fc != NULL)
		{
			// This edge is a function call, indent another level.
			m_current_indent_level++;
			//std::cout << "PUSHING CALL: " << fc->m_function_call->GetIdentifier() << std::endl;
			m_call_stack.push_back(fc->m_function_call);
			retval = push_color_context;
		}
		else if(ret != NULL)
		{
			// This edge is a function return.
			// Pop the call off the m_call_stack and outdent a level.
			//std::cout << "POPPING CALL: " << ret->m_function_call->GetIdentifier() << std::endl;
			m_call_stack.pop_back();
			m_current_indent_level--;
			retval = pop_color_context;
		}
#endif
		// Tell the search function if we want to push or pop the color context.
		return retval;
	}
	
	return_value_t examine_edge(T_CFG_EDGE_DESC ed)
	{
		// Filter out any edges that we want to pretend aren't even part of the
		// graph we're looking at.
		CFGEdgeTypeBase *edge_type;
		
		edge_type = m_graph[ed].m_edge_type;
		
		if(dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(edge_type) != NULL)
		{
			// Skip FunctionCallBypasses entirely.
			return terminate_branch;
		}

		CFGEdgeTypeReturn *ret;
		ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);
		if((ret != NULL) && (ret->m_function_call != m_call_stack.back()))
		{
			// This is a return edge, but for a different call than the one
			// that brought us here.  Skip it.
			return terminate_branch;
		}
		
		// Check if this edge is a function call or return.  If it is, tell the
		// DFS to push/pop a new color context.
		T_CFG_VERTEX_DESC v;
		CFGEdgeTypeBase *et;
		CFGEdgeTypeFunctionCall *fc;
		et = m_graph[ed].m_edge_type;
		
		// Attempt dynamic casts to call/return types to see if we need to handle
		// these specially.
		fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(et);
		ret = dynamic_cast<CFGEdgeTypeReturn*>(et);
		
		// Get the target vertex.
		v = boost::target(ed, m_graph);
		
		// If the edge is a function call or return, we have to:
		// - Indent/Outdent another level.
		// - Push/Pop this call onto/off the call stack.
		// - Shift to a new / back to the old "color context".  We have to do this
		//   because the CFG may pass through a function multiple times, and we have
		//   to have clear color maps for each call.
		if(fc != NULL)
		{
			// This edge is a function call, indent another level.
			m_current_indent_level++;
			//std::cout << "PUSHING CALL: " << fc->m_function_call->GetIdentifier() << std::endl;
			m_call_stack.push_back(fc->m_function_call);
			return push_color_context;
		}
		else if(ret != NULL)
		{
			// This edge is a function return.
			// Pop the call off the m_call_stack and outdent a level.
			//std::cout << "POPPING CALL: " << ret->m_function_call->GetIdentifier() << std::endl;
			m_call_stack.pop_back();
			m_current_indent_level--;
			return pop_color_context;
		}
		
		return ok;
	}
	
private:
	
	/// Vertex corresponding to the last statement of the function.
	/// We'll terminate the search when we find this.
	T_CFG_VERTEX_DESC m_last_statement;
	
	T_CFG_VERTEX_DESC m_current_subgraph_root;
	
	/// The indent level.  This corresponds to the number of branching statements
	/// with unterminated branches between the starting node and the current node.
	long m_current_indent_level;
	
	/// The FunctionCall call stack.
	std::vector<FunctionCall*> m_call_stack;
	
	std::stack< long > m_indent_level_stack;
	
	std::stack< long > m_statement_indent_level_stack;
};

template <class IncidenceGraph, class ImprovedDFSVisitor, class ColorMapStack>
//BOOST_CONCEPT_REQUIRES((IncidenceGraphConcept<IncidenceGraph>), (void))
//BOOST_CONCEPT_REQUIRES(((IncidenceGraphConcept<IncidenceGraph>)), (void))
void improved_depth_first_visit(IncidenceGraph &graph,
	typename boost::graph_traits<IncidenceGraph>::vertex_descriptor source,
	ImprovedDFSVisitor &visitor,
	ColorMapStack &color_map_stack)
{
	// Some convenience typedefs.
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef typename boost::graph_traits<IncidenceGraph>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<IncidenceGraph>::out_edge_iterator T_OUT_EDGE_ITERATOR;
	typedef std::map< T_VERTEX_DESC,  boost::default_color_type > T_COLOR_MAP;
	
	// The local variables.
	T_VERTEX_DESC u;
	T_OUT_EDGE_ITERATOR ei, eend;
	typename ImprovedDFSVisitor::return_value_t visitor_return_value;

	// The vertex "context" stack.
	std::vector<VertexInfo> dfs_stack;
	
	// Push a new color context onto the color map stack.
	color_map_stack.push_back(new T_COLOR_MAP);
	
	// The converging node stack.
	std::vector< T_VERTEX_DESC > converging_node_stack;
	
	// Start at the first known vertex of this Function's CFG.
	u = source;

	converging_node_stack.push_back(u);
	
	while(!converging_node_stack.empty())
	{
		// Pop the next vertex off the converging node stack.
		u = converging_node_stack.back();
		converging_node_stack.pop_back();
		
		visitor_return_value = visitor.start_subgraph_vertex(u);
		
		// Mark this vertex as explored.
		(*color_map_stack.back())[u] = T_COLOR::gray();

		// Let the visitor look at the vertex via discover_vertex().
		visitor_return_value = visitor.discover_vertex(u);

		// Get iterators to the out edges of vertex u.
		boost::tie(ei, eend) = boost::out_edges(u, graph);

		// Push the first vertex onto the stack and we're ready to go.
		/// @todo decide if the first vertex here terminates the search.
		if(visitor_return_value == ImprovedDFSVisitor::terminate_branch)
		{
			// The visitor decided in discover_vertex() that this node terminates
			// this branch.  Push an empty edge range onto the DFS stack, so we won't follow
			// it.
			/// @todo Is there a reason we can't just do a "continue" and avoid the push_back()?
			ei = eend;
		}
		else if(visitor_return_value == ImprovedDFSVisitor::terminate_search)
		{
			/// @todo Stop the search.
			ei = eend;
		}

		vertex_info.Set(u, ei, eend);
		dfs_stack.push_back(vertex_info);

		while(!dfs_stack.empty())
		{
			// Pop the context off the top of the stack.
			u = dfs_stack.back().m_v;
			ei = dfs_stack.back().m_ei;
			eend = dfs_stack.back().m_eend;
			dfs_stack.pop_back();

			// Now iterate over the out_edges.
			while(ei != eend)
			{
				T_VERTEX_DESC v;
				boost::default_color_type v_color;

				// Let the visitor examine the edge *ei.
				visitor_return_value = visitor.examine_edge(*ei);
				switch(visitor_return_value)
				{
					case ImprovedDFSVisitor::terminate_branch:
					{
						// The visitor wants us to skip this edge.
						// Skip it.  This will make it appear as if it was never in 
						// the graph at all.
						++ei;
						continue;
						break;
					}
					case ImprovedDFSVisitor::terminate_search:
					{
						/// @todo Stop searching.
						break;
					}
					case ImprovedDFSVisitor::push_color_context:
					{
						color_map_stack.push_back(new T_COLOR_MAP);
						break;
					}
					case ImprovedDFSVisitor::pop_color_context:
					{
						delete color_map_stack.back();
						color_map_stack.pop_back();
						break;
					}
					default:
						break;
				}

				// Get the target vertex of the current edge.
				v = boost::target(*ei, graph);
				
				//
				// Get the target vertex's color.
				//
				typename T_COLOR_MAP::iterator cmi;
				cmi = (*color_map_stack.back()).find(v);
				if(cmi == (*color_map_stack.back()).end())
				{
					// Wasn't in the map, must not have seen it before.
					// Pretend it was and add it with the default color (white).
					(*color_map_stack.back())[v] = T_COLOR::white();
					v_color = T_COLOR::white();
				}
				else
				{
					// Vertex was in the map, get the color.
					v_color = cmi->second;
				}

				//
				// Now decide what to do based on the color of the vertex.
				//
				if(v_color == T_COLOR::white())
				{
					// This is a tree edge, i.e. it is one of the edges
					// that is a member of the search tree.

					// Visit the edge.
					visitor_return_value = visitor.tree_edge(*ei);
					/*switch(visitor_return_value)
					{
						case ImprovedDFSVisitor::push_color_context:
							color_map_stack.push_back(new T_COLOR_MAP);
							break;
						case ImprovedDFSVisitor::pop_color_context:
							delete color_map_stack.back();
							color_map_stack.pop_back();
							break;
						/// @todo Handle other cases.
						default:
							break;
					}*/
										
					// Go to the next out-edge of this vertex.
					++ei;
					vertex_info.Set(u, ei, eend);
					dfs_stack.push_back(vertex_info);
					u = v;
					(*color_map_stack.back())[u] = T_COLOR::gray();
					

					// Visit the edge's target vertex with discover_vertex(u).
					visitor_return_value = visitor.discover_vertex(u);
		
					// Get the out-edges of this vertex.
					boost::tie(ei, eend) = boost::out_edges(u, graph);
					
					if(visitor_return_value == ImprovedDFSVisitor::terminate_branch)
					{
						// Visitor wants us to stop searching past this vertex.
						// Set the iterators the same so that on the next loop,
						// we'll break out of the while().
						/// @todo Can't we just break?
						ei = eend;
						converging_node_stack.push_back(u);
					}
					else if(visitor_return_value == ImprovedDFSVisitor::terminate_search)
					{
						// Stop searching by not pushing this vertex onto the converging_node_stack.
						/// @todo Is this really enough?
						ei = eend;
					}
				}
				else if(v_color == T_COLOR::gray())
				{
					// This is a back edge, i.e. an edge to a vertex that we've 
					// already visited.  Visit it, but don't follow it.
					visitor_return_value = visitor.back_edge(*ei);
					/// @todo Interpret and handle return value.
					++ei;
				}
				else
				{
					// A forward or cross edge.  Visit it, but don't follow it.
					visitor_return_value = visitor.forward_or_cross_edge(*ei);
					/// @todo Interpret and handle return value.
					++ei;
				}
			}

			// Visited, so mark the vertex black.
			(*color_map_stack.back())[u] = T_COLOR::black();

			// Finish the vertex.
			visitor.finish_vertex(u);
		}
	}	
}

void Function::PrintControlFlowGraph()
{
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef std::map< T_CFG_VERTEX_DESC,  boost::default_color_type > T_COLOR_MAP;
	std::vector< T_COLOR_MAP* > color_map_stack;
	
	function_control_flow_graph_visitor cfg_visitor(*m_cfg, m_last_statement);
	improved_depth_first_visit(*m_cfg, m_first_statement, cfg_visitor, color_map_stack);
	
#if 0
	long current_indent_level = 0;
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef std::map< T_CFG_VERTEX_DESC,  boost::default_color_type > T_COLOR_MAP;
	T_CFG_VERTEX_DESC u;
	boost::graph_traits< T_CFG >::out_edge_iterator ei, eend;

	// The vertex "context" stack.
	std::vector<VertexInfo> dfs_stack;
	
	// The call stack.
	std::vector<FunctionCall*> call_stack;
	std::vector< T_COLOR_MAP* > color_map_stack;
	color_map_stack.push_back(new T_COLOR_MAP);
	
	// The converging node stack.
	typedef boost::tuple < T_CFG_VERTEX_DESC, long > T_CONVERGENCE_NODE;
	std::vector< T_CONVERGENCE_NODE > converging_node_stack;
	
	// Start at the first known vertex of this Function's CFG.
	u = m_first_statement;

	converging_node_stack.push_back(boost::tie(u, current_indent_level));
	
	while(!converging_node_stack.empty())
	{
		// Pop the next vertex off the converging node stack.
		u = converging_node_stack.back().get<0>();
		current_indent_level = converging_node_stack.back().get<1>();
		
		converging_node_stack.pop_back();
		indent(current_indent_level);
		std::cout << "POPPED CONVERGING EDGE: " << (*m_cfg)[u].m_statement->GetIdentifierCFG() << std::endl;
		
		// Mark this vertex as explored.
		(*color_map_stack.back())[u] = T_COLOR::gray();

		// Boost puts a discover_vertex() here.
		{
			
		}

		// Get iterators to the out edges.
		boost::tie(ei, eend) = boost::out_edges(u, *m_cfg);

		// Push the first vertex onto the stack and we're ready to go.
		vertex_info.Set(u, ei, eend, current_indent_level);
		dfs_stack.push_back(vertex_info);

		while(!dfs_stack.empty())
		{
			// Pop the context off the top of the stack.
			u = dfs_stack.back().m_v;
			ei = dfs_stack.back().m_ei;
			eend = dfs_stack.back().m_eend;
			current_indent_level = dfs_stack.back().m_indent_level;
			dfs_stack.pop_back();

			//std::cout << "INFO: Out edge types:" << std::endl;
			//PrintOutEdgeTypes(v, *m_cfg);

			// Now iterate over the out_edges.
			while(ei != eend)
			{
				T_CFG_VERTEX_DESC v;
				boost::default_color_type v_color;

				// Filter out any edges that we want to pretend aren't even part of the
				// graph we're looking at.
				if(dynamic_cast<CFGEdgeTypeFunctionCallBypass*>((*m_cfg)[*ei].m_edge_type) != NULL)
				{
					// Skip FunctionCallBypasses entirely.
					++ei;
					continue;
				}

				CFGEdgeTypeReturn *ret;
				ret = dynamic_cast<CFGEdgeTypeReturn*>((*m_cfg)[*ei].m_edge_type);
				if((ret != NULL) && (ret->m_function_call != call_stack.back()))
				{
					// This is a return edge, but for a different call than the one
					// that brought us here.  Skip it.
					++ei;
					continue;
				}

				// Get the target vertex of the current edge.
				v = boost::target(*ei, *m_cfg);

				// Boost does a examine_edge of *ei here.
				// tbd???

				// Get the target vertex's color.
				T_COLOR_MAP::iterator cmi;
				cmi = (*color_map_stack.back()).find(v);
				if(cmi == (*color_map_stack.back()).end())
				{
					// Wasn't in the map already, add it with the default color (white).
					(*color_map_stack.back())[v] = T_COLOR::white();
					v_color = T_COLOR::white();
				}
				else
				{
					// Vertex was in the map, get the color.
					v_color = cmi->second;
				}

				if(v_color == T_COLOR::white())
				{
					// This is a tree edge.

					// Boost does a tree_edge() visit of *ei here.
					{
						CFGEdgeTypeBase *et;
						CFGEdgeTypeFunctionCall *fc;
						et = (*m_cfg)[*ei].m_edge_type;
						fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(et);
						if(fc != NULL)
						{
							// This edge is a function call, indent another level.
							current_indent_level++;
							//std::cout << "PUSHING CALL: " << fc->m_function_call->GetIdentifier() << std::endl;
							call_stack.push_back(fc->m_function_call);
							color_map_stack.push_back(new T_COLOR_MAP);
						}
						else if(ret != NULL)
						{
							// This edge is a function return.
							// Pop the call off the call_stack and outdent a level.
							delete color_map_stack.back();
							color_map_stack.pop_back();
							call_stack.pop_back();
							current_indent_level--;
						}

						// Indent and print the target statement.

						if(
							(dynamic_cast<FunctionCall*>((*m_cfg)[v].m_statement) != NULL) ||
							 (dynamic_cast<If*>((*m_cfg)[v].m_statement) != NULL) ||
							 (dynamic_cast<Switch*>((*m_cfg)[v].m_statement) != NULL))
						{
						indent(current_indent_level);
						std::cout << (*m_cfg)[v].m_statement->GetIdentifierCFG()
							<< " <"
							<< *(*m_cfg)[v].m_statement->GetLocation()
							<< "> FROM:" << (*m_cfg)[boost::source(*ei, *m_cfg)].m_statement->GetIdentifierCFG()
							<< " OutDegree=" << boost::out_degree(v, *m_cfg)
							<< std::endl;
						}
						// If the target node results in a branch of the CFG,
						// indent the subsequent nodes another level.
						Statement *p = (*m_cfg)[v].m_statement;
						if((dynamic_cast<If*>(p) != NULL) ||
						 (dynamic_cast<Switch*>(p) != NULL))
						{
							indent(current_indent_level);
							std::cout << "{" << std::endl;
							current_indent_level++;
						}
					}

					// Go to the next out-edge.
					++ei;
					vertex_info.Set(u, ei, eend, current_indent_level);
					dfs_stack.push_back(vertex_info);

					u = v;
					(*color_map_stack.back())[u] = T_COLOR::gray();

					// Boost does a discover_vertex(u) here.

					boost::tie(ei, eend) = boost::out_edges(u, *m_cfg);
					if(u == m_last_statement)
					{
						std::cout << "INFO: Found last statement of function" << std::endl;
						ei = eend;				
					}
					else if(filtered_in_degree(u, *m_cfg) > 1)
					{
						// This is a vertex where two or more flows converge.
						// Make it terminate this path of the DFS, and push it on the
						// stack for the next one.
						ei = eend;
						indent(current_indent_level);
						std::cout << "}" << std::endl;
						converging_node_stack.push_back(boost::tie(u, current_indent_level));
					}
				}
				else if(v_color == T_COLOR::gray())
				{
					// A back edge, skip it.
					//std::cout << "INFO: Found back edge" << std::endl;
					++ei;
				}
				else
				{
					// A forward or cross edge, skip it.
					//std::cout << "INFO: Found fwd/cross statement" << std::endl;
					++ei;
				}
			}

			// Visited, so mark the vertex black.
			(*color_map_stack.back())[u] = T_COLOR::black();

			// Finish the vertex.
			// tbd???
		}
	}
#endif
}


void Function::PrintDotCFG(const std::string &the_dot, const boost::filesystem::path& output_dir)
{
	std::string dot_filename;

	T_VERTEX_PROPERTY_MAP vpm = boost::get(&CFGVertexProperties::m_containing_function, *m_cfg);

	vertex_filter_predicate the_filter(vpm, this);
	boost::filtered_graph<T_CFG, boost::keep_all, vertex_filter_predicate>
		graph_of_this_function(*m_cfg, boost::keep_all(), the_filter);

	dot_filename = output_dir.string()+m_function_id+".dot";
	
	std::cerr << "Creating " << dot_filename << std::endl;
	
	std::ofstream outfile(dot_filename.c_str());
	
	boost::write_graphviz(outfile, graph_of_this_function,
						 cfg_vertex_property_writer(*m_cfg),
						 cfg_edge_property_writer(*m_cfg),
						 graph_property_writer());
	
	outfile.close();
	
	std::cerr << "Compiling " << dot_filename << std::endl;
	::system((the_dot + " -O -Tpng "+dot_filename).c_str());
}

bool Function::CreateControlFlowGraph(T_CFG & cfg)
{
	// We create the Control Flow Graph in two stages:
	// - First we go through each basic block and add all Statements to m_cfg,
	//   adding an edge linking each to its predecessor as we go.  We exclude in-edges
	//   and out-edges for the first and last Statement in each block, resp.
	// - Second, we link the last Statement of each block to its Successors.
	
	m_cfg = &cfg;
	
	std::map< T_BLOCK_GRAPH::vertex_descriptor, T_CFG_VERTEX_DESC > first_statement_of_block;
	std::vector< T_CFG_VERTEX_DESC > last_statement_of_block;
	bool ok;
				
	// Do the first step.
	T_BLOCK_GRAPH::vertex_iterator vit, vend;
	for(boost::tie(vit,vend) = boost::vertices(m_block_graph); vit != vend; vit++)
	{
		Block::T_STATEMENT_LIST_ITERATOR sit;
		T_CFG_VERTEX_DESC last_vid;
		bool is_first = true;
		
		// Iterate over all Statements in this Block.
		for(sit = m_block_graph[*vit].m_block->begin(); sit != m_block_graph[*vit].m_block->end(); sit++)
		{
			// Add this Statement to the Control Flow Graph.
			T_CFG_VERTEX_DESC vid;
			vid = boost::add_vertex(*m_cfg);
			(*m_cfg)[vid].m_statement = *sit;
			(*m_cfg)[vid].m_containing_function = this;
			
			if(!is_first)
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
				
				if(m_block_graph[*vit].m_block->IsENTRY())
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
		
		if(m_block_graph[*vit].m_block->IsEXIT())
		{
			// This is the last statement of the EXIT block.
			m_last_statement = last_vid;
		}
	}
	
	// Do the second step.
	std::vector< T_CFG_VERTEX_DESC >::iterator last_statement_it;
	std::map< T_BLOCK_GRAPH::vertex_descriptor, T_CFG_VERTEX_DESC >::iterator first_statement_it;
	last_statement_it = last_statement_of_block.begin();
	for(boost::tie(vit,vend) = boost::vertices(m_block_graph); vit != vend; vit++)
	{
		T_BLOCK_GRAPH::out_edge_iterator eit, eend;
		for(boost::tie(eit, eend) = boost::out_edges(*vit, m_block_graph); eit != eend; eit++)
		{
			// Add an edge from the last statement of Block vit to the first statement
			// of the Block pointed to by eit.
			T_CFG_VERTEX_DESC target_vertex_descr = boost::target(*eit, m_block_graph);

			first_statement_it = first_statement_of_block.find(target_vertex_descr);
			
			if(first_statement_it == first_statement_of_block.end())
			{
				std::cout << "ERROR: No first block statement found." << std::endl;
			}
			else
			{
				// Add the edge.
				T_CFG_EDGE_DESC new_edge_desc;
				boost::tie(new_edge_desc, ok) = boost::add_edge(*last_statement_it, first_statement_it->second, *m_cfg);
				
				/// @todo Stubbing in fallthrough type for now, change this to add the real type.
				(*m_cfg)[new_edge_desc].m_edge_type = new CFGEdgeTypeFallthrough();
			}
		}
		last_statement_it++;
	}
	
	// Third step.  CFG is created.  Look for back edges and set their edge types appropriately.
	// Property map for getting at the edge types in the CFG.
	//T_EDGE_TYPE_PROPERTY_MAP edge_type_property_map = boost::get(&CFGEdgeProperties::m_edge_type, *m_cfg);
	T_VERTEX_PROPERTY_MAP vpm = boost::get(&CFGVertexProperties::m_containing_function, *m_cfg);
	std::vector<T_CFG_EDGE_DESC> back_edges;
	
	// Define a filtered view of this function's CFG, which hides the back-edges
	// so that we can produce a topological sort.
	dfs_back_edge_finder_visitor back_edge_finder(back_edges);

	vertex_filter_predicate the_vertex_filter(vpm, this);
	boost::filtered_graph<T_CFG, boost::keep_all, vertex_filter_predicate>
		graph_of_this_function(*m_cfg, boost::keep_all(), the_vertex_filter);
	
	// Find all the back edges.
	boost::depth_first_search(*m_cfg, boost::visitor(back_edge_finder));
	
	// Mark them as back edges.
	BOOST_FOREACH(T_CFG_EDGE_DESC e, back_edges)
	{
		// Change this edge type to a back edge.
		(*m_cfg)[e].m_edge_type->MarkAsBackEdge(true);
	}
	
	return true;
}
