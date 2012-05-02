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
#include <boost/graph/graph_mutability_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <utility>

//class Vertex;
// Including header to get access to member typedefs.
#include "Vertex.h"
#include "Edge.h"
//class Edge;
#include "DescriptorBaseClass.h"

//typedef DescriptorBaseClass<Vertex> VertexDescriptor;
typedef Vertex* VertexDescriptor;

struct VertexDescriptorConv
{
	VertexDescriptor operator()(Vertex* v) const { return VertexDescriptor(v); };

	/// This is for boost::result_of().
	typedef VertexDescriptor result_type;
};

struct Graph_traversal_tag :
    public virtual boost::vertex_list_graph_tag,
    public virtual boost::incidence_graph_tag,
    public virtual boost::adjacency_graph_tag,
    public virtual boost::bidirectional_graph_tag { };

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

	//typedef Vertex* vertex_descriptor;
	typedef VertexDescriptor vertex_descriptor;

	//typedef vertex_list_type::const_iterator vertex_iterator;
	typedef boost::transform_iterator<VertexDescriptorConv,
			vertex_list_type::iterator,
			VertexDescriptor,
			VertexDescriptor> vertex_iterator;


	static vertex_descriptor null_vertex() { return NULL;/*VertexDescriptor::GetNullDescriptor();*/ };

	/// @name These are specifically for interoperability with the Boost graph library.
	//@{
	typedef EdgeDescriptor edge_descriptor;
	typedef Vertex::edge_iterator edge_iterator;
	typedef Vertex::out_edge_iterator out_edge_iterator;
	typedef Vertex::in_edge_iterator in_edge_iterator;
	typedef Vertex::degree_size_type degree_size_type;

	typedef boost::directed_tag directed_category;
	typedef boost::allow_parallel_edge_tag edge_parallel_category;
	typedef Graph_traversal_tag traversal_category;
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
	typedef typename boost::graph_detail::edge_prop<boost::no_property>::property edge_property_type;
	typedef typename boost::graph_detail::graph_prop<boost::no_property>::property graph_property_type;
	//typedef typename boost::property_map<Graph, size_t StatementBase::*>::type VertexIndexMapType;
	//@}

	/// Mutability
	typedef boost::mutable_property_graph_tag mutability_category;

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

private:

	/// @name Vertex unique ID generator routines.
	//@{

	void InitVertexIDGenerator();
	VertexID GetNewVertexID();
	void AssignAVertexIndexToVertex(Vertex *v);

	//@}


protected:

	/// Collection of all vertices in the Graph.
	vertex_list_type m_vertices;

	/// The Vertex ID generator state.
	VertexID m_vertex_id_state;
};




#endif /* GRAPH_H_ */
