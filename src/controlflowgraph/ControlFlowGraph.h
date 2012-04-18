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
#include "SparsePropertyMap.h"

#include "Graph.h"

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
#if 0
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
#endif

typedef StatementBase* T_CFG_VERTEX_DESC;
typedef CFGEdgeTypeBase* T_CFG_EDGE_DESC;


// Forward declare the FilteredGraph class template.
template < typename EdgeFilterPredicate, typename VertexFilterPredicate >
class FilteredGraph;

struct CastToStatementBaseReference
{
	StatementBase*& operator()(Vertex* const &v) const
	{
		return (StatementBase*&)v;
	};

	/// This is for boost::result_of().
	typedef StatementBase*& result_type;
};

/**
 * The primary control flow graph class.
 * @todo Well, it will be.  At the moment too much functionality is implemented in terms of the Boost Graph Library's
 *       free functions acting on the underlying T_CFG.
 *
 */
class ControlFlowGraph : public Graph //: boost::noncopyable
{
public:
	/// @name Public member types.
	//@{
	typedef StatementBase* vertex_descriptor;
	typedef boost::transform_iterator< CastToStatementBaseReference, Graph::vertex_iterator, StatementBase*&, StatementBase* > vertex_iterator;
	typedef StatementBase::out_edge_iterator out_edge_iterator;

	static inline vertex_descriptor null_vertex() { return NULL; };


	/// @name These are specifically for interoperability with the Boost graph library.
	//@{
	typedef CFGEdgeTypeBase* edge_descriptor;
	typedef StatementBase::edge_iterator edge_iterator;
	typedef StatementBase::in_edge_iterator in_edge_iterator;
	typedef StatementBase::degree_size_type degree_size_type;
	typedef boost::directed_tag directed_category;
	typedef boost::allow_parallel_edge_tag edge_parallel_category;
	typedef boost::bidirectional_graph_tag traversal_category;
	/// (@todo AFAICT, the BidirectionalGraph concept doesn't need the below three, but a concept check of that chokes if they're not
	/// in here.  boost::graph_traits<> appears to always need them.)
	// AdjacencyGraph
	typedef vertex_iterator adjacency_iterator;
	// VertexListGraph (efficient traversal of all vertices in graph)
	typedef vertex_list_type::size_type vertices_size_type;
	// EdgeListGraph (efficient traversal of all edges in graph)
	typedef edge_list_type edges_size_type;
	//@}

	//@}

public:
	ControlFlowGraph();
	virtual ~ControlFlowGraph();

	/**
	 * This returns a reference to the underlying T_CFG object.
	 *
	 * @deprecated Ultimately, the goal is to make exposing this unnecessary.  Avoid using this interface if
	 * at all possible.
	 *
	 * @return A reference to the underlying T_CFG (Boost Graph Library adjacency_list) object.
	 */
	//const T_CFG& GetConstT_CFG() const;

	/// @name Graph construction helpers
	//@{

	/**
	 * Traverses the CFG of Function @a f and marks all back edges.
	 * @param f
	 */
	//virtual void FixupBackEdges(Function *f);

	//virtual void InsertMergeNodes(Function *f);

	//virtual void SplitCriticalEdges(Function *f);

	//virtual void StructureCompoundConditionals(Function *f);

	//virtual void RemoveRedundantNodes(Function *f);

	//@}

	/// @name Debugging helper functions
	//@{
	virtual void PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	virtual void PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	//@}

	//virtual T_CFG_VERTEX_DESC AddVertex(StatementBase * statement, Function *containing_function);
	//virtual T_CFG_EDGE_DESC AddEdge(const T_CFG_VERTEX_DESC &source, const T_CFG_VERTEX_DESC &target, CFGEdgeTypeBase *edge_type);

	/// @name Vertex attribute accessors.
	//@{

	//virtual VertexID GetID(T_CFG_VERTEX_DESC vdesc) = 0;

	//virtual std::pair< T_CFG_OUT_EDGE_ITERATOR, T_CFG_OUT_EDGE_ITERATOR > OutEdges(T_CFG_VERTEX_DESC vdesc) = 0;

	//virtual StatementBase* GetStatementPtr(T_CFG_VERTEX_DESC v) = 0;

	//virtual void ReplaceStatementPtr(T_CFG_VERTEX_DESC v, StatementBase *new_statement_base) = 0;

