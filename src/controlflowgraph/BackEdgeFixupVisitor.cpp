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

#include <vector>
#include <boost/unordered_set.hpp>

#include "../debug_utils/debug_utils.hpp"
#include "BackEdgeFixupVisitor.h"

void BackEdgeFixupVisitor::tree_edge(T_CFG_EDGE_DESC e, const T_CFG &g)
{
	// An edge just became part of the DFS search tree.  Capture the predecessor info this provides.
	m_predecessor_map[boost::target(e, g)] = e;
}

void BackEdgeFixupVisitor::back_edge(T_CFG_EDGE_DESC e, const T_CFG &g)
{
	// Found a back edge.  Search for a suitable target for an Impossible edge which will
	// be inserted later as a "proxy" for the back edge.

	/// @todo
	BackEdgeFixupInfo fui;

	dlog_cfg << "FOUND BACK EDGE: " << e << std::endl;

	fui.m_back_edge = e;
	//fui.m_impossible_target_vertex = FindForwardTargetForBackEdge(g, e);
	fui.m_impossible_target_vertex = boost::target(e, g);

	m_back_edges.push_back(fui);
}

T_CFG_EDGE_DESC FindDifferentOutEdge(T_CFG_EDGE_DESC e, const T_CFG &cfg)
{
	T_CFG_OUT_EDGE_ITERATOR ei, eend;

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

T_CFG_VERTEX_DESC BackEdgeFixupVisitor::FindForwardTargetForBackEdge(const T_CFG &cfg, T_CFG_EDGE_DESC e)
{
	// The source vertex of the back edge.
	T_CFG_VERTEX_DESC u;
	// The target vertex of the back edge.
	T_CFG_VERTEX_DESC v;

	T_CFG_VERTEX_DESC w;
	// The forward target we'll try to find.
	T_CFG_VERTEX_DESC retval = boost::graph_traits<T_CFG>::null_vertex();

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
			retval = boost::target(FindDifferentOutEdge(e, cfg), cfg);
			break;
		}

		// On the next iteration, start from the vertex we just found.
		u = w;

		// Otherwise, continue until we find a decision statement which might lead us out of here.
	} while(w != v);

	return retval;
}
