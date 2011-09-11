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

#ifndef BACKEDGEFIXUPVISITOR_H
#define BACKEDGEFIXUPVISITOR_H

#include <boost/graph/depth_first_search.hpp>

#include "ControlFlowGraph.h"

/**
 * Depth-first search visitor which finds the information necessary to fix up back edges in the control flow graph.
 */
class BackEdgeFixupVisitor : public boost::default_dfs_visitor
{
public:

	struct BackEdgeFixupInfo
	{
		/// The back edge that we found.
		T_CFG_EDGE_DESC m_back_edge;

		/// The vertex we found to point a new Impossible edge to.
		T_CFG_VERTEX_DESC m_impossible_target_vertex;
	};

	BackEdgeFixupVisitor(std::vector<BackEdgeFixupInfo> &back_edges) :
		boost::default_dfs_visitor(), m_back_edges(back_edges), m_predecessor_map()
	{
	};
	~BackEdgeFixupVisitor() {};

	/**
	 * Tree-edge visitor to capture which edge of the DFS search tree resulted in
	 * vertex.target being visited.  I.e., the predecessor info.
	 *
	 * @param e The search tree edge.
	 * @param g The graph being traversed.
	 */
	void tree_edge(T_CFG_EDGE_DESC e, const T_CFG &g);

	void back_edge(T_CFG_EDGE_DESC e, const T_CFG &g);

private:

	/// Reference to an external vector where we'll store the edges we'll mark later.
	std::vector<BackEdgeFixupInfo> &m_back_edges;

	/// Map where we'll store the predecessors we find during the depth-first-search.
	/// We need this info to find a suitable target for the Impossible edges we'll add to the graph
	/// to deal with back edges caused by loops.
	boost::unordered_map<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC> m_predecessor_map;

	/**
	 * Search the predecessor list from this vertex to the back edge's target vertex for the first decision
	 * statement we can find.  This is primarily for adding a "proxy" edge to replace the back edge for certain
	 * purposes, such as printing and searching the CFG.
	 *
	 * @todo Make sure the one we find actually is the one which breaks us out of the loop.
	 */
	T_CFG_VERTEX_DESC FindForwardTargetForBackEdge(const T_CFG &cfg, T_CFG_EDGE_DESC e);
};

#endif /* BACKEDGEFIXUPVISITOR_H */
