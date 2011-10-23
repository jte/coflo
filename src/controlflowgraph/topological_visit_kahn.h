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

#ifndef TOPOLOGICAL_VISIT_KAHN_H
#define TOPOLOGICAL_VISIT_KAHN_H

#include <stack>
#include <boost/throw_exception.hpp>
#include <boost/unordered_map.hpp>
#include <boost/concept/requires.hpp>
#include <boost/concept_check.hpp>

#include "visitors/ImprovedDFSVisitorBase.h"

/**
 * Map of vertex descriptors to the remaining in degree value.
 * This is a lazily-evaluated data structure, in that vertices don't have real entries until the first
 * call to either get() or set().  In the case of get(), a never-before-seen vertex will be initialized to
 * a remaining in degree of its real boost::in_degree() (actually filtered_in_degree() at the moment).
 */
template < typename Graph >
class RemainingInDegreeMap
{
	typedef typename boost::graph_traits<Graph>::degree_size_type T_DEGREE_SIZE_TYPE;
	typedef typename boost::graph_traits<Graph>::vertex_descriptor T_VERTEX_DESC;
	typedef boost::unordered_map<T_VERTEX_DESC, T_DEGREE_SIZE_TYPE> T_UNDERLYING_MAP;

public:
	RemainingInDegreeMap(Graph &graph) : m_graph(graph) {};
	~RemainingInDegreeMap() {};

	void set(const T_VERTEX_DESC& vdesc, long val)
	{
		if(val != 0)
		{
			// Simply set the value.
			m_remaining_in_degree_map[vdesc] = val;
		}
		else
		{
			// If we're setting that value to 0, we no longer need the key.
			// Let's erase it in the interest of conserving space.
			m_remaining_in_degree_map.erase(vdesc);
		}
	};

	long get(const T_VERTEX_DESC& vdesc)
	{
		typename T_UNDERLYING_MAP::iterator it;

		it = m_remaining_in_degree_map.find(vdesc);
		if (it == m_remaining_in_degree_map.end())
		{
			// The target vertex wasn't in the map, which means we haven't
			// encountered it before now.
			// Pretend it was in the map and add it with its original in-degree.
			T_DEGREE_SIZE_TYPE indegree;
			/// @todo We should find a better way to get this "filtered" in degree value.
			indegree = filtered_in_degree(vdesc, m_graph);
			m_remaining_in_degree_map[vdesc] = indegree;

			return indegree;
		}
		else
		{
			// Vertex was in the map.
			return it->second;
		}
	};

private:

	/// Reference to the graph object whose vertices we're looking at.
	Graph &m_graph;

	/// The underlying vertex descriptor to integer map.
	T_UNDERLYING_MAP m_remaining_in_degree_map;
};


/**
 * Kahn's algorithm for topologically sorting (in this case visiting) the nodes of a graph.
 *
 * @tparam BidirectionalGraph The graph type.  Must model the BidirectionalGraphConcept.
 * @tparam ImprovedDFSVisitor The type of the @a visitor object which will be notified of graph traversal events.
 *
 * @param graph The graph to traverse.
 * @param source An edge descriptor whose target vertex is the vertex where the graph traversal should begin.
 * @param visitor The visitor to notify of traversal events.
 */
template<typename BidirectionalGraph, typename ImprovedDFSVisitor>
BOOST_CONCEPT_REQUIRES(
	((boost::BidirectionalGraphConcept< BidirectionalGraph >)),
	(void))
topological_visit_kahn(BidirectionalGraph &graph,
		typename boost::graph_traits<BidirectionalGraph>::edge_descriptor source,
		ImprovedDFSVisitor &visitor)
{
	// Required concepts of the passed graph type.
	// Require a BidirectionGraph because we need efficient access to in edges as well as out edges.
	boost::function_requires< boost::BidirectionalGraphConcept<BidirectionalGraph> >();

	// Some convenience typedefs.
	typedef typename boost::graph_traits<BidirectionalGraph>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<BidirectionalGraph>::edge_descriptor T_EDGE_DESC;
	typedef typename boost::graph_traits<BidirectionalGraph>::out_edge_iterator T_OUT_EDGE_ITERATOR;


	// The local variables.
	T_VERTEX_DESC u, v;
	T_EDGE_DESC e;
	T_OUT_EDGE_ITERATOR ei, eend;
	vertex_return_value_t visitor_vertex_return_value;
	edge_return_value_t visitor_edge_return_value;

	// The set of all edges whose target vertices have no remaining incoming edges.
	std::stack<T_EDGE_DESC> no_remaining_in_edges_set;

	// Map of the remaining in-degrees.
	typedef RemainingInDegreeMap< BidirectionalGraph > T_IN_DEGREE_MAP;
	T_IN_DEGREE_MAP in_degree_map(graph);

	// Start at the source vertex.
	no_remaining_in_edges_set.push(source);
	visitor.start_vertex(source);

	while (!no_remaining_in_edges_set.empty())
	{
		// We'll count up the number of vertices pushed into the no-remaining-edges set by this vertex.
		long num_vertices_pushed = 0;

		// Remove a vertex from the set of in-degree == 0 vertices.
		e = no_remaining_in_edges_set.top();
		no_remaining_in_edges_set.pop();

		u = boost::target(e, graph);

		// Visit vertex u.  Vertices will be visited in the correct (i.e. not reverse-topologically-sorted) order.
		visitor_vertex_return_value = visitor.discover_vertex(u, e);
		if (visitor_vertex_return_value	== vertex_return_value_t::terminate_branch)
		{
			// Visitor wants us to not explore the children of this vertex.
			continue;
		}

		//
		// Decrement the in-degrees of all vertices that are immediate descendants of vertex u.
		//

		// Get iterators to the out edges of vertex u.
		boost::tie(ei, eend) = boost::out_edges(u, graph);
		T_EDGE_DESC first_edge_pushed;

		while (ei != eend)
		{
			// Let the visitor examine the edge *ei.
			visitor_edge_return_value = visitor.examine_edge(*ei);
			switch (visitor_edge_return_value.as_enum())
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

			// Get the target vertex of this edge.
			v = boost::target(*ei, graph);

			// Look up the current in-degree of the target vertex of *ei in the
			// in-degree map.
			long id;
			id = in_degree_map.get(v);

			// We're "removing" this edge, so decrement the effective in-degree of
			// vertex v.
			--id;

			// Store the decremented value back to the map.
			in_degree_map.set(v, id);

			if (id == 0)
			{
				// The edge is now part of
				// the topologically sorted search graph.  Let the visitor know.
				// Note that tree edges are visited in a breadth-first order, and in particular note
				// that the fact that this edge pushed a vertex here does not mean that the pushed vertex
				// will be the next one to be visited.
				visitor_edge_return_value = visitor.tree_edge(*ei);
				/// @todo This doesn't currently return anything we need to handle, but we should handle the return value
				/// appropriately anyway.

				// The target vertex now has an in-degree of zero, push it into the
				// input set.
				first_edge_pushed = *ei;
				no_remaining_in_edges_set.push(*ei);
				//std::cout << "Pushed: " << v << std::endl;
				//PrintInEdgeTypes(v, graph);
				num_vertices_pushed++;
			}

			// Go to the next out-edge of u.
			++ei;
		}

		// We've visited all the out edges of this vertex.  Tell the visitor that we're done, and how many
		// new vertices we added to the no-in-edges set.
		visitor.vertex_visit_complete(u, num_vertices_pushed, first_edge_pushed);
	}
};

#endif /* TOPOLOGICAL_VISIT_KAHN_H */