	/**
	 * Return the in degree of vertex @a v.
	 *
	 * @param v
	 * @return The in degree of @v.
	 */
	//virtual T_CFG_DEGREE_SIZE_TYPE InDegree(T_CFG_VERTEX_DESC v) = 0;

	//@}

	/// @name Edge attribute accessors.
	//@{

	/**
	 * Return the source vertex of edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return The source vertex of @a e.
	 */
	//virtual T_CFG_VERTEX_DESC Source(T_CFG_EDGE_DESC e) = 0;

	/**
	 * Return the target vertex of edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return The target vertex of @a e.
	 */
	//virtual T_CFG_VERTEX_DESC Target(T_CFG_EDGE_DESC e) = 0;

	/**
	 * Returns a pointer to the CFGEdgeTypeBase-derived edge type class associated with edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return Pointer to the CFGEdgeTypeBase-derived edge type class associated with edge @a e.
	 */
	//virtual CFGEdgeTypeBase* GetEdgeTypePtr(T_CFG_EDGE_DESC e) = 0;

	virtual void ReplaceEdgeTypePtr(T_CFG_EDGE_DESC e, CFGEdgeTypeBase *new_edge_type_ptr);

	//@}



	/// @name Property Map Functions
	//@{

	//virtual T_VERTEX_PROPERTY_MAP_CONTAINING_FUNCTION GetPropMap_ContainingFunction();

	//T_VERTEX_PROPERTY_MAP_INDEX GetPropMap_VertexIndex();

	//@}

	StatementBase* operator[](ControlFlowGraph::vertex_descriptor vd) { return vd; };
	CFGEdgeTypeBase* operator[](ControlFlowGraph::edge_descriptor ed) { return ed; };

	virtual void Vertices(ControlFlowGraph::vertex_iterator* ibegin, ControlFlowGraph::vertex_iterator* iend);

private:

	/// @name Vertex unique ID generator routines.
	//@{

	virtual void InitVertexIDGenerator();
	virtual VertexID GetNewVertexID();

	//@}

	virtual void RemoveVertex(T_CFG_VERTEX_DESC v);

	/// @name Edge manipulation routines.
	//@{

	/**
	 * Add an edge between the given source and target vertices.
	 *
	 * @param source Source vertex descriptor.
	 * @param target Target vertex descriptor.
	 */
	//virtual void AddEdge(const T_CFG_VERTEX_DESC &source, const T_CFG_VERTEX_DESC &target);
	virtual void RemoveEdge(const T_CFG_EDGE_DESC &e);
	virtual void ChangeEdgeTarget(T_CFG_EDGE_DESC &e, const T_CFG_VERTEX_DESC &target);
	virtual void ChangeEdgeSource(T_CFG_EDGE_DESC &e, const T_CFG_VERTEX_DESC &source);

	//@}

	/// Reference to the Boost Graph Library graph we'll use for our underlying graph implementation.
	/// We'll point this at *m_cfg_ptr in the constructor.
	/// @note We should probably just use the pointer, this is a leftover from the pre-FilteredGraph implementation.
	///*T_CFG*/ UnderlyingGraphType &m_cfg;

	/// The Vertex ID generator state.
	VertexID m_vertex_id_state;
};


//@}

/// @name Free-function declarations for adapting this graph class to the Boost graph library.
//@{
namespace boost
{
	ControlFlowGraph::vertex_descriptor target(const ControlFlowGraph::edge_descriptor &e, const ControlFlowGraph &/*g*/);
	ControlFlowGraph::vertex_descriptor source(const ControlFlowGraph::edge_descriptor &e, const ControlFlowGraph &/*g*/);

	std::pair<ControlFlowGraph::out_edge_iterator, ControlFlowGraph::out_edge_iterator>
	out_edges(ControlFlowGraph::vertex_descriptor u, const ControlFlowGraph &/*g*/);
	std::pair<ControlFlowGraph::in_edge_iterator, ControlFlowGraph::in_edge_iterator>
	in_edges(ControlFlowGraph::vertex_descriptor u, const ControlFlowGraph &/*g*/);

	std::pair<ControlFlowGraph::vertex_descriptor, ControlFlowGraph::vertex_descriptor> vertices(const ControlFlowGraph& g);
}
//@}

/// @name Other headers in this library.
//@{
#include "visitors/ControlFlowGraphVisitorBase.h"
#include "algorithms/topological_visit_kahn.h"
//@}

#endif	/* CONTROLFLOWGRAPH_H */
