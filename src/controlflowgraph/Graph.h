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

#ifndef GRAPH_H_
#define GRAPH_H_

#include <boost/tr1/unordered_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_archetypes.hpp>
#include <utility>

//class Vertex;
// Including header to get access to member typedefs.
#include "Vertex.h"
#include "Edge.h"
//class Edge;
#include "DescriptorBaseClass.h"


class VertexDescriptor : public DescriptorBaseClass
{
public:
	static VertexDescriptor GetNullDescriptor() { return VertexDescriptor(); };

	VertexDescriptor() { m_v = NULL; };
	explicit VertexDescriptor(Vertex* v) { m_v = v; };
	VertexDescriptor(const VertexDescriptor& other) { m_v = other.m_v; };
	virtual ~VertexDescriptor() {};

	VertexDescriptor& operator=(const VertexDescriptor& other) { m_v = other.m_v; return *this; };

	operator Vertex*() const { return GetPointerToVertex(); };
	bool operator==(const VertexDescriptor& other) const { return m_v == other.m_v; };
	bool operator!=(const VertexDescriptor& other) const { return m_v != other.m_v; };

	/**
	 * Stream insertion operator.  Inserts @a loc into stream @a os in the following format:
	 * "filepath : line_no(:column_no)?"
	 *
	 * @param os
	 * @param loc
	 * @return Reference to @a os.
	 */
	friend std::ostream& operator<<(std::ostream& os, const VertexDescriptor& vd);

protected:

	virtual Vertex* GetPointerToVertex() const { return m_v; };

	Vertex *m_v;
};


struct VertexDescriptorConv
{
	VertexDescriptor operator()(Vertex* v) const { return VertexDescriptor(v); };

	/// This is for boost::result_of().
	typedef VertexDescriptor result_type;
};


/**
 * Graph base class.
 */
class Graph
{
public:
	/// @name Public member types.
	//@{

	/// The type for the collection of all vertices in the graph.
	typedef std::tr1::unordered_set< Vertex* > vertex_list_type;
	/// The type for the collection of all edges in the graph
	/// @todo FIXME: this isn't actually correct, it's just for the edges of a particular vertex.
	typedef Vertex::edge_list_type edge_list_type;

	//typedef vertex_list_type::const_iterator vertex_iterator;
	typedef boost::transform_iterator<VertexDescriptorConv, vertex_list_type::iterator> vertex_iterator;
	//typedef Vertex* vertex_descriptor;
	typedef VertexDescriptor vertex_descriptor;

	static inline vertex_descriptor null_vertex() { return VertexDescriptor::GetNullDescriptor(); };

	/// @name These are specifically for interoperability with the Boost graph library.
	//@{
	//typedef Edge* edge_descriptor;
	typedef EdgeDescriptor edge_descriptor;
	typedef Vertex::edge_iterator edge_iterator;
	typedef Vertex::out_edge_iterator out_edge_iterator;
	typedef Vertex::in_edge_iterator in_edge_iterator;
	typedef Vertex::degree_size_type degree_size_type;
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
	typedef edge_list_type::size_type edges_size_type;

	/// For vertex_index_t.
	typedef std::size_t vertex_index_type;
    typedef std::size_t edge_index_type;
	typedef boost::property<boost::vertex_index_t, vertex_index_type> VertexProperty;
	typedef VertexProperty vertex_property_type;
	typedef boost::no_property edge_property_type;
	typedef boost::no_property graph_property_type;
	//typedef typename boost::property_map<Graph, size_t StatementBase::*>::type VertexIndexMapType;
	//@}

	//@}

public:
	Graph();
	virtual ~Graph();

	virtual void AddVertex(Vertex *v);
	virtual void RemoveVertex(Vertex *v);
	virtual void ReplaceVertex(Vertex *old_vertex, Vertex *new_vertex);

	virtual void AddEdge(Vertex *source, Vertex *target, Edge *e);
	virtual void RemoveEdge(Edge *e);

	Graph::vertex_index_type GetVertexIndex(Vertex *v) const { return v->GetVertexIndex(); };

	//std::pair<vertex_iterator, vertex_iterator> Vertices();
	virtual void Vertices(std::pair<Graph::vertex_iterator, Graph::vertex_iterator> *iterator_pair) const;
	vertices_size_type NumVertices() const { return m_vertices.size(); };

	Vertex* operator[](const Graph::vertex_descriptor vd) { return vd; };
	Edge* operator[](const Graph::edge_descriptor ed) { return ed; };
	const Vertex* operator[](const Graph::vertex_descriptor vd) const { return vd; };
	const Edge* operator[](const Graph::edge_descriptor ed) const { return ed; };

protected:

	template < typename IteratorType >
	std::pair<IteratorType, IteratorType> GetVertexIteratorPair() const
	{
		IteratorType it_begin, it_end;
		it_begin = m_vertices.begin();
		it_end = m_vertices.end();
		return std::make_pair(it_begin, it_end);
	};

private:

	/// @name Vertex unique ID generator routines.
	//@{

	void InitVertexIDGenerator();
	VertexID GetNewVertexID();
	void AssignAVertexIndexToVertex(Vertex *v);

	//@}


private:

	/// Collection of all vertices in the Graph.
	boost::unordered_set< Vertex* > m_vertices;

	/// The Vertex ID generator state.
	VertexID m_vertex_id_state;
};

extern long dummy_val;

/// @name Free-function declarations for adapting this graph class to the Boost graph library.
//@{
namespace boost
{
	Graph::vertex_descriptor target(const Graph::edge_descriptor &e, const Graph &/*g*/);
	Graph::vertex_descriptor source(const Graph::edge_descriptor &e, const Graph &/*g*/);

	Graph::degree_size_type out_degree(Graph::vertex_descriptor u, const Graph& /*g*/);
	Graph::degree_size_type in_degree(Graph::vertex_descriptor u, const Graph& /*g*/);

	std::pair<Graph::out_edge_iterator, Graph::out_edge_iterator> out_edges(Graph::vertex_descriptor u, const Graph &/*g*/);
	std::pair<Graph::in_edge_iterator, Graph::in_edge_iterator> in_edges(Graph::vertex_descriptor u, const Graph &/*g*/);

	//std::pair<Graph::vertex_iterator, Graph::vertex_iterator> vertices(Graph& g) { return vertices(g); };
	std::pair<Graph::vertex_iterator, Graph::vertex_iterator> vertices(const Graph& g);
	Graph::vertices_size_type num_vertices(const Graph & g);

	/// Vertex index (vertex_index_t) property map.
	class Graph_vertex_index_map
		: public boost::put_get_helper<long, Graph_vertex_index_map>
	{
	public:
		typedef boost::readable_property_map_tag category;
		typedef long value_type;
		typedef long reference;
		typedef Graph::vertex_descriptor key_type;
		Graph_vertex_index_map() /*: m_g(0)*/ { };
		Graph_vertex_index_map(const Graph& /*g*/) /*: m_g(&g)*/ { }
		Graph_vertex_index_map(const Graph_vertex_index_map& /*other*/) {};
		reference operator[](const key_type v) const { return static_cast<Vertex*>(v)->GetVertexIndex(); };
	protected:
		//const Graph* m_g;
	};
	inline Graph_vertex_index_map get(vertex_index_t, Graph& g)
	{
		return Graph_vertex_index_map(g);
	}
	inline Graph_vertex_index_map get(vertex_index_t, const Graph& g)
	{
		return Graph_vertex_index_map(g);
	}
	inline long get(vertex_index_t, const Graph& /*g*/, Graph::vertex_descriptor v)
	{
		return static_cast<Vertex*>(v)->GetVertexIndex();
	}

	/// Property map traits specializations.
	template <>
	struct property_map<Graph, vertex_index_t>
	{
		typedef Graph_vertex_index_map type;
		typedef const Graph_vertex_index_map const_type;
	};
	template <>
	struct property_map<const Graph, vertex_index_t>
	{
		typedef const Graph_vertex_index_map const_type;
	};


	/// @name Free functions for implementing the MutableGraph concept.
	//@{

	/**
	 * Create a new Vertex and add it to Graph @a g.
	 *
	 * @param g The instance of Graph to add the new Vertex to.
	 * @return Descriptor of the new vertex.
	 */
	boost::graph_traits<Graph>::vertex_descriptor add_vertex(Graph& g);
	std::pair<Graph::edge_descriptor, bool> add_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g);
	void remove_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g);
	void remove_edge(Graph::edge_descriptor e, Graph &g);
	void clear_vertex(Graph::vertex_descriptor u, Graph &g);
	void remove_vertex(Graph::vertex_descriptor u, Graph &g);

	//@}


}
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
	BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyMapConcept<const boost::Graph_vertex_index_map, const Graph::vertex_descriptor> ));
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
	// Is our Graph class a ReadablePropertyGraph for vertex_index_t?
	BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyGraphConcept<Graph, Graph::vertex_descriptor, boost::vertex_index_t> ));
	// Is our Graph class a PropertyGraph for vertex_color_t?
	//BOOST_CONCEPT_ASSERT(( boost::PropertyGraphConcept<Graph, Graph::vertex_descriptor, boost::vertex_color_t> ));
	//BOOST_CONCEPT_ASSERT(( boost::VertexIndexGraphConcept<Graph> ))
	BOOST_CONCEPT_ASSERT(( boost::MutableGraphConcept<Graph> ));
}

#endif /* GRAPH_H_ */
