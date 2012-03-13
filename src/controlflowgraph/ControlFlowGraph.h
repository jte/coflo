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

#include "statements/statements.h"
#include "edges/CFGEdgeTypeBase.h"
#include "VertexID.h"
#include "SparsePropertyMap.h"

class Function;


/// @name Control Flow Graph definitions.
//@{

/// Vertex properties for the CFG graph.
struct CFGVertexProperties
{
	/// The index of this vertex.
	//size_t vertex_index;

	/// The Statement at this vertex of the CFG.
	StatementBase *m_statement;
	
	/// The Function which contains this vertex.
	Function *m_containing_function;
};

typedef boost::property<boost::vertex_index_t, size_t,
		boost::property<boost::vertex_bundle_t, CFGVertexProperties> > T_CFG_VERTEX_PROPERTIES;


/// Edge properties for the CFG graph.
struct CFGEdgeProperties
{
	/// The Edge Type.
	/// May be a FunctionCall, fallthrough, goto, etc.
	CFGEdgeTypeBase *m_edge_type;
};

/// Typedef for the CFG graph.
typedef boost::adjacency_list
		<
		/// Selector type to specify the out edge list storage type.
		boost::listS,
		/// Selector type to specify the Vertex list storage type.
		/// @todo REMOVE: boost::vecS,
		boost::listS,
		/// Selector type to specify the directedness of the graph.
		boost::bidirectionalS,
		/// The Vertex properties type.
		T_CFG_VERTEX_PROPERTIES,
		/// The Edge properties type.
		CFGEdgeProperties
		> T_CFG;

/// Typedef for the vertex_descriptors in the control flow graph.
typedef boost::graph_traits<T_CFG>::vertex_descriptor T_CFG_VERTEX_DESC;

/// Typedef for the edge_descriptors in the control flow graph.
typedef boost::graph_traits<T_CFG>::edge_descriptor T_CFG_EDGE_DESC;

/// Typedef for vertex iterators for the CFG.
typedef boost::graph_traits< T_CFG >::vertex_iterator T_CFG_VERTEX_ITERATOR;

/// Typedef for out-edge iterators for the CFG.
typedef boost::graph_traits< T_CFG >::out_edge_iterator T_CFG_OUT_EDGE_ITERATOR;

/// Typedef for in-edge iterators for the CFG.
typedef boost::graph_traits< T_CFG >::in_edge_iterator T_CFG_IN_EDGE_ITERATOR;

/// Typedef for the type used to represent vertex degrees.
typedef boost::graph_traits< T_CFG >::degree_size_type T_CFG_DEGREE_SIZE_TYPE;

// Typedef for the type used to index the Vertices.
typedef boost::graph_traits<T_CFG>::vertices_size_type T_CFG_VERTICES_SIZE_TYPE;


/// Property map typedef for property maps which allow us to get at the function pointer stored at
/// CFGVertexProperties::m_containing_function in the T_CFG.
typedef boost::property_map<T_CFG, Function* CFGVertexProperties::*>::type T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION;

typedef boost::property_map<T_CFG, size_t CFGVertexProperties::*>::type T_VERTEX_PROPERTY_MAP_INDEX;


template < typename CFGEdgeType >
boost::tuple<T_CFG_EDGE_DESC, bool> GetFirstOutEdgeOfType(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg)
{
	boost::graph_traits< T_CFG >::out_edge_iterator eit, eend;
	boost::tuple<T_CFG_EDGE_DESC, bool> retval;
	
	boost::tie(eit, eend) = boost::out_edges(vdesc, cfg);
	for(; eit != eend; eit++)
	{
		if(NULL != dynamic_cast<CFGEdgeType*>(cfg[*eit].m_edge_type))
		{
			// Found it.
			retval = boost::make_tuple(*eit, true);
			return retval;
		}
	}

	// Couldn't find one.
	retval = boost::make_tuple(*eit, false);
	return retval;
}

// Forward declare the FilteredGraph class template.
template < typename EdgeFilterPredicate, typename VertexFilterPredicate >
class FilteredGraph;


