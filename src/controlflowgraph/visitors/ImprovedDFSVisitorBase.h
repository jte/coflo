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

#ifndef IMPROVEDDFSVISITORBASE_H
#define	IMPROVEDDFSVISITORBASE_H

#include "../../safe_enum.h"

/**
 * @class vertex_return_value_t
 * The return value type for the visitor's *_vertex() member functions, indicating to the
 * DFS algorithm whether to continue searching or not.
 * 
 * Definitions:
 *  - ok\n
 *    Continue the graph traversal.
 *  - terminate_branch\n
 *    Stop traversing this branch of the graph.  Do not visit any out-edges of the vertex.
 *  - terminate_search\n
 *    Stop the traversal entirely.
 */
DECLARE_ENUM_CLASS(vertex_return_value_t, ok, terminate_branch, terminate_search)

/**
 * @class edge_return_value_t
 *
 * The return value type for the visitor's *_edge() member functions, indicating to the
 * DFS algorithm whether to continue searching or not.
 * 
 * Definitions:
 *  - ok\n
 *    Continue the graph traversal.
 *  - terminate_branch\n
 *    Stop traversing this branch of the graph.
 *  - terminate_search\n
 *    Stop the traversal entirely.
 *  - push_color_context
 *  - pop_color_context
 */
DECLARE_ENUM_CLASS(edge_return_value_t, ok, terminate_branch, terminate_search,
				   push_color_context, pop_color_context)

/**
 * Class template for a graph traversal visitor with additional functionality beyond
 * that provided by boost::default_dfs_visitor.  The primary differences are:
 * - It stores a reference to the graph being visited, thus eliminating the need to
 *   pass a reference to it into the event handlers.
 * - The event handlers return values allowing them to tell the traversal algorithm whether
 *   to continue, stop exploring the current branch, or stop the traversal entirely.
 */
template <typename Vertex, typename Edge, typename Graph>
class ImprovedDFSVisitorBase
{
public:
	ImprovedDFSVisitorBase(const Graph &g) : m_graph(g)
	{

	};
	ImprovedDFSVisitorBase(const ImprovedDFSVisitorBase &orig) : m_graph(orig.m_graph)
	{

	};
	~ImprovedDFSVisitorBase() {};
	
	vertex_return_value_t initialize_vertex(Vertex u) { return vertex_return_value_t::ok; };
	vertex_return_value_t start_vertex(Vertex u) { return vertex_return_value_t::ok; };
	
	vertex_return_value_t start_subgraph_vertex(Vertex u) { return vertex_return_value_t::ok; };
	
	/**
	 * Called when Vertex u is seen for the first time.  Vertex u will have already been
	 * colored gray at the time this member is invoked.
	 * 
     * @param u
     * @return 
     */
	vertex_return_value_t discover_vertex(Vertex u) { return vertex_return_value_t::ok; };
	
	/**
	 * Invoked on every out-edge of each vertex after that vertex has been discovered.
	 * At the time this member is invoked, the source vertex will have been colored gray, but
	 * the target vertex will not have been colored by this iteration of the graph traversal.
	 * However, the target vertex may have already been colored by a previous iteration,
	 * so the color is not known a priori.
	 * 
     * @param u The edge to examine.
     * @return 
     */
	edge_return_value_t examine_edge(Edge u) { return edge_return_value_t::ok; };
	
	/**
	 * The Edge u has been determined to be part of the DFS tree.  This means that
	 * the target vertex has been discovered to be white.  This member is called
	 * before the target vertex is colored gray.
	 * 
     * @param u
     * @return 
     */
	edge_return_value_t tree_edge(Edge u) { return edge_return_value_t::ok; };
	
	/**
	 * The Edge u has been determined to be part of the DFS tree, and has also been determined
	 * to be a back edge.
	 */
	edge_return_value_t back_edge(Edge u) { return edge_return_value_t::ok; };
	edge_return_value_t forward_or_cross_edge(Edge u) { return edge_return_value_t::ok; };
	
	/**
	 * Called on each Vertex u only after it has been called on all child vertices
	 * of u in its DFS tree.
	 * 
     * @param u
     * @return 
     */
	vertex_return_value_t finish_vertex(Vertex u) { return vertex_return_value_t::ok; };
	
	
protected:
	
	/// The Graph we'll be visiting.
	const Graph &m_graph;
};

#endif	/* IMPROVEDDFSVISITORBASE_H */

