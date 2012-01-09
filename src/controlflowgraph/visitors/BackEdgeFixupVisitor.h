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

#ifndef BACKEDGEFIXUPVISITOR_H
#define BACKEDGEFIXUPVISITOR_H

#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/graph/depth_first_search.hpp>

#include "ControlFlowGraph.h"

#include "../../debug_utils/debug_utils.hpp"

/**
 * Depth-first search visitor which finds the information necessary to fix up back edges in the control flow graph.
 */
template <typename GraphType>
class BackEdgeFixupVisitor : public boost::default_dfs_visitor
{
public:

	typedef typename boost::graph_traits<GraphType>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<GraphType>::edge_descriptor T_EDGE_DESC;
	typedef typename boost::graph_traits<GraphType>::out_edge_iterator T_OUT_EDGE_ITERATOR;

	struct BackEdgeFixupInfo
	{
		/// The back edge that we found.
		T_EDGE_DESC m_back_edge;

		/// The vertex we found to point a new Impossible edge to.
		T_VERTEX_DESC m_impossible_target_vertex;
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
	void tree_edge(T_EDGE_DESC e, const GraphType &g)
	{
		// An edge just became part of the DFS search tree.  Capture the predecessor info this provides.
		m_predecessor_map[boost::target(e, g)] = e;
	};

	void back_edge(T_EDGE_DESC e, const GraphType &g)
	{
		// Found a back edge.

		// Don't skip self edges here.  We want to mark them as back edges as well.

		// Search for a suitable target for an Impossible edge which will
		// be inserted later as a "proxy" for the back edge.

		/// @todo
		BackEdgeFixupInfo fui;

		fui.m_back_edge = e;
		if(boost::source(e,g) == boost::target(e,g))
		{
			dlog_cfg << "FOUND BACK EDGE (SELF): " << e << std::endl;
			fui.m_impossible_target_vertex = boost::graph_traits<GraphType>::null_vertex();
		}
		else
		{
			fui.m_impossible_target_vertex = FindForwardTargetForBackEdge(g, e);
		}
		//fui.m_impossible_target_vertex = boost::target(e, g);

		m_back_edges.push_back(fui);
	}

private:

	/// Reference to an external vector where we'll store the edges we'll mark later.
	std::vector<BackEdgeFixupInfo> &m_back_edges;

	/// Map where we'll store the predecessors we find during the depth-first-search.
	/// We need this info to find a suitable target for the Impossible edges we'll add to the graph
	/// to deal with back edges caused by loops.
	boost::unordered_map<T_VERTEX_DESC, T_EDGE_DESC> m_predecessor_map;

	T_EDGE_DESC FindDifferentOutEdge(T_EDGE_DESC e, const GraphType &cfg)
	{
		T_OUT_EDGE_ITERATOR ei, eend;

		// Get a pair of iterators over the out edges of e's source node.
		boost::tie(ei, eend) = boost::out_edges(boost::source(e, cfg), cfg);

		for(; ei != eend; ++ei)
		{
			if(*ei != e)
			{
				// Pick the first one that's not e.
				dlog_cfg << "FOUND OUT EDGE: " << *ei << std::endl;
				return *ei;
			}
		}

		// If we get here, something is wrong.
		dlog_cfg << "ERROR: Couldn't find different out edge." << std::endl;
		return *ei;
	}


	/**
	 * Search the predecessor list from this vertex to the back edge's target vertex for the first decision
	 * statement we can find.  This is primarily for adding a "proxy" edge to replace the back edge for certain
	 * purposes, such as printing and searching the CFG.
	 *
	 * @todo Make sure the one we find actually is the one which breaks us out of the loop.
	 */
	T_VERTEX_DESC FindForwardTargetForBackEdge(const GraphType &cfg, T_EDGE_DESC e)
	{
		// The source vertex of the back edge.
		T_VERTEX_DESC u;
		// The target vertex of the back edge.
		T_VERTEX_DESC v;

		T_VERTEX_DESC w;
		// The forward target we'll try to find.
		T_VERTEX_DESC retval = boost::graph_traits<GraphType>::null_vertex();

		u = boost::source(e, cfg);
		v = boost::target(e, cfg);


		// Walk back up the path by which the DFS got here until we reach the target of the back edge.
		// We're looking for a way out of the cycle.
		// If we get to the target vertex of the back edge and we haven't found a way out, there's
		// some sort of problem with the CFG.
		do
		{
			// Get the predecessor of this vertex.
			e = m_predecessor_map[u];
			w = boost::source(e, cfg);

			dlog_cfg << "WALKING PREDECESSOR TREE: VERTEX " << w << std::endl;

			if(cfg[w].m_statement->IsDecisionStatement())
			{
				// It's a decision statement, this might be the way out.
				dlog_cfg << "FOUND DECISION PREDECESSOR VERTEX: " << w << std::endl;

				// Find an edge that's not this one out of the decision vertex.
				/// @todo Make this more robust.  As far as I know, this isn't guaranteed to be the right way out, or even *a* way out.
				T_CFG_EDGE_DESC other_edge;
				other_edge = FindDifferentOutEdge(e, cfg);

				retval = boost::target(other_edge, cfg);
				break;
			}

			// On the next iteration, start from the vertex we just found.
			u = w;

			// Otherwise, continue until we find a decision statement which might lead us out of here.
		} while(w != v);

		return retval;
	};

};

#endif /* BACKEDGEFIXUPVISITOR_H */