/**
 * The primary control flow graph class.
 * @todo Well, it will be.  At the moment too much functionality is implemented in terms of the Boost Graph Library's
 *       free functions acting on the underlying T_CFG.
 *
 */
template <class UnderlyingGraphType = T_CFG>
class ControlFlowGraphBase : boost::noncopyable
{
public:
	ControlFlowGraphBase();
	virtual ~ControlFlowGraphBase();

	template < typename EdgeFilterPredicate, typename VertexFilterPredicate >
	FilteredGraph<EdgeFilterPredicate,VertexFilterPredicate>* CreateFilteredGraph(EdgeFilterPredicate edge_filter, VertexFilterPredicate vertex_filter)
	{
		FilteredGraph<EdgeFilterPredicate, VertexFilterPredicate> *retval;

		retval = new FilteredGraph<EdgeFilterPredicate, VertexFilterPredicate>(*this, m_cfg, edge_filter, vertex_filter);

		return retval;
	}


	/**
	 * This returns a reference to the underlying T_CFG object.
	 *
	 * @deprecated Ultimately, the goal is to make exposing this unnecessary.  Avoid using this interface if
	 * at all possible.
	 *
	 * @return A reference to the underlying T_CFG (Boost Graph Library adjacency_list) object.
	 */
	const T_CFG& GetConstT_CFG() const { return m_cfg; };

	/// @name Graph construction helpers
	//@{

	/**
	 * Traverses the CFG of Function @a f and marks all back edges.
	 * @param f
	 */
	void FixupBackEdges(Function *f);

	void InsertMergeNodes(Function *f);

	void SplitCriticalEdges(Function *f);

	void StructureCompoundConditionals(Function *f);

	void RemoveRedundantNodes(Function *f);

	//@}

	/// @name Debugging helper functions
	//@{
	void PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	void PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	//@}

	T_CFG_VERTEX_DESC AddVertex(StatementBase * statement, Function *containing_function);
	T_CFG_EDGE_DESC AddEdge(const T_CFG_VERTEX_DESC &source, const T_CFG_VERTEX_DESC &target, CFGEdgeTypeBase *edge_type);

	/// @name Vertex attribute accessors.
	//@{

	VertexID GetID(T_CFG_VERTEX_DESC vdesc);

	std::pair< T_CFG_OUT_EDGE_ITERATOR, T_CFG_OUT_EDGE_ITERATOR > OutEdges(T_CFG_VERTEX_DESC vdesc);

	StatementBase* GetStatementPtr(T_CFG_VERTEX_DESC v) { return m_cfg[v].m_statement; };

	void ReplaceStatementPtr(T_CFG_VERTEX_DESC v, StatementBase *new_statement_base);

	/**
	 * Return the in degree of vertex @a v.
	 *
	 * @param v
	 * @return The in degree of @v.
	 */
	T_CFG_DEGREE_SIZE_TYPE InDegree(T_CFG_VERTEX_DESC v) { return boost::in_degree(v, m_cfg); };

	std::pair< T_CFG_VERTEX_ITERATOR, T_CFG_VERTEX_ITERATOR > Vertices();

	template < typename CFGEdgeType >
	boost::tuple<T_CFG_EDGE_DESC, bool> GetFirstOutEdgeOfType(T_CFG_VERTEX_DESC vdesc)
	{
		return ::GetFirstOutEdgeOfType<CFGEdgeType>(vdesc, m_cfg);
	}

	//@}

	/// @name Edge attribute accessors.
	//@{

	/**
	 * Return the source vertex of edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return The source vertex of @a e.
	 */
	T_CFG_VERTEX_DESC Source(T_CFG_EDGE_DESC e) { return boost::source(e, m_cfg); };

	/**
	 * Return the target vertex of edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return The target vertex of @a e.
	 */
	T_CFG_VERTEX_DESC Target(T_CFG_EDGE_DESC e) { return boost::target(e, m_cfg); };

