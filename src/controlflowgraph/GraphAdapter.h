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

/**
 * @file
 * Header file containing the graph adapter free functions which adapt the Graph class
 *       (and any derived classes) to the Boost Graph Library's interface.
 */

#ifndef GRAPHADAPTER_H
#define GRAPHADAPTER_H

#include "Graph.h"
#include "ControlFlowGraph.h"

#include <boost/utility/enable_if.hpp>
//#include <boost/type_traits/is_base_of.hpp>

#include "coflo_exceptions.hpp"

/// @name Free-function declarations for adapting this graph class to the Boost graph library.
//@{

/// @name Functions for the IncidenceGraph concept.
///@{
template <typename GraphType>
inline typename boost::graph_traits<GraphType>::vertex_descriptor target(const typename GraphType::edge_descriptor &e, const GraphType &/*g*/)
{
	return typename GraphType::vertex_descriptor(e->Target());
};

template <typename GraphType>
inline typename boost::graph_traits<GraphType>::vertex_descriptor source(const typename GraphType::edge_descriptor &e, const GraphType &/*g*/)
{
	return typename GraphType::vertex_descriptor(e->Source());
};

template <typename GraphType>
inline typename GraphType::degree_size_type out_degree(typename GraphType::vertex_descriptor u, const GraphType& /*g*/)
{
	return u->OutDegree();
};

template <typename GraphType>
inline std::pair<typename GraphType::out_edge_iterator, typename GraphType::out_edge_iterator>
out_edges(typename GraphType::vertex_descriptor u, const GraphType &/*g*/)
{
	return u->OutEdges();
};

///@}

/// @name The BidirectionalGraph concept adds these.
///@
template <typename GraphType>
inline typename GraphType::degree_size_type in_degree(typename GraphType::vertex_descriptor u, const GraphType& /*g*/)
{
	return u->InDegree();
};


template <typename GraphType>
inline std::pair<typename GraphType::in_edge_iterator, typename GraphType::in_edge_iterator>
in_edges(typename GraphType::vertex_descriptor u, const GraphType &/*g*/)
{
	return u->InEdges();
};
///@}

/// @name the VertexListGraph concept adds these to the Graph concept.
///@{

template <typename GraphType>
inline std::pair<typename GraphType::vertex_iterator, typename GraphType::vertex_iterator> vertices(const GraphType& g)
{
	std::pair<typename GraphType::vertex_iterator, typename GraphType::vertex_iterator> retval;

	g.Vertices(&retval);

	return retval;
};

template <typename GraphType>
inline typename GraphType::vertices_size_type num_vertices(const GraphType& g)
{
	return g.NumVertices();
};

///@}

// VertexIndexGraphConcept requires this.
inline void renumber_vertex_indices(Graph &/*g*/) {};

/// Index property map (for vertex_index_t and edge_index_t).
template <typename VertexOrEdgeDescriptorType>
class Generic_index_map
{
public:
	typedef boost::readable_property_map_tag category;
	typedef std::size_t value_type;
	typedef std::size_t reference;
	typedef VertexOrEdgeDescriptorType key_type;
	Generic_index_map() /*: m_g(0)*/ { };
	Generic_index_map(const Graph& /*g*/) /*: m_g(&g)*/ { }
	Generic_index_map(const Generic_index_map& /*other*/) {};
	//reference operator[](const key_type v) const { return (*v)->GetVertexIndex(); };
protected:
	//const Graph* m_g;
};

template <typename VertexOrEdgeDescriptorType>
inline typename Generic_index_map<VertexOrEdgeDescriptorType>::reference get(const Generic_index_map<VertexOrEdgeDescriptorType>& /*pmap*/,
		const typename Generic_index_map<VertexOrEdgeDescriptorType>::key_type& key)
{
	return key->GetIndex();
};

typedef Generic_index_map<Graph::vertex_descriptor> Graph_vertex_index_map;
typedef Generic_index_map<Graph::edge_descriptor> Graph_edge_index_map;

inline Graph_vertex_index_map get(boost::vertex_index_t, const Graph& g)
{
	return Graph_vertex_index_map(g);
};
inline Graph_edge_index_map get(boost::edge_index_t, const Graph& g)
{
	return Graph_edge_index_map(g);
};

inline std::size_t get(boost::vertex_index_t, const Graph& /*g*/, Graph::vertex_descriptor v)
{
	return v->GetIndex();
};

namespace boost
{
	/// Property map traits specializations.
	template <>
	struct property_map<Graph, boost::vertex_index_t>
	{
		typedef Graph_vertex_index_map type;
		typedef const Graph_vertex_index_map const_type;
	};
	template <>
	struct property_map<const Graph, boost::vertex_index_t>
	{
		typedef const Graph_vertex_index_map const_type;
	};
};

/// @name Free functions for implementing the MutableGraph concept.
//@{

