/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include "cfg_algs.h"

#include <boost/concept_check.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>

#include "../ControlFlowGraph.h"
#include "../../Function.h"
#include "../visitors/BackEdgeFixupVisitor.h"
#include "../edges/CFGEdgeTypeImpossible.h"


/// Property map typedef for property maps which allow us to get at the function pointer stored at
/// CFGVertexProperties::m_containing_function in the T_CFG.
//typedef boost::property_map<ControlFlowGraph, Function* StatementBase::*>::type T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION;

//typedef boost::property_map<ControlFlowGraph, size_t StatementBase::*>::type T_VERTEX_PROPERTY_MAP_INDEX;


void FixupBackEdges(ControlFlowGraph &g, Function *f)
{
#if 0
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vpm = g.GetPropMap_ContainingFunction();
	vertex_filter_predicate the_vertex_filter(vpm, f);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
	// Define a filtered view of only this function's CFG.
	T_FILTERED_GRAPH graph_of_this_function(m_cfg, boost::keep_all(), the_vertex_filter);
#endif

	// Check that BackEdgeFixupVisitor models DFSVisitorConcept.
	/// @todo This is probably not the best place for this check.
	BOOST_CONCEPT_ASSERT(( boost::GraphConcept<ControlFlowGraph> ));
	BOOST_CONCEPT_ASSERT((boost::DFSVisitorConcept< BackEdgeFixupVisitor<ControlFlowGraph>, ControlFlowGraph >));

	std::vector<BackEdgeFixupVisitor<ControlFlowGraph>::BackEdgeFixupInfo> back_edges;

	// Define a visitor which will find all the back edges and send back the info
	// we need to fix them up.
	BackEdgeFixupVisitor<ControlFlowGraph> back_edge_finder(back_edges);

	//typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef boost::default_color_type T_COLOR;
	typedef std::map<ControlFlowGraph::vertex_descriptor, T_COLOR> T_COLORMAP;
	T_COLORMAP color_map;
	typedef boost::associative_property_map<T_COLORMAP> T_COLOR_PROPERTY_MAP;
	T_COLOR_PROPERTY_MAP color_property_map(color_map);

	// Check if the color_property_map fulfills the concept.
	BOOST_CONCEPT_ASSERT((boost::ReadWritePropertyMapConcept< T_COLOR_PROPERTY_MAP, ControlFlowGraph::vertex_descriptor >));

	// Set the back_edge_finder visitor loose on the function's CFG, with its
	// search strategy being a simple depth-first search.
	// Locate all the back edges, and send the fix-up info back to the back_edges
	// std::vector<> above.
	boost::depth_first_search(g, /*boost::visitor(back_edge_finder)*/back_edge_finder, color_property_map);

	// Mark the edges we found as back edges.
	BOOST_FOREACH(BackEdgeFixupVisitor<ControlFlowGraph>::BackEdgeFixupInfo fixinfo, back_edges)
	{
		ControlFlowGraph::edge_descriptor e = fixinfo.m_back_edge;

		// Change this edge type to a back edge.
		e->MarkAsBackEdge(true);
		g[e]->MarkAsBackEdge(true);

		// Skip the rest if this is a self edge.
		if(fixinfo.m_impossible_target_vertex == boost::graph_traits<ControlFlowGraph>::null_vertex())
		{
			dlog_cfg << "Self edge, no further action: " << e << std::endl;
			continue;
		}

		// If the source node of this back edge now has no non-back-edge out-edges,
		// add a CFGEdgeTypeImpossible edge to it, so topological sorting works correctly.
		ControlFlowGraph::vertex_descriptor src;
		src = /*boost::*/source(e, g);
		if (/*boost::*/out_degree(src, g) == 1)
		{
			/*ControlFlowGraph::edge_descriptor newedge;
			boost::tie(newedge, boost::tuples::ignore) =
					boost::add_edge(src, fixinfo.m_impossible_target_vertex, g);
			m_cfg[newedge].m_edge_type = new CFGEdgeTypeImpossible;*/
			g.AddEdge(src, fixinfo.m_impossible_target_vertex, new CFGEdgeTypeImpossible);

			dlog_cfg << "Retargetting back edge " << e << " to " << fixinfo.m_impossible_target_vertex << std::endl;
		}
	}

	dlog_cfg << "Back edge fixup complete." << std::endl;
}

