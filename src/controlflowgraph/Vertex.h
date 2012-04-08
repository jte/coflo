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

#ifndef VERTEX_H_
#define VERTEX_H_

#include <boost/unordered_set.hpp>
#include <boost/any.hpp>
#include <utility>

//class Edge;
#include "Edge.h"

/*
 *
 */
class Vertex
{
public:
	typedef boost::unordered_set< Edge* >::const_iterator Edge_iterator;
	typedef Edge_iterator Out_Edge_iterator;
	typedef Edge_iterator In_Edge_iterator;
	typedef boost::unordered_set< Edge* >::size_type degree_size_t;
	struct out_edge_iterator_pair_t { Out_Edge_iterator first; Out_Edge_iterator second; };


public:
	Vertex();
	virtual ~Vertex();

	void AddInEdge(Edge *e);
	void AddOutEdge(Edge *e);

	std::pair<Vertex::In_Edge_iterator, Vertex::In_Edge_iterator> InEdges();
	std::pair<Vertex::Out_Edge_iterator, Vertex::Out_Edge_iterator> OutEdges();

	degree_size_t InDegree() { return m_in_edges.size(); };
	degree_size_t OutDegree() { return m_out_edges.size(); };

private:

	boost::unordered_set< Edge* > m_out_edges;
	boost::unordered_set< Edge* > m_in_edges;

};

#endif /* VERTEX_H_ */
