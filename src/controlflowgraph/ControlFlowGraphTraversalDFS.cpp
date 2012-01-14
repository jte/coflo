/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include "ControlFlowGraphTraversalDFS.h"

#include <stack>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/tuple/tuple.hpp>

#include "SparsePropertyMap.h"
#include "ControlFlowGraph.h"
#include "visitors/ImprovedDFSVisitorBase.h"
#include "edges/edge_types.h"

#include "../Function.h"

/**
 * Small struct for maintaining DFS state on the DFS stack.
 *
 * @param v
 * @param ei
 * @param eend
 */
template <typename IncidenceGraph>
struct VertexInfo
{
	typedef typename boost::graph_traits<IncidenceGraph>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<IncidenceGraph>::out_edge_iterator T_OUT_EDGE_ITERATOR;

	/**
	 * Set the VertexInfo's members to the given values.
     */
	void Set(T_VERTEX_DESC v,
		T_OUT_EDGE_ITERATOR ei,
		T_OUT_EDGE_ITERATOR eend
		)
	{
		m_v = v;
		m_ei = ei;
		m_eend = eend;
	};

	T_VERTEX_DESC m_v;
	T_OUT_EDGE_ITERATOR m_ei;
	T_OUT_EDGE_ITERATOR m_eend;
};


ControlFlowGraphTraversalDFS::ControlFlowGraphTraversalDFS(ControlFlowGraph &control_flow_graph) : ControlFlowGraphTraversalBase(control_flow_graph)
{

}

ControlFlowGraphTraversalDFS::~ControlFlowGraphTraversalDFS()
{

}

void ControlFlowGraphTraversalDFS::Traverse(boost::graph_traits<T_CFG>::vertex_descriptor source,
		ControlFlowGraphVisitorBase *visitor)
{
	// Some convenience typedefs.
	typedef VertexInfo<T_CFG> T_VERTEX_INFO;
	typedef boost::graph_traits<T_CFG>::vertex_descriptor T_VERTEX_DESC;
	typedef boost::graph_traits<T_CFG>::out_edge_iterator T_OUT_EDGE_ITERATOR;
	typedef boost::color_traits<boost::default_color_type> T_COLOR;

	// The local variables.
	T_VERTEX_INFO vertex_info;
	T_VERTEX_DESC u;
	T_OUT_EDGE_ITERATOR ei, eend;
	vertex_return_value_t visitor_vertex_return_value;
	edge_return_value_t visitor_edge_return_value;

	// The depth-first search vertex "context" stack.
	// This stack is used exclusively for the essentially ordinary stack-based depth first search.
	std::stack<T_VERTEX_INFO> dfs_stack;

	// Push a dummy "function call" and the root color context onto the color map stack.
	// This stack is solely for managing function calls we encounter while traversing the control flow graph.
	// It primarily maintains a separate color map for each function call, so we don't have to duplicate each Function's
	// individual CFG for each call; this mechanism will make it appear to the search that we did.
	PushCallStack(new CallStackFrameBase(NULL));

	// Start at the source vertex.
	u = source;

	// Mark this vertex as having been visited, but that there are still vertices reachable from it.
	TopCallStack()->GetColorMap()->set(u, T_COLOR::gray());

	// Let the visitor look at the vertex via discover_vertex().
	visitor_vertex_return_value = visitor->discover_vertex(u);

	// Get iterators to the out edges of vertex u.
	boost::tie(ei, eend) = boost::out_edges(u, m_control_flow_graph.GetT_CFG());

	// Push the first vertex onto the stack and we're ready to go.
	if(visitor_vertex_return_value == vertex_return_value_t::terminate_branch)
	{
		// The visitor decided in discover_vertex() that this node terminates
		// this branch.  Push an empty edge range onto the DFS stack, so we won't follow
		// it.
		/// @todo Is there a reason we can't just do a "continue" and avoid the push_back()?
		ei = eend;
	}
	else if(visitor_vertex_return_value == vertex_return_value_t::terminate_search)
	{
		/// @todo Stop the search.
		ei = eend;
	}

	vertex_info.Set(u, ei, eend);
	dfs_stack.push(vertex_info);

	while(!dfs_stack.empty())
	{
		// Pop the context off the top of the stack.
		u = dfs_stack.top().m_v;
		ei = dfs_stack.top().m_ei;
		eend = dfs_stack.top().m_eend;
		dfs_stack.pop();

		// Now iterate over the out_edges.
		while(ei != eend)
		{
			T_VERTEX_DESC v;
			boost::default_color_type v_color;

			// Check if we want to filter out this edge.
			if(SkipEdge(*ei))
			{
				// Skip this edge.
				++ei;
				continue;
			}

			// Let the visitor examine the edge *ei.
			visitor_edge_return_value = visitor->examine_edge(*ei);
			switch(visitor_edge_return_value.as_enum())
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
					// Stop the traversal.
					/// @todo Is there anything more we need to do here?
					return;
				}
				default:
					break;
			}

			// Get the target vertex of the current edge.
			v = m_control_flow_graph.Target(*ei);

			// Get the target vertex's color.
			v_color = TopCallStack()->GetColorMap()->get(v);

			//
			// Now decide what to do based on the color of the target vertex.
			//
			if(v_color == T_COLOR::white())
			{
				// Target has not yet been visited.

				// This is a tree edge, i.e. it is one of the edges
				// that is a member of the search tree.

				// Visit the edge.
				visitor_edge_return_value = visitor->tree_edge(*ei);
				switch(visitor_edge_return_value.as_enum())
				{
					/// @todo Handle other cases.
					default:
						break;
				}

				// Go to the next out-edge of this vertex.
				++ei;

				// Push this vertex onto the stack.
				vertex_info.Set(u, ei, eend);
				dfs_stack.push(vertex_info);

				// Go to the target vertex.
				u = v;

				// Mark the next vertex as touched.
				TopCallStack()->GetColorMap()->set(u, T_COLOR::gray());

				// Visit the next vertex with discover_vertex(u).
				visitor_vertex_return_value = visitor->discover_vertex(u);


				StatementBase* sbp = m_control_flow_graph.GetStatementPtr(u);
				//// If this is a FunctionCallResolved node, push a new stack frame.
				if(sbp->IsType<FunctionCallResolved>())
				{
					//std::cout << "PUSH-fcr" << std::endl;
					PushCallStack(new CallStackFrameBase(dynamic_cast<FunctionCallResolved*>(sbp)));
				}

				// Get the out-edges of the target vertex.
				boost::tie(ei, eend) = boost::out_edges(u, m_control_flow_graph.GetT_CFG());

				if(visitor_vertex_return_value == vertex_return_value_t::terminate_branch)
				{
					// Visitor wants us to stop searching past this vertex.
					// Set the iterators the same so that on the next loop,
					// we'll break out of the while().
					/// @todo Can't we just break?
					ei = eend;
				}
				else if(visitor_vertex_return_value == vertex_return_value_t::terminate_search)
				{
					// Stop searching entirely.
					/// @todo Is this really enough?
					return;
				}
			}
			else if(v_color == T_COLOR::gray())
			{
				// This vertex has been visited, but we still have vertices reachable from it
				// that haven't been visited.

				// This is a back edge, i.e. an edge to a vertex that we've
				// already visited.  Visit it, but don't follow it.
				visitor_edge_return_value = visitor->back_edge(*ei);
				//std::cout << "BACKEDGE" << std::endl;
				/// @todo Interpret and handle return value.
				++ei;
			}
			else
			{
				// This vertex has been visited and so have all vertices reachable from it.

				// A forward or cross edge.  Visit it, but don't follow it.
				visitor_edge_return_value = visitor->forward_or_cross_edge(*ei);
				//std::cout << "FWDCROSS" << std::endl;
				/// @todo Interpret and handle return value.
				++ei;
			}
		}

		// All successors have been visited, so mark the vertex black.
		TopCallStack()->GetColorMap()->set(u, T_COLOR::black());

		// Finish the vertex.
		visitor->finish_vertex(u);
	}
}

