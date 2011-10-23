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

#include "ControlFlowGraphTraversalDFS.h"

#include <stack>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/tuple/tuple.hpp>

#include "SparsePropertyMap.h"
#include "ControlFlowGraph.h"
#include "visitors/ImprovedDFSVisitorBase.h"

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
	// TODO Auto-generated destructor stub
}

void ControlFlowGraphTraversalDFS::Traverse(typename boost::graph_traits<T_CFG>::vertex_descriptor source,
		ControlFlowGraphVisitorBase *visitor)
{
	// Some convenience typedefs.
	typedef VertexInfo<T_CFG> T_VERTEX_INFO;
	typedef typename boost::graph_traits<T_CFG>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<T_CFG>::out_edge_iterator T_OUT_EDGE_ITERATOR;
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	//typedef std::map< T_VERTEX_DESC, boost::default_color_type > T_COLOR_MAP;
	typedef SparsePropertyMap< T_VERTEX_DESC, boost::default_color_type, boost::white_color > T_COLOR_MAP;
	typedef std::stack< T_COLOR_MAP* > T_COLOR_MAP_STACK;

	// The local variables.
	T_VERTEX_INFO vertex_info;
	T_VERTEX_DESC u;
	T_OUT_EDGE_ITERATOR ei, eend;
	T_COLOR_MAP_STACK color_map_stack;
	vertex_return_value_t visitor_vertex_return_value;
	edge_return_value_t visitor_edge_return_value;

	// The vertex "context" stack.
	std::stack<T_VERTEX_INFO> dfs_stack;

	// Push a new color context onto the color map stack.
	color_map_stack.push(new T_COLOR_MAP);

	// Start at the source vertex.
	u = source;

	// Mark this vertex as having been visited, but that there are still vertices reachable from it.
	color_map_stack.top()->set(u, T_COLOR::gray());

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
					/// @todo Stop searching.
					break;
				}
				default:
					break;
			}

			// Get the target vertex of the current edge.
			v = m_control_flow_graph.Target(*ei);

			// Get the target vertex's color.
			v_color = color_map_stack.top()->get(v);

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

				// Go to the next vertex.
				u = v;

				// Mark the next vertex as touched.
				color_map_stack.top()->set(u, T_COLOR::gray());

				// Visit the next vertex with discover_vertex(u).
				visitor_vertex_return_value = visitor->discover_vertex(u);

				// Get the out-edges of this vertex.
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
					// Stop searching by not pushing this vertex onto the converging_node_stack.
					/// @todo Is this really enough?
					ei = eend;
				}
			}
			else if(v_color == T_COLOR::gray())
			{
				// This vertex has been visited, but we still have vertices reachable from it
				// that haven't been visited.

				// This is a back edge, i.e. an edge to a vertex that we've
				// already visited.  Visit it, but don't follow it.
				visitor_edge_return_value = visitor->back_edge(*ei);
				std::cout << "BACKEDGE" << std::endl;
				/// @todo Interpret and handle return value.
				++ei;
			}
			else
			{
				// This vertex has been visited and so have all vertices reachable from it.

				// A forward or cross edge.  Visit it, but don't follow it.
				visitor_edge_return_value = visitor->forward_or_cross_edge(*ei);
				std::cout << "FWDCROSS" << std::endl;
				/// @todo Interpret and handle return value.
				++ei;
			}
		}

		// Visited, so mark the vertex black.
		color_map_stack.top()->set(u, T_COLOR::black());

		// Finish the vertex.
		visitor->finish_vertex(u);
	}
}

void ControlFlowGraphTraversalDFS::DoCallStackPushIfNecessary(typename boost::graph_traits<T_CFG>::vertex_descriptor u)
{
	StatementBase *p = m_control_flow_graph.GetT_CFG()[u].m_statement;

	if (p->IsType<FunctionCallResolved>())
	{
		// This is a function call which has been resolved (i.e. has a link to the
		// actual Function that's being called).  Track the call context, and
		// check if we're going recursive.
		FunctionCallResolved *fcr;

		fcr = dynamic_cast<FunctionCallResolved*>(p);

		// Assume we're not.
		//m_last_discovered_vertex_is_recursive = false;

		if(AreWeRecursing(fcr->m_target_function))
		{
			// We're recursing, we need to treat this vertex as if it were a FunctionCallUnresolved.
			std::cout << "RECURSION DETECTED: Function \"" << fcr->m_target_function->GetIdentifier() << "\"" << std::endl;
			//m_last_discovered_vertex_is_recursive = true;
		}
		else
		{
			// We're not recursing, push a normal stack frame and do the call.
			std::cout << "PUSH" << std::endl;
			PushCallStack(new CallStackFrameBase(fcr));
		}
	}
}

void ControlFlowGraphTraversalDFS::DoCallStackPopIfNecessary(typename boost::graph_traits<T_CFG>::vertex_descriptor u)
{
	StatementBase *p = m_control_flow_graph.GetT_CFG()[u].m_statement;

	if(p->IsType<Exit>())
	{
		// We're leaving the function we were in, pop the call stack entry it pushed.
		std::cout << "POP" << std::endl;
		PopCallStack();
	}
}

bool ControlFlowGraphTraversalDFS::SkipEdge(typename boost::graph_traits<T_CFG>::edge_descriptor e)
{
	if(e.m_source == e.m_target)
	{
		// Skip self-edges.
		return true;
	}

	if(m_control_flow_graph.GetT_CFG()[e].m_edge_type->IsBackEdge())
	{
		return true;
	}

	if(ret->m_function_call != TopCallStack())
	{
		// This edge is a return, but not the one corresponding to the FunctionCall
		// that brought us here.  Or, the call stack is empty, indicating that we got here
		// by means other than tracing the control-flow graph (e.g. we started tracing the
		// graph at an internal vertex).
		// Skip it.
		/// @todo An empty call stack here could also be an error in the program.  We should maybe
		/// add a fake "call" when starting a cfg trace from an internal vertex.
		return edge_return_value_t::terminate_branch;
	}
}
