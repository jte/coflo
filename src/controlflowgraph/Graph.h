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

#include <boost/unordered_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_archetypes.hpp>
#include <utility>

//class Vertex;
// Including header to get access to member typedefs.
#include "Vertex.h"
#include "Edge.h"
//class Edge;

/**
 * Graph base class.
 */
class Graph
{
public:
	/// @name Public member types.
	//@{

	/// The type for the collection of all vertices in the graph.
	typedef boost::unordered_set< Vertex* > vertex_list_type;
	typedef Vertex::edge_list_type edge_list_type;

	typedef boost::unordered_set< Vertex* >::const_iterator vertex_iterator;
	typedef Vertex* vertex_descriptor;
	typedef boost::unordered_set< Edge* >::const_iterator out_edge_iterator;

	/// @name These are specifically for interoperability with the Boost graph library.
	//@{
	typedef Edge* edge_descriptor;
	typedef Vertex::edge_iterator edge_iterator;
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
	typedef edge_list_type edges_size_type;
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

	//std::pair<vertex_iterator, vertex_iterator> Vertices();
	virtual void Vertices(std::pair<Graph::vertex_iterator, Graph::vertex_iterator> *iterator_pair);

protected:

	template < typename IteratorType >
	std::pair<IteratorType, IteratorType> GetVertexIteratorPair()
	{
		IteratorType it_begin, it_end;
		it_begin = m_vertices.begin();
		it_end = m_vertices.end();
		return std::make_pair(it_begin, it_end);
	};

private:

	/// Collection of all vertices in the Graph.
	boost::unordered_set< Vertex* > m_vertices;
};

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
}
//@}

#endif /* GRAPH_H_ */