bool ControlFlowGraphTraversalDFS::SkipEdge(boost::graph_traits<T_CFG>::edge_descriptor e)
{
	CFGEdgeTypeBase *edge_type;
	CFGEdgeTypeFunctionCall *fc;
	CFGEdgeTypeReturn *ret;
	CFGEdgeTypeFunctionCallBypass *fcb;

	edge_type = m_control_flow_graph.GetT_CFG()[e].m_edge_type;

	// Attempt dynamic casts to call/return types to see if we need to handle
	// these specially.
	fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(edge_type);
	ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);
	fcb = dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(edge_type);

	if(edge_type->IsBackEdge())
	{
		// Skip back edges.
		//std::cout << "skipping back edge" << std::endl;
		return true;
	}

	if(ret != NULL)
	{
		if(ret->m_function_call != TopCallStack()->GetPushingCall())
		{
			// This edge is a return, but not the one corresponding to the FunctionCall
			// that brought us here.  Or, the call stack is empty, indicating that we got here
			// by means other than tracing the control-flow graph (e.g. we started tracing the
			// graph at an internal vertex).
			// Skip it.
			/// @todo An empty call stack here could also be an error in the program.  We should maybe
			/// add a fake "call" when starting a cfg trace from an internal vertex.
			//std::cout << "skipping return:" << std::endl;
			//std::cout << ret->m_function_call->GetIdentifierCFG() << " [" << e.m_target << "]" << " <" << ret->m_function_call->GetLocation() << ">" << std::endl;
			if(TopCallStack()->GetPushingCall() == NULL)
			{
				//std::cout << "NULL" << std::endl;
			}
			else
			{
				//std::cout << TopCallStack()->GetPushingCall()->GetIdentifierCFG() << " [" << e.m_target << "]" << " <" << TopCallStack()->GetPushingCall()->GetLocation() << ">" << std::endl;
			}
			return true;
		}
		else
		{
			//std::cout << "POP-exit" << std::endl;
			PopCallStack();
			return false;
		}
	}

	// Handle recursion.
	// We deal with recursion by deciding here which path to take out of a FunctionCallResolved vertex.
	// Note that this is currently the only vertex type which can result in recursion.
	if ((fcb != NULL) && true/*(m_last_discovered_vertex_is_recursive == false)*/)
	{
		// If we're not in danger of infinite recursion,
		// skip FunctionCallBypasses entirely.  Otherwise take them.
		//std::cout << "skipping fcb" << std::endl;
		return true;
	}
	else if ((fc != NULL)
			&& /*(m_last_discovered_vertex_is_recursive == true)*/false)
	{
		// If we are in danger of infinite recursion,
		// skip FunctionCalls entirely.  Otherwise take them.
		std::cout << "t3" << std::endl;
		return true;
	}

	return false;
}
