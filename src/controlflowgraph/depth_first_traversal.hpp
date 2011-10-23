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

#ifndef DEPTH_FIRST_TRAVERSAL_HPP
#define DEPTH_FIRST_TRAVERSAL_HPP

#include <stack>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/tuple/tuple.hpp>

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

/**
 * A depth-first visit algorithm with a number of changes and improvements from the one in the BGL.
 * The differences:
 * - The @a visitor can terminate the search by returning terminate_search from its visiting member functions,
 *   and does not need to throw an exception.
 * - The @a visitor can prune the search tree by returning terminate_branch from its visiting member functions.
 * - Supports the notion of a color map stack, allowing transitions into and out of color contexts.  This is primarily
 *   for the benefit of control flow graph traversals, where multiple function calls to the same function would otherwise
 *   not work correctly.
 *
 * @tparam IncidenceGraph
 * @tparam ImprovedDFSVisitor
 *
 * @param graph  The graph to traverse.
 * @param source  The vertex to start from.
 * @param visitor  The visitor.
 * @param color_map_stack
 */
template <class IncidenceGraph, class ImprovedDFSVisitor>
void improved_depth_first_visit(IncidenceGraph &graph,
	typename boost::graph_traits<IncidenceGraph>::vertex_descriptor source,
	ImprovedDFSVisitor &visitor)
{
	// Some convenience typedefs.
	typedef VertexInfo<IncidenceGraph> T_VERTEX_INFO;
	typedef typename boost::graph_traits<IncidenceGraph>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<IncidenceGraph>::out_edge_iterator T_OUT_EDGE_ITERATOR;
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef std::map< T_VERTEX_DESC, boost::default_color_type > T_COLOR_MAP;
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

	visitor_vertex_return_value = visitor.start_subgraph_vertex(u);

	// Mark this vertex as having been visited, but that there are still vertices reachable from it.
	(*color_map_stack.top())[u] = T_COLOR::gray();

	// Let the visitor look at the vertex via discover_vertex().
	visitor_vertex_return_value = visitor.discover_vertex(u);

	// Get iterators to the out edges of vertex u.
	boost::tie(ei, eend) = boost::out_edges(u, graph);

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

			// Let the visitor examine the edge *ei.
			visitor_edge_return_value = visitor.examine_edge(*ei);
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
				case edge_return_value_t::push_color_context:
				{
					color_map_stack.push(new T_COLOR_MAP);
					break;
				}
				case edge_return_value_t::pop_color_context:
				{
					delete color_map_stack.top();
					color_map_stack.pop();
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
			cmi = (*color_map_stack.top()).find(v);
			if(cmi == (*color_map_stack.top()).end())
			{
				// Wasn't in the map, must not have seen it before.
				// Pretend it was and add it with the default color (white).
				(*color_map_stack.top())[v] = T_COLOR::white();
				v_color = T_COLOR::white();
			}
			else
			{
				// Vertex was in the map, get the color.
				v_color = cmi->second;
			}

			//
			// Now decide what to do based on the color of the target vertex.
			//
			if(v_color == T_COLOR::white())
			{
				// Target has not yet been visited.

				// This is a tree edge, i.e. it is one of the edges
				// that is a member of the search tree.

				// Visit the edge.
				visitor_edge_return_value = visitor.tree_edge(*ei);
				switch(visitor_edge_return_value.as_enum())
				{
					case edge_return_value_t::push_color_context:
						color_map_stack.push(new T_COLOR_MAP);
						break;
					case edge_return_value_t::pop_color_context:
						delete color_map_stack.top();
						color_map_stack.pop();
						break;
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
				(*color_map_stack.top())[u] = T_COLOR::gray();

				// Visit the next vertex with discover_vertex(u).
				visitor_vertex_return_value = visitor.discover_vertex(u);

				// Get the out-edges of this vertex.
				boost::tie(ei, eend) = boost::out_edges(u, graph);

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
				visitor_edge_return_value = visitor.back_edge(*ei);
				std::cout << "BACKEDGE" << std::endl;
				/// @todo Interpret and handle return value.
				++ei;
			}
			else
			{
				// This vertex has been visited and so have all vertices reachable from it.

				// A forward or cross edge.  Visit it, but don't follow it.
				visitor_edge_return_value = visitor.forward_or_cross_edge(*ei);
				std::cout << "FWDCROSS" << std::endl;
				/// @todo Interpret and handle return value.
				++ei;
			}
		}

		// Visited, so mark the vertex black.
		(*color_map_stack.top())[u] = T_COLOR::black();

		// Finish the vertex.
		visitor.finish_vertex(u);
	}
}

#endif /* DEPTH_FIRST_TRAVERSAL_HPP */
