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

#include <utility>

#include <boost/foreach.hpp>
#include <boost/graph/filtered_graph.hpp>

#include "ControlFlowGraph.h"
#include "BackEdgeFixupVisitor.h"
#include "MergeNodeInsertionVisitor.h"
#include "edges/edge_types.h"
#include "../Function.h"

using std::cout;
using std::cerr;
using std::endl;

/// Property map typedef which allows us to get at the function pointer stored at
/// CFGVertexProperties::m_containing_function in the T_CFG.
typedef boost::property_map<T_CFG, Function* CFGVertexProperties::*>::type T_VERTEX_PROPERTY_MAP;

/**
 * Predicate for filtering the CFG for only the vertices of the given function.
 */
struct vertex_filter_predicate
{
	vertex_filter_predicate()
	{
	}
	;
	vertex_filter_predicate(T_VERTEX_PROPERTY_MAP vertex_prop_map,
			Function *parent_function) :
			m_vertex_prop_map(vertex_prop_map), m_parent_function(
					parent_function)
	{
	}
	;
	bool operator()(const T_CFG_VERTEX_DESC& vid) const
	{
		if (m_parent_function == get(m_vertex_prop_map, vid))
		{
			// This vertex belongs to the function we're concerned with.
			return true;
		}
		else
		{
			return false;
		}
	}
	;

	T_VERTEX_PROPERTY_MAP m_vertex_prop_map;
	Function *m_parent_function;
};


static T_CFG_VERTEX_DESC f_dummy_vertex;

ControlFlowGraph::ControlFlowGraph()
{

}

ControlFlowGraph::~ControlFlowGraph()
{

}

void ControlFlowGraph::InitializeControlFlowGraph()
{
	// Create a dummy node.
	f_dummy_vertex = boost::add_vertex(m_cfg);
}

bool ControlFlowGraph::IsDummyVertex(const T_CFG_VERTEX_DESC &v)
{
	return (v == f_dummy_vertex);
}

T_CFG_VERTEX_DESC ControlFlowGraph::GetDummyVertex()
{
	return f_dummy_vertex;
}

void ControlFlowGraph::PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc)
{
	T_CFG_OUT_EDGE_ITERATOR ei, eend;
	
	boost::tie(ei, eend) = boost::out_edges(vdesc, m_cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(m_cfg[*ei].m_edge_type)).name() << std::endl;
	}
}

void ControlFlowGraph::PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc)
{
	T_CFG_IN_EDGE_ITERATOR ei, eend;
	
	boost::tie(ei, eend) = boost::in_edges(vdesc, m_cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(m_cfg[*ei].m_edge_type)).name() << std::endl;
	}
}

void ControlFlowGraph::FixupBackEdges(Function *f)
{
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, m_cfg);
	vertex_filter_predicate the_vertex_filter(vpm, f);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
	// Define a filtered view of only this function's CFG.
	T_FILTERED_GRAPH graph_of_this_function(m_cfg, boost::keep_all(), the_vertex_filter);

	std::vector<BackEdgeFixupVisitor<T_FILTERED_GRAPH>::BackEdgeFixupInfo> back_edges;

	// Define a visitor which will find all the back edges and send back the info
	// we need to fix them up.
	BackEdgeFixupVisitor<T_FILTERED_GRAPH> back_edge_finder(back_edges);

	// Set the back_edge_finder visitor loose on the function's CFG, with its
	// search strategy being a simple depth-first search.
	// Locate all the back edges, and send the fix-up info back to the back_edges
	// std::vector<> above.
	boost::depth_first_search(graph_of_this_function, boost::visitor(back_edge_finder));

	// Mark the edges back edges we found.
	BOOST_FOREACH(BackEdgeFixupVisitor<T_FILTERED_GRAPH>::BackEdgeFixupInfo fixinfo, back_edges)
	{
		T_CFG_EDGE_DESC e = fixinfo.m_back_edge;

		// Change this edge type to a back edge.
		m_cfg[e].m_edge_type->MarkAsBackEdge(true);

		// Skip the rest if this is a self edge.
		if(fixinfo.m_impossible_target_vertex == boost::graph_traits<T_FILTERED_GRAPH>::null_vertex())
		{
			dlog_cfg << "Self edge, no further action: " << e << std::endl;
			continue;
		}

		// If the source node of this back edge now has no non-back-edge out-edges,
		// add a CFGEdgeTypeImpossible edge to it, so topological sorting works correctly.
		T_CFG_VERTEX_DESC src;
		src = boost::source(e, m_cfg);
		if (boost::out_degree(src, m_cfg) == 1)
		{
			T_CFG_EDGE_DESC newedge;
			boost::tie(newedge, boost::tuples::ignore) =
					boost::add_edge(src, fixinfo.m_impossible_target_vertex, m_cfg);
			m_cfg[newedge].m_edge_type = new CFGEdgeTypeImpossible;

			dlog_cfg << "Retargetting back edge " << e << " to " << fixinfo.m_impossible_target_vertex << std::endl;
		}
	}

	dlog_cfg << "Back edge fixup complete." << std::endl;
}