#if 0
void ControlFlowGraph::InsertMergeNodes(Function *f)
{

	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vpm = boost::get(
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
		BOOST_FOREACH(ControlFlowGraph::edge_descriptor e, mi.m_terminal_edges)
		{
				cout << e << endl;
		}
	}

	// Now modify the tree.
	ControlFlowGraph::edge_descriptor last_merge_vertex_out_edge;
	BOOST_FOREACH(MergeNodeInsertionVisitor<T_FILTERED_GRAPH>::MergeInsertionInfo mii, returned_merge_info)
	{
		T_CFG_VERTEX_DESC merge_vertex, last_merge_vertex;
		ControlFlowGraph::edge_descriptor new_edge, last_merge_edge;
		CFGEdgeTypeBase *new_edge_type;

		if(mii.m_terminal_edges.size() < 3)
		{
			// Something went wrong, this should never be less than 3.
			std::cerr << "ERROR: Less than 3 vertices in InsertMergeNodes()." << std::endl;
		}

		std::vector<ControlFlowGraph::edge_descriptor>::iterator eit, eend;
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
		std::vector<ControlFlowGraph::edge_descriptor>::iterator eit, eend;
		eit = mii.m_terminal_edges.begin();
		eend = mii.m_terminal_edges.end();
		for(; eit+1 != eend; ++eit)
		{
			RemoveEdge(*eit);
		}
	}

}
#endif

#if 0
void ControlFlowGraph::SplitCriticalEdges(Function *f)
{

	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vpm = boost::get(
			&CFGVertexProperties::m_containing_function, m_cfg);
	vertex_filter_predicate the_vertex_filter(vpm, f);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
	// Define a filtered view of only this function's CFG.
	T_FILTERED_GRAPH graph_of_this_function(m_cfg, boost::keep_all(), the_vertex_filter);

	std::vector<ControlFlowGraph::edge_descriptor> edges_to_remove;

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
	BOOST_FOREACH(ControlFlowGraph::edge_descriptor e, edges_to_remove)
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
		ControlFlowGraph::edge_descriptor new_edge_1, new_edge_2;
		boost::tie(new_edge_1, boost::tuples::ignore) = boost::add_edge(source_vertex_desc, splitting_vertex, m_cfg);
		boost::tie(new_edge_2, boost::tuples::ignore) = boost::add_edge(splitting_vertex, target_vertex_desc, m_cfg);
		m_cfg[new_edge_1].m_edge_type = m_cfg[e].m_edge_type;
		m_cfg[e].m_edge_type = NULL;
		m_cfg[new_edge_2].m_edge_type = new CFGEdgeTypeFallthrough();
		boost::remove_edge(e, m_cfg);
	}

}
#endif
#if 0
void ControlFlowGraphBase::StructureCompoundConditionals(Function *f)
{

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

}
#endif
#if 0
void ControlFlowGraphBase::RemoveRedundantNodes(Function* f)
{
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vpm = GetPropMap_ContainingFunction();
	vertex_filter_predicate the_vertex_filter(vpm, f);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
					vertex_filter_predicate> T_FILTERED_GRAPH;
	// Define a filtered view of only this function's CFG.
	T_FILTERED_GRAPH graph_of_this_function(m_cfg, boost::keep_all(), the_vertex_filter);

	std::vector<T_CFG_VERTEX_DESC> vertices_to_remove;

	T_FILTERED_GRAPH::vertex_iterator it, it2;

	boost::tie(it, it2) = boost::vertices(graph_of_this_function);

	// Iterate over all vertices.
	for(;it != it2; ++it)
	{
		StatementBase *sbp = GetStatementPtr(*it);
		if(sbp->IsType<Goto>())
		{
			// This is a Goto.  Check if it's redundant.
			if(boost::in_degree(*it, graph_of_this_function)==1 && boost::out_degree(*it, graph_of_this_function)==1)
			{
				// It's redundant.

				T_FILTERED_GRAPH::in_edge_iterator in_eit;
				T_FILTERED_GRAPH::out_edge_iterator out_eit;
				ControlFlowGraph::edge_descriptor in_edge, out_edge;

				boost::tie(in_eit, boost::tuples::ignore) = boost::in_edges(*it, graph_of_this_function);
				boost::tie(out_eit, boost::tuples::ignore) = boost::out_edges(*it, graph_of_this_function);
				in_edge = *in_eit;
				out_edge = *out_eit;

				T_CFG_VERTEX_DESC source_vertex_desc, target_vertex_desc;
				long target_id, source_od;

				// Get the vertex descriptors.
				source_vertex_desc = boost::source(in_edge, graph_of_this_function);
				target_vertex_desc = boost::target(out_edge, graph_of_this_function);

				// Point the incoming edge to the target of our outgoing edge, bypassing us.
				ChangeEdgeTarget(in_edge, target_vertex_desc);

				// Remove our outgoing edge.
				RemoveEdge(out_edge);

				// We should now have no edges.
				if(boost::degree(*it, m_cfg) != 0)
				{
					std::cerr << "STILL HAS EDGES" << std::endl;
				}
				else
				{
					// Delete this vertex.
					vertices_to_remove.push_back(*it);
				}
			}
		}

	}

	// Remove all the vertices we found.
	BOOST_FOREACH(T_CFG_VERTEX_DESC i, vertices_to_remove)
	{
		RemoveVertex(i);
	}
}
#endif