	/**
	 * Returns a pointer to the CFGEdgeTypeBase-derived edge type class associated with edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return Pointer to the CFGEdgeTypeBase-derived edge type class associated with edge @a e.
	 */
	CFGEdgeTypeBase* GetEdgeTypePtr(T_CFG_EDGE_DESC e) { return m_cfg[e].m_edge_type; };

	void ReplaceEdgeTypePtr(T_CFG_EDGE_DESC e, CFGEdgeTypeBase *new_edge_type_ptr);

	//@}



	/// @name Property Map Functions
	//@{

	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION GetPropMap_ContainingFunction();

	//T_VERTEX_PROPERTY_MAP_INDEX GetPropMap_VertexIndex();

	//@}


protected:

	T_CFG& GetT_CFG() { return m_cfg; };

	void ReplaceUnderlyingGraphObjectInstance(T_CFG *new_cfg) { delete m_cfg_ptr; m_cfg_ptr = new_cfg; };

	/// Pointer to the Boost Graph Library graph we'll use for our underlying graph implementation.
	/*T_CFG*/UnderlyingGraphType *m_cfg_ptr;

private:

	/// @name Vertex unique ID generator routines.
	//@{

	void InitVertexIDGenerator();
	VertexID GetNewVertexID();

	//@}

	void RemoveVertex(T_CFG_VERTEX_DESC v);

	/// @name Edge manipulation routines.
	//@{

	/**
	 * Add an edge between the given source and target vertices.
	 *
	 * @param source Source vertex descriptor.
	 * @param target Target vertex descriptor.
	 */
	void AddEdge(const T_CFG_VERTEX_DESC &source, const T_CFG_VERTEX_DESC &target);
	void RemoveEdge(const T_CFG_EDGE_DESC &e);
	void ChangeEdgeTarget(T_CFG_EDGE_DESC &e, const T_CFG_VERTEX_DESC &target);
	void ChangeEdgeSource(T_CFG_EDGE_DESC &e, const T_CFG_VERTEX_DESC &source);

	//@}

	/// Reference to the Boost Graph Library graph we'll use for our underlying graph implementation.
	/// We'll point this at *m_cfg_ptr in the constructor.
	/// @note We should probably just use the pointer, this is a leftover from the pre-FilteredGraph implementation.
	/*T_CFG*/ UnderlyingGraphType &m_cfg;

	/// The Vertex ID generator state.
	VertexID m_vertex_id_state;
};

//#include "ControlFlowGraph.cpp"

template < typename EdgeFilterPredicate, typename VertexFilterPredicate >
ControlFlowGraphBase::ControlFlowGraphBase() : m_cfg(*m_cfg_ptr)
{
	// Create the new Boost graph.
	m_cfg_ptr = new T_CFG;

	InitVertexIDGenerator();
}

ControlFlowGraphBase::~ControlFlowGraphBase()
{
	delete m_cfg_ptr;
}

void ControlFlowGraphBase::PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc)
{
	T_CFG_OUT_EDGE_ITERATOR ei, eend;

	boost::tie(ei, eend) = boost::out_edges(vdesc, m_cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(m_cfg[*ei].m_edge_type)).name() << std::endl;
	}
}

void ControlFlowGraphBase::PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc)
{
	T_CFG_IN_EDGE_ITERATOR ei, eend;

	boost::tie(ei, eend) = boost::in_edges(vdesc, m_cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(m_cfg[*ei].m_edge_type)).name() << std::endl;
	}
}

void ControlFlowGraphBase::FixupBackEdges(Function *f)
{
	// Property map for getting at the edge types in the CFG.
	T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION vpm = GetPropMap_ContainingFunction();
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


class ControlFlowGraph : public ControlFlowGraphBase<ControlFlowGraph>
{
public:
	ControlFlowGraph() { m_cfg_ptr = new T_CFG; };

protected:
	typedef T_CFG underlying_type_t;
};

//@}


/// @name Other headers in this library.
//@{
#include "visitors/ControlFlowGraphVisitorBase.h"
#include "topological_visit_kahn.h"
//@}

#endif	/* CONTROLFLOWGRAPH_H */