/**
 * Create a new Vertex and add it to Graph @a g.
 *
 * @param g The instance of Graph to add the new Vertex to.
 * @return Descriptor of the new vertex.
 */
inline Graph::vertex_descriptor add_vertex(Graph& g)
{
	/// @todo We probably need to use the "virtual constructor" pattern here.
	Vertex *v = new Vertex();

	g.AddVertex(v);

	return VertexDescriptor(v);
};

inline std::pair<Graph::edge_descriptor, bool> add_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g)
{
	Edge *e = new Edge();

	g.AddEdge(u, v, e);

	/// @todo From the BGL docs: "If the graph disallows parallel edges, and the edge (u,v) is already in the graph,
	/// then the bool flag returned is false and the returned edge descriptor points to the already existing edge.".

	return std::make_pair(e, true);
};

inline void remove_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g)
{
	u=u; v=v; g=g;
	BOOST_THROW_EXCEPTION( not_implemented() );
};

template <typename GraphType>
inline void remove_edge(typename GraphType::edge_descriptor e, GraphType &g)
{
	g.RemoveEdge(e);
};

inline void clear_vertex(Graph::vertex_descriptor u, Graph &g)
{
	u=u; g=g;
	BOOST_THROW_EXCEPTION( not_implemented() );
};

typedef void (Graph::*GraphMemberFunction)(typename Graph::vertex_descriptor);

template <typename GraphType>
inline /*typename boost::enable_if<typename GraphType::is_derived_from_Graph_t, void>::type*/ void
remove_vertex(typename GraphType::vertex_descriptor u, GraphType &g)
{
	g.RemoveVertex(u);
};
//@}

/// @name Free functions for implementing the EdgeListGraph concept.
//@{
	std::pair<Graph::edge_iterator, Graph::edge_iterator> edges(const Graph& g);
	Graph::edges_size_type num_edges(const Graph& g);
//@}

//@}

/**
 * Concept checker function for the Graph class.
 * This is templatized so that it never actually gets instantiated, hence wasting space in the resulting binary.
 *
 * @note Don't try to instantiate or use this function.  It's solely here to check concepts associated with Graph,
 * which are done at compile time.
 */
template <typename Dummy>
void Graph_concept_checker()
{
	// Is our vertex index map a ReadablePropertyMap?
	BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyMapConcept<const Graph_vertex_index_map, const Graph::vertex_descriptor> ));
	// Is our vertex index map a WritablePropertyMap?
	//BOOST_CONCEPT_ASSERT(( boost::WritablePropertyMapConcept<boost::Graph_vertex_id_map, Graph::vertex_descriptor> ));
	// Is our vertex index map a ReadWritePropertyMap?
	//BOOST_CONCEPT_ASSERT(( boost::ReadWritePropertyMapConcept<boost::Graph_vertex_id_map, Graph::vertex_descriptor> ));

	// Check iterators.
	BOOST_CONCEPT_ASSERT(( boost::DefaultConstructibleConcept< Graph::vertex_descriptor > ));
    BOOST_CONCEPT_ASSERT(( boost::EqualityComparableConcept< Graph::vertex_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::AssignableConcept<Graph::vertex_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::DefaultConstructibleConcept<Graph::edge_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::EqualityComparableConcept<Graph::edge_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::AssignableConcept<Graph::edge_descriptor> ));

	// Is our Graph class a Boost Graph?
	BOOST_CONCEPT_ASSERT(( boost::GraphConcept<Graph> ));

	// Need to model IncidenceGraph for depth_first_search.
	// According to the docs, we need to model VertexListGraph too, but the code doesn't appear to need it.
	BOOST_CONCEPT_ASSERT(( boost::IncidenceGraphConcept<Graph> ));
	// Is our Graph class a ReadablePropertyGraph for vertex_index_t?
	BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyGraphConcept<Graph, Graph::vertex_descriptor, boost::vertex_index_t> ));

	// Need to model VertexListGraph for a number of algorithms.
	BOOST_CONCEPT_ASSERT(( boost::VertexListGraphConcept<Graph> ));

	// Need to model EdgeListGraph for write_graphviz().
	BOOST_CONCEPT_ASSERT(( boost::EdgeListGraphConcept<Graph> ));

	// Is our Graph class a PropertyGraph for vertex_color_t?
	//BOOST_CONCEPT_ASSERT(( boost::PropertyGraphConcept<Graph, Graph::vertex_descriptor, boost::vertex_color_t> ));
	BOOST_CONCEPT_ASSERT(( boost::VertexIndexGraphConcept<Graph> ));

	BOOST_CONCEPT_ASSERT(( boost::VertexMutableGraphConcept<Graph> ));
	BOOST_CONCEPT_ASSERT(( boost::EdgeMutableGraphConcept<Graph> ));
	BOOST_CONCEPT_ASSERT(( boost::MutableGraphConcept<Graph> ));
};

#endif
