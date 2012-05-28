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

/** @file
 * Header for the ControlFlowGraph class.
 */

#ifndef CONTROLFLOWGRAPH_H
#define	CONTROLFLOWGRAPH_H

#include <utility>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/utility.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "statements/statements.h"
#include "edges/CFGEdgeTypeBase.h"
#include "VertexID.h"

#include "Graph.h"
#include "GraphAdapter.h"

class Function;


/// @name Control Flow Graph definitions.
//@{

/// The vertex descriptor type for ControlFlowGraphs.
//typedef DescriptorBaseClass<StatementBase> CFGVertexDescriptor;
typedef StatementBase* CFGVertexDescriptor;


/**
 * Functor for use with transform_iterator<> to convert VertexDescriptor's into CFGVertexDescriptor's.
 */
struct CFGVertexDescriptorConv
{
	/// This is for boost::result_of().
	typedef CFGVertexDescriptor result_type;

	CFGVertexDescriptor operator()(const VertexDescriptor& v) const { return dynamic_cast<CFGVertexDescriptor>(v); };
};



/**
 * The primary control flow graph class.
 */
class ControlFlowGraph : public Graph //: boost::noncopyable
{
public:
	/// @name Public member types.
	//@{

	typedef CFGVertexDescriptor vertex_descriptor;
	typedef boost::transform_iterator< CFGVertexDescriptorConv, vertex_list_type::iterator> vertex_iterator;
	//typedef Graph::vertex_iterator vertex_iterator;
	typedef StatementBase::out_edge_iterator out_edge_iterator;

	static inline vertex_descriptor null_vertex() { return NULL; /*CFGVertexDescriptor::GetNullDescriptor();*/ };


	/// @name These are specifically for interoperability with the Boost graph library.
	//@{
	typedef CFGEdgeDescriptor edge_descriptor;
	typedef StatementBase::edge_iterator edge_iterator;
	typedef StatementBase::in_edge_iterator in_edge_iterator;
	typedef StatementBase::degree_size_type degree_size_type;
	typedef boost::directed_tag directed_category;
	typedef boost::bidirectional_graph_tag traversal_category;
	typedef boost::allow_parallel_edge_tag edge_parallel_category;
	/// (@todo AFAICT, the BidirectionalGraph concept doesn't need the below three, but a concept check of that chokes if they're not
	/// in here.  boost::graph_traits<> appears to always need them.)
	// AdjacencyGraph
	typedef vertex_iterator adjacency_iterator;
	// VertexListGraph (efficient traversal of all vertices in graph)
	typedef vertex_list_type::size_type vertices_size_type;
	// EdgeListGraph (efficient traversal of all edges in graph)
	typedef edge_list_type::size_type edges_size_type;

	/// For vertex_index_t.
	//typedef boost::property<boost::vertex_index_t, long> VertexProperty;
	//typedef VertexProperty vertex_property_type;
	//@}

	//@}

public:
	ControlFlowGraph();
	virtual ~ControlFlowGraph();

	/// @name Graph construction helpers
	//@{

	//virtual void InsertMergeNodes(Function *f);

	//virtual void SplitCriticalEdges(Function *f);

	//virtual void StructureCompoundConditionals(Function *f);

	//virtual void RemoveRedundantNodes(Function *f);

	//@}

	/// @name Debugging helper functions
	//@{
	virtual void PrintOutEdgeTypes(vertex_descriptor vdesc);
	virtual void PrintInEdgeTypes(vertex_descriptor vdesc);
	//@}

	/// @name Property Map Functions
	//@{
	//@}


	StatementBase* operator[](const ControlFlowGraph::vertex_descriptor vd) { return vd; };
	CFGEdgeTypeBase* operator[](const ControlFlowGraph::edge_descriptor ed) { return ed; };
	const StatementBase* operator[](const ControlFlowGraph::vertex_descriptor vd) const { return vd; };
	const CFGEdgeTypeBase* operator[](const ControlFlowGraph::edge_descriptor ed) const { return ed; };

	virtual void Vertices(ControlFlowGraph::vertex_iterator* ibegin, ControlFlowGraph::vertex_iterator* iend) const;

private:

};


//@}

/// @name Free-function declarations for adapting this graph class to the Boost graph library.
//@{

	std::pair<ControlFlowGraph::out_edge_iterator, ControlFlowGraph::out_edge_iterator>
		out_edges(ControlFlowGraph::vertex_descriptor u, const ControlFlowGraph &/*g*/);
	std::pair<ControlFlowGraph::in_edge_iterator, ControlFlowGraph::in_edge_iterator>
		in_edges(ControlFlowGraph::vertex_descriptor u, const ControlFlowGraph &/*g*/);


	//std::pair<ControlFlowGraph::vertex_iterator, ControlFlowGraph::vertex_iterator> vertices(ControlFlowGraph& g) { return vertices(g); };
	std::pair<ControlFlowGraph::vertex_iterator, ControlFlowGraph::vertex_iterator> vertices(const ControlFlowGraph& g);


	inline void checker(ControlFlowGraph&)
	{
		// Concept checks.
		BOOST_CONCEPT_ASSERT(( boost::GraphConcept<ControlFlowGraph> ));
		//BOOST_CONCEPT_ASSERT(( boost::VertexListGraphConcept<ControlFlowGraph> ));
		//BOOST_CONCEPT_ASSERT(( boost::BidirectionalGraphConcept<ControlFlowGraph> ));
		//BOOST_CONCEPT_ASSERT(( boost::MutableGraphConcept<ControlFlowGraph> ));
		//BOOST_CONCEPT_ASSERT(( boost::MutablePropertyGraph<ControlFlowGraph> ));
		//BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyGraphConcept<ControlFlowGraph, ControlFlowGraph::vertex_descriptor, boost::vertex_index_t> ));
	}


//@}

#endif	/* CONTROLFLOWGRAPH_H */
