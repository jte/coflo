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
#include <utility>

class Vertex;
class Edge;

/**
 * Graph base class.
 */
class Graph
{
public:
	/// @name Public member types.
	//@{
	typedef boost::unordered_set< Vertex* >::const_iterator vertex_iterator;
	typedef Vertex* vertex_descriptor;
	typedef boost::unordered_set< Edge* >::const_iterator out_edge_iterator;
	//@}

public:
	Graph();
	virtual ~Graph();

	virtual void AddVertex(Vertex *v);
	virtual void RemoveVertex(Vertex *v);

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

#endif /* GRAPH_H_ */
