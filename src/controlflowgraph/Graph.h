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

/// The vertex descriptor for Graphs is simply a Vertex pointer.
typedef Vertex* VertexDescriptor;

struct VertexDescriptorConv
{
	VertexDescriptor operator()(Vertex* v) const { return VertexDescriptor(v); };

	/// This is for boost::result_of().
	typedef VertexDescriptor result_type;
};

struct Graph_traversal_tag :
    public virtual boost::incidence_graph_tag,
    public virtual boost::bidirectional_graph_tag,
    public virtual boost::vertex_list_graph_tag,
    public virtual boost::edge_list_graph_tag
    { };

/**
 * Graph base class.
 */
class Graph
{
public:
	/// @name Public member typenames.
	//@{
		/// The type for the collection of all vertices in the graph.
		typedef std::tr1::unordered_set< Vertex* > vertex_list_type;
		/// The type for the collection of all edges in the graph.
		/// @note The Vertex class has its own methods of managing edges incident on the vertex which are not necessarily
		/// the same as those of the Graph class.  In particular, edge_iterators are not interchangeable with in_edge_iterators
		/// or out_edge_iterators.
		typedef std::tr1::unordered_set< Edge* > edge_list_type;

		/// @name Member types for the Graph concept.
		/// @note Contrary to Boost 1.49 documentation, what is listed here are the complete requirements for GraphConcept.
		/// E.g., no null_vertex(), no edge_descriptor.
		//@{
			typedef VertexDescriptor vertex_descriptor;
			/// Our "directionality" will be bidirectional.
			/// @note directed_category must be convertible to either directed_tag or undirected_tag.  bidirectional_tag inherits from
			/// directed_category.
			typedef boost::bidirectional_tag directed_category;
			/// Allow parallel edges.
			typedef boost::allow_parallel_edge_tag edge_parallel_category;
			typedef Graph_traversal_tag traversal_category;
		//@}

		/// @name Member types for the IncidenceGraph concept.
		/// BidirectionalGraphConcept inherits from this concept.
		//@{
			typedef EdgeDescriptor edge_descriptor;
			typedef Vertex::out_edge_iterator out_edge_iterator;
			typedef Vertex::degree_size_type degree_size_type;
			/**
			 *  Operations:
			 * - out_edges(u, g)
			 * - out_degree(u, g)
			 * - source(e, g)
			 * - target(e, g)
			 */
		//@}

		/// @name Member types for the BidirectionalGraph concept.
		//@{
			typedef Vertex::in_edge_iterator in_edge_iterator;
			/**
			 * Operations:
			 * - in_edges()
			 * - in_degree()
			 */
		//@}

		/// @name Member types for the VertexListGraph concept, which inherits from GraphConcept.
		//@{
			typedef boost::transform_iterator<VertexDescriptorConv,
				vertex_list_type::iterator,
				VertexDescriptor,
				VertexDescriptor> vertex_iterator;
			typedef vertex_list_type::size_type vertices_size_type;
			/**
			 * Operations:
			 * - vertices()
			 * - num_vertices();
			 */
		//@}

		/// @name Member types for the AdjacencyGraph concept, which inherits from GraphConcept.
		//@{
			/// @note boost::graph_traits<> appears to always need this definition, even if you don't model it.
			typedef void adjacency_iterator;
			/**
			 * Operations:
			 * - adjacent_vertices()
			 */
		//@}

		/// @name Member types for the EdgeListGraph concept (efficient traversal of all edges in graph),
		/// which inherits from GraphConcept.
		//@{
			// BidirectionalGraphConcept already has this typedef covered.
			//typedef EdgeDescriptor edge_descriptor;
			typedef edge_list_type::const_iterator edge_iterator;
			typedef edge_list_type::size_type edges_size_type;
			/**
			 * Operations:
			 * - edges()
			 * - source()
			 * - target()
			 */
		//@}

		/// @name Member types for the VertexAndEdgeListGraph concept.
		/// Inherits from both VertexListGraph and EdgeListGraph.
		//@{
			/// No new members.
		//@}


		/// Currently no concept checks for this?  It is in graph_traits<> though.
		static vertex_descriptor null_vertex() { return NULL; };

		/**
		 * Return a null edge descriptor.
		 * @note Not something used by Boost Graph Library.
		 * @return
		 */
		static edge_descriptor null_edge() { return NULL; };


		/// @name Various proprties.
		//@{
			typedef std::size_t vertex_index_type;
			typedef std::size_t edge_index_type;
			typedef boost::property<boost::vertex_index_t, vertex_index_type> VertexProperty;
			typedef boost::property<boost::vertex_index_t, edge_index_type> EdgeProperty;
			typedef VertexProperty vertex_property_type;
			typedef EdgeProperty edge_property_type;
			typedef boost::no_property graph_property_type;
			typedef void vertex_bundled;
			typedef void edge_bundled;
		//@}

		/// Mutability
		typedef boost::mutable_property_graph_tag mutability_category;
	//@}

public:
	Graph();
	virtual ~Graph();

	/**
	 * Adds the Vertex pointed to by @a v to the Graph.  Graph does not own @a v.
	 *
	 * @param v Pointer to the vertex to add.
	 */
	virtual void AddVertex(Vertex *v);

	/**
	 * Removes the Vertex pointed to by @a v from this Graph.
	 *
	 * @param v Pointer to the vertex to remove from the graph.
	 */
	virtual void RemoveVertex(Vertex *v);
	virtual void ReplaceVertex(Vertex *old_vertex, Vertex *new_vertex);

	/**
	 * Adds Edge @a e to the graph.
	 *
	 * @pre Edge @a e has no source or target.
	 * @post Edge @a e has source = @a source, target = @a target.
	 *
	 * @param source
	 * @param target
	 * @param e
	 */
	virtual void AddEdge(Vertex *source, Vertex *target, Edge *e);
	virtual void RemoveEdge(Edge *e);

	Graph::vertex_index_type GetIndex(Vertex *v) const { return v->GetIndex(); };
	Graph::edge_index_type GetIndex(Edge *e) const { return e->GetIndex(); };

	//std::pair<vertex_iterator, vertex_iterator> Vertices();
	virtual void Vertices(std::pair<Graph::vertex_iterator, Graph::vertex_iterator> *iterator_pair) const;
	vertices_size_type NumVertices() const { return m_vertices.size(); };

	Graph::edges_size_type NumEdges() const { return m_edges.size(); };
	Graph::edge_iterator EdgeListBegin() const;
	Graph::edge_iterator EdgeListEnd() const;

	Vertex* operator[](const Graph::vertex_descriptor vd) { return vd; };
	Edge* operator[](const Graph::edge_descriptor ed) { return ed; };
	const Vertex* operator[](const Graph::vertex_descriptor vd) const { return vd; };
	const Edge* operator[](const Graph::edge_descriptor ed) const { return ed; };

	/// @name Find functions.
	//@{

	Graph::edge_descriptor FindEdge(const Graph::vertex_descriptor source, const Graph::vertex_descriptor target);

	//@}

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

	/// Collection of all Edges in the Graph.
	edge_list_type m_edges;

	/// The Vertex ID generator state.
	VertexID m_vertex_id_state;
};




#endif /* GRAPH_H_ */
