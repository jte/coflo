/*
 * topological_visit_kahn.h
 *
 *  Created on: Sep 3, 2011
 *      Author: Gary
 */

#ifndef TOPOLOGICAL_VISIT_KAHN_H
#define TOPOLOGICAL_VISIT_KAHN_H

#include <stack>

#include "ImprovedDFSVisitorBase.h"

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
	//visitor.prior_to_push(source);
	no_remaining_in_edges_set.push(source);
	visitor.start_subgraph_vertex(source);

	while (!no_remaining_in_edges_set.empty())
	{
		// We'll count up the number of vertices pushed into the no-remaining-edges set by this vertex.
		long num_vertices_pushed = 0;

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
			// Note that tree edges are visited in a breadth-first order.
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
			//std::cout << "Removing edge " << *ei << std::endl;
			--id;

			if (id == 0)
			{
				// This vertex now has an in-degree of zero, push it into the
				// input set.
				no_remaining_in_edges_set.push(v);
				visitor.prior_to_push(v, *ei);
				in_degree_map.erase(it);
				//std::cout << "Pushed: " << v << std::endl;
				//PrintInEdgeTypes(v, graph);
				num_vertices_pushed++;
			}
			else
			{
				// Store the decremented value back to the map.
				it->second = id;
			}
			// Go to the next out-edge of u.
			++ei;
		}

		// We've visited all the out edges of this vertex.  Tell the visitor that we're done, and how many
		// new vertices we added to the no-in-edges set.
		visitor.vertex_visit_complete(u, num_vertices_pushed);
	}
};

#endif /* TOPOLOGICAL_VISIT_KAHN_H */
