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

/// Always include a .cpp file's header first.  This ensures that the header file is
/// idempotent at compile time.  If it isn't, the compile will fail, alerting you to the problem.
#include "ControlFlowGraph.h"

#include <utility>
#include <boost/foreach.hpp>
#include <boost/graph/filtered_graph.hpp>

#include "visitors/BackEdgeFixupVisitor.h"
#include "visitors/MergeNodeInsertionVisitor.h"
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


ControlFlowGraph::ControlFlowGraph()
{

}

ControlFlowGraph::~ControlFlowGraph()
{

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

	// Mark the edges we found as back edges.
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


void ControlFlowGraph::SplitCriticalEdges(Function *f)
{
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, m_cfg);
	vertex_filter_predicate the_vertex_filter(vpm, f);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
	// Define a filtered view of only this function's CFG.
	T_FILTERED_GRAPH graph_of_this_function(m_cfg, boost::keep_all(), the_vertex_filter);

	std::vector<T_CFG_EDGE_DESC> edges_to_remove;

	// Find any critical edges and split them by inserting NOOPs.
	boost::graph_traits<T_FILTERED_GRAPH>::edge_iterator eit, eend;

	boost::tie(eit, eend) = boost::edges(graph_of_this_function);
	for (; eit != eend; ++eit)
	{
		T_CFG_VERTEX_DESC source_vertex_desc, target_vertex_desc;
		long target_id, source_od;

		// Get the vertex descriptors.
		source_vertex_desc = boost::source(*eit, graph_of_this_function);
		target_vertex_desc = boost::target(*eit, graph_of_this_function);

		// Get the effective in and out degrees.
		/// @todo
		/*
		source_od = filtered_out_degree(source_vertex_desc, m_cfg);
		target_id = filtered_in_degree(target_vertex_desc, m_cfg);

		// Check if they meet the criteria for a critical edge.
		if((source_od > 1) && (target_id > 1))
		{
			// They do, we've found a critical edge.
			edges_to_remove.push_back(*eit);
		}
		*/
	}

	// Remove the critical edges we found.
	BOOST_FOREACH(T_CFG_EDGE_DESC e, edges_to_remove)
	{
		T_CFG_VERTEX_DESC source_vertex_desc, target_vertex_desc, splitting_vertex;

		// Get the vertex descriptors.
		source_vertex_desc = boost::source(e, graph_of_this_function);
		target_vertex_desc = boost::target(e, graph_of_this_function);

		// Create the new NoOp vertex.
		splitting_vertex = boost::add_vertex(m_cfg);
		m_cfg[splitting_vertex].m_statement = new NoOp(Location("[UNKNOWN : 0]"));
		m_cfg[splitting_vertex].m_containing_function = f;

		// Split the edge by pointing the old edge at the new vertex, and a new fallthrough
		// edge from the new vertex to the old target.
		T_CFG_EDGE_DESC new_edge_1, new_edge_2;
		boost::tie(new_edge_1, boost::tuples::ignore) = boost::add_edge(source_vertex_desc, splitting_vertex, m_cfg);
		boost::tie(new_edge_2, boost::tuples::ignore) = boost::add_edge(splitting_vertex, target_vertex_desc, m_cfg);
		m_cfg[new_edge_1].m_edge_type = m_cfg[e].m_edge_type;
		m_cfg[e].m_edge_type = NULL;
		m_cfg[new_edge_2].m_edge_type = new CFGEdgeTypeFallthrough();
		boost::remove_edge(e, m_cfg);
	}
}

void ControlFlowGraph::AddEdge(const T_CFG_VERTEX_DESC & source, const T_CFG_VERTEX_DESC & target)
{
	boost::add_edge(source, target, m_cfg);
}

void ControlFlowGraph::ChangeEdgeTarget(T_CFG_EDGE_DESC & e, const T_CFG_VERTEX_DESC & target)
{
	/// @todo Implement this.
}

T_CFG_VERTEX_DESC ControlFlowGraph::AddVertex(StatementBase *statement, Function *containing_function)
{
	T_CFG_VERTEX_DESC retval;

	retval = boost::add_vertex(m_cfg);
	m_cfg[retval].m_statement = statement;
	m_cfg[retval].m_containing_function = containing_function;

	return retval;
}

T_CFG_EDGE_DESC ControlFlowGraph::AddEdge(const T_CFG_VERTEX_DESC & source, const T_CFG_VERTEX_DESC & target, CFGEdgeTypeBase *edge_type)
{
	T_CFG_EDGE_DESC eid;
	bool ok;

	boost::tie(eid, ok) = boost::add_edge(source, target, m_cfg);
	// Since this edge is within the block, it is just a fallthrough.
	m_cfg[eid].m_edge_type = edge_type;

	return eid;
}

void ControlFlowGraph::ChangeEdgeSource(T_CFG_EDGE_DESC & e, const T_CFG_VERTEX_DESC & source)
{
}

void ControlFlowGraph::RemoveEdge(const T_CFG_EDGE_DESC & e)
{
	boost::remove_edge(e, m_cfg);
}

void ControlFlowGraph::StructureCompoundConditionals(Function *f)
{
#if 0
	// ... postorder traversal...
	T_CFG_VERTEX_DESC v;
	T_FILTERED_GRAPH fg;
	If *if_ptr, *if_ptr2;

	if(fg[v].m_statement->IsType<If>())
	{
		// This is an If vertex, see if we need to restructure it.
		true_successor = if_ptr->GetTrueSuccessor();
		false_successor = if_ptr->GetFalseSuccessor();

		if(GetStatementPtr(true_successor)->IsType<If>()
				&& InDegree(true_successor) == 1)
		{
			if_ptr2 = GetStatementPtr(true_successor);

			if(if_ptr2->GetTrueSuccessor() == false_successor)
			{
				// The structure is (t,f):
				//      1
				//     / \
				//    2   |
				//   / \ /
				//  t   f

				// The structure is (t,f):
				//      1
				//     / \
				//    2   |
				//   / \ /
				//  t   f
			}
		}
	}
#endif
}