void ControlFlowGraph::InsertMergeNodes(Function *f)
{
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, m_cfg);
	vertex_filter_predicate the_vertex_filter(vpm, f);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
	// Define a filtered view of only this function's CFG.
	T_FILTERED_GRAPH graph_of_this_function(m_cfg, boost::keep_all(), the_vertex_filter);

	std::vector<MergeNodeInsertionVisitor<T_FILTERED_GRAPH>::MergeInsertionInfo> returned_merge_info;

	MergeNodeInsertionVisitor<T_FILTERED_GRAPH> mni_visitor(&returned_merge_info);

	boost::depth_first_search(graph_of_this_function, boost::visitor(mni_visitor));

	cout << "Returned Merge info:" << endl;
	BOOST_FOREACH(MergeNodeInsertionVisitor<T_FILTERED_GRAPH>::MergeInsertionInfo mi, returned_merge_info)
	{
		cout << "MI:" << endl;
		BOOST_FOREACH(T_CFG_EDGE_DESC e, mi.m_terminal_edges)
		{
				cout << e << endl;
		}
	}

	// Now modify the tree.
	T_CFG_EDGE_DESC last_merge_vertex_out_edge;
	BOOST_FOREACH(MergeNodeInsertionVisitor<T_FILTERED_GRAPH>::MergeInsertionInfo mii, returned_merge_info)
	{
		T_CFG_VERTEX_DESC merge_vertex, last_merge_vertex;
		T_CFG_EDGE_DESC new_edge, last_merge_edge;
		CFGEdgeTypeBase *new_edge_type;

		if(mii.m_terminal_edges.size() < 3)
		{
			// Something went wrong, this should never be less than 3.
			std::cerr << "ERROR: Less than 3 vertices in InsertMergeNodes()." << std::endl;
		}

		std::vector<T_CFG_EDGE_DESC>::iterator eit, eend;
		eit = mii.m_terminal_edges.begin();
		eend = mii.m_terminal_edges.end();
		last_merge_edge = *eit;
		last_merge_vertex = boost::source(last_merge_edge, m_cfg);
		new_edge_type = m_cfg[last_merge_edge].m_edge_type;
		++eit;
		for(; eit+1 != eend; ++eit)
		{
			std::cout << "INFO: Inserting Merge vertex, in edges=" << endl;
			cout << *eit << endl;
			merge_vertex = boost::add_vertex(m_cfg);
			m_cfg[merge_vertex].m_statement = new Merge(Location("[UNKNOWN : 0]"));
			m_cfg[merge_vertex].m_containing_function = f;

			// Add the in-edges.
			boost::tie(new_edge, boost::tuples::ignore) = boost::add_edge(last_merge_vertex, merge_vertex, m_cfg);
			m_cfg[new_edge].m_edge_type = new_edge_type;
			cout << " " << last_merge_vertex << "->" << merge_vertex << std::endl;
			boost::tie(new_edge, boost::tuples::ignore) = boost::add_edge(boost::source(*eit, m_cfg), merge_vertex, m_cfg);
			m_cfg[new_edge].m_edge_type = m_cfg[*eit].m_edge_type;
			cout << " " << boost::source(*eit, m_cfg) << "->" << merge_vertex << std::endl;

			last_merge_vertex = merge_vertex;

			new_edge_type = new CFGEdgeTypeFallthrough();
		}

		// Now add the final out edge of the last merge vertex added above.
		boost::tie(new_edge, boost::tuples::ignore) = boost::add_edge(last_merge_vertex, boost::target(*eit, m_cfg), m_cfg);
		m_cfg[new_edge].m_edge_type = new_edge_type;
		cout << " Target=" << last_merge_vertex << "->" << boost::target(*eit, m_cfg) << endl;
	}

	// Now remove all the old edges which are now invalid.
	BOOST_FOREACH(MergeNodeInsertionVisitor<T_FILTERED_GRAPH>::MergeInsertionInfo mii, returned_merge_info)
	{
		std::vector<T_CFG_EDGE_DESC>::iterator eit, eend;
		eit = mii.m_terminal_edges.begin();
		eend = mii.m_terminal_edges.end();
		for(; eit+1 != eend; ++eit)
		{
			RemoveEdge(*eit);
		}
	}
}


void ControlFlowGraph::RemoveEdge(const T_CFG_EDGE_DESC & e)
{
	boost::remove_edge(e, m_cfg);
}

