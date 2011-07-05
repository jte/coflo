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

template <typename Vertex, typename Edge, typename Graph>
class ImprovedDFSVisitorBase
{
public:
	ImprovedDFSVisitorBase(Graph &g) : m_graph(g) {};
	ImprovedDFSVisitorBase(const ImprovedDFSVisitorBase &orig) : m_graph(orig.m_graph) {};
	virtual ~ImprovedDFSVisitorBase() {};
	
	/**
	 * The return value type for the visitor's member functions, indicating to the
	 * DFS algorithm whether to continue searching or not.
	 * 
	 * Definitions:
	 *	- ok = Continue the graph traversal.
	 *	- terminate_branch = Stop traversing this branch of the tree.
	 *  - terminate_search = Stop the traversal entirely.
	 */
	enum return_value_t {ok, terminate_branch, terminate_search, push_color_context,
		pop_color_context};
	
	return_value_t initialize_vertex(Vertex u) { return ok; };
	return_value_t start_vertex(Vertex u) { return ok; };
	
	return_value_t start_subgraph_vertex(Vertex u) { return ok; };
	
	/**
	 * Called when Vertex u is seen for the first time.
	 * 
     * @param u
     * @return 
     */
	return_value_t discover_vertex(Vertex u) { return ok; };
	
	/**
	 * Invoked on every out-edge of each vertex after that vertex has been discovered.
	 * 
     * @param u
     * @return 
     */
	return_value_t examine_edge(Edge u) { return ok; };
	
	/**
	 * The Edge u has been determined to be part of the DFS tree.
	 * 
     * @param u
     * @return 
     */
	return_value_t tree_edge(Edge u) { return ok; };
	
	return_value_t back_edge(Edge u) { return ok; };
	return_value_t forward_or_cross_edge(Edge u) { return ok; };
	
	/**
	 * Called on each Vertex u only after it has been called on any child vertices
	 * of u in its DFS tree.
	 * 
     * @param u
     * @return 
     */
	return_value_t finish_vertex(Vertex u) { return ok; };
	
	
protected:
	
	/// The Graph we'll be visiting.
	Graph &m_graph;
};

#endif	/* IMPROVEDDFSVISITORBASE_H */

