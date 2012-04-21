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

#include "VertexID.h"

//class Edge;
#include "Edge.h"

/**
 *
 */
class Vertex
{
public:
	typedef boost::unordered_set< Edge* > edge_list_type;
	typedef edge_list_type::const_iterator edge_iterator;
	typedef edge_iterator out_edge_iterator;
	typedef edge_iterator in_edge_iterator;
	typedef boost::unordered_set< Edge* >::size_type degree_size_type;
	struct out_edge_iterator_pair_t { out_edge_iterator first; out_edge_iterator second; };


public:
	Vertex();
	virtual ~Vertex();

	void CopyFrom(Vertex *other);

	void TransferOwnedResourcesTo(Vertex *other);

	/**
	 * Return this Vetex's ID.  The ID is guaranteed to be unique to the containing Graph.
	 *
	 * @return
	 */
	VertexID GetID() const;
	long GetVertexIndex() const { return 0; /** @todo FIXME */};

	void AddInEdge(Edge *e);
	void RemoveInEdge(Edge *e);
	void AddOutEdge(Edge *e);
	void RemoveOutEdge(Edge *e);

	std::pair<Vertex::in_edge_iterator, Vertex::in_edge_iterator> InEdges();
	std::pair<Vertex::out_edge_iterator, Vertex::out_edge_iterator> OutEdges();

	degree_size_type InDegree() { return m_in_edges.size(); };
	degree_size_type OutDegree() { return m_out_edges.size(); };

private:
	typedef edge_list_type T_EDGE_PTR_CONTAINER;
	T_EDGE_PTR_CONTAINER m_out_edges;
	T_EDGE_PTR_CONTAINER m_in_edges;

};

#endif /* VERTEX_H_ */
