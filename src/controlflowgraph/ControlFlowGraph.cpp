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

/// Always include a .cpp file's own header first.  This ensures that the header file is
/// idempotent at compile time.  If it isn't, the compile will fail, alerting you to the problem.
#include "ControlFlowGraph.h"

#include "GraphAdapter.h"

#include <utility>

#include <boost/foreach.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_concepts.hpp>


#include "visitors/BackEdgeFixupVisitor.h"
#include "visitors/MergeNodeInsertionVisitor.h"
#include "edges/edge_types.h"
#include "../Function.h"

using std::cout;
using std::cerr;
using std::endl;

#if 0

/**
 * Predicate for filtering the CFG for only the vertices of the given function.
 */
struct vertex_filter_predicate
{
	vertex_filter_predicate()
	{
	}
	;
	vertex_filter_predicate(T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vertex_prop_map,
			Function *parent_function) :
			m_vertex_prop_map(vertex_prop_map), m_parent_function(parent_function)
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

	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION m_vertex_prop_map;
	Function *m_parent_function;
};
#endif

ControlFlowGraph::ControlFlowGraph()
{
}

ControlFlowGraph::~ControlFlowGraph()
{
}

#if 0

void ControlFlowGraph::InsertMergeNodes(Function *f)
{
#if 0
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
#endif
}


void ControlFlowGraph::SplitCriticalEdges(Function *f)
{
#if 0
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vpm = boost::get(
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
#endif
}

#endif


void ControlFlowGraph::Vertices(ControlFlowGraph::vertex_iterator* ibegin, ControlFlowGraph::vertex_iterator* iend) const
{
	*ibegin = boost::make_transform_iterator< CFGVertexDescriptorConv, vertex_list_type::iterator>(m_vertices.begin());
	*iend = boost::make_transform_iterator< CFGVertexDescriptorConv, vertex_list_type::iterator>(m_vertices.end());
}


#if 0
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


void ControlFlowGraph::RemoveRedundantNodes(Function* f)
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
				T_CFG_EDGE_DESC in_edge, out_edge;

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

void ControlFlowGraph::PrintOutEdgeTypes(ControlFlowGraph::vertex_descriptor vdesc)
{
	out_edge_iterator ei, eend;

	boost::tie(ei, eend) = out_edges(vdesc, *this);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*((*this)[*ei])).name() << std::endl;
	}
}

void ControlFlowGraph::PrintInEdgeTypes(ControlFlowGraph::vertex_descriptor vdesc)
{
	in_edge_iterator ei, eend;

	boost::tie(ei, eend) = in_edges(vdesc, *this);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*((*this)[*ei])).name() << std::endl;
	}
}

/// @name Free-function definitions for adapting this graph class to the Boost graph library.
//@{
ControlFlowGraph::vertex_descriptor target(const ControlFlowGraph::edge_descriptor e, const ControlFlowGraph &/*g*/) { return e->Target(); };
ControlFlowGraph::vertex_descriptor source(const ControlFlowGraph::edge_descriptor e, const ControlFlowGraph &/*g*/) { return e->Source(); };


	std::pair<ControlFlowGraph::out_edge_iterator, ControlFlowGraph::out_edge_iterator>
	out_edges(ControlFlowGraph::vertex_descriptor u, const ControlFlowGraph &/*g*/)
	{
		ControlFlowGraph::out_edge_iterator r1, r2;
		u->OutEdges(&r1, &r2);
		return std::make_pair(r1, r2);
	};
	std::pair<ControlFlowGraph::in_edge_iterator, ControlFlowGraph::in_edge_iterator>
	in_edges(ControlFlowGraph::vertex_descriptor u, const ControlFlowGraph &/*g*/)
	{
		ControlFlowGraph::out_edge_iterator r1, r2;
		u->InEdges(&r1, &r2);
		return std::make_pair(r1, r2);
	};

	std::pair<ControlFlowGraph::vertex_iterator, ControlFlowGraph::vertex_iterator> vertices(const ControlFlowGraph& g)
	{
		ControlFlowGraph::vertex_iterator ibegin, iend;

		g.Vertices(&ibegin, &iend);

		return std::make_pair(ibegin, iend);
	}

//@}

