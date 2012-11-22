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

#include "Vertex.h"

#include <algorithm>
#include <boost/tr1/functional.hpp>
#include <boost/foreach.hpp>

#include "coflo_exceptions.hpp"


Vertex::Vertex()
{
	// TODO Auto-generated constructor stub

}

Vertex::Vertex(const Vertex& other)
{
	BOOST_THROW_EXCEPTION( not_implemented() );
}

Vertex::~Vertex()
{
	// TODO Auto-generated destructor stub
}

void Vertex::CopyFrom(Vertex* other)
{
	BOOST_THROW_EXCEPTION( not_implemented() );
}

void Vertex::TransferOwnedResourcesTo(Vertex* other)
{
	// Point the in edges of this Vertex to the other Vertex.
	BOOST_FOREACH(Edge *i, m_in_edges)
	{
		i->SetTarget(other);
	}

	// Point the out edges of this Vertex to the other Vertex.
	BOOST_FOREACH(Edge *i, m_out_edges)
	{
		i->SetSource(other);
	}

	// Swap edge lists between the two vertices.
	m_in_edges.swap(other->m_in_edges);
	m_out_edges.swap(other->m_out_edges);
}

void Vertex::AddInEdge(Edge* e)
{
	m_in_edges.insert(e);
}

void Vertex::AddOutEdge(Edge* e)
{
	m_out_edges.insert(e);
}

void Vertex::RemoveInEdge(Edge* e)
{
	m_in_edges.erase(e);
}

void Vertex::RemoveOutEdge(Edge* e)
{
	m_out_edges.erase(e);
}

std::pair<Vertex::in_edge_iterator, Vertex::in_edge_iterator> Vertex::InEdges()
{
	return std::pair<Vertex::in_edge_iterator, Vertex::in_edge_iterator>(MakeIterator(m_in_edges.begin()), MakeIterator(m_in_edges.end()));
}

std::pair<Vertex::out_edge_iterator, Vertex::out_edge_iterator> Vertex::OutEdges()
{
	return std::pair<Vertex::out_edge_iterator, Vertex::out_edge_iterator>(MakeIterator(m_out_edges.begin()), MakeIterator(m_out_edges.end()));
}

void Vertex::SetVertexIndex(VertexID vertex_index)
{
	m_vertex_index = vertex_index;
}

Vertex::edge_iterator Vertex::MakeIterator(Vertex::base_edge_list_iterator i)
{
	return boost::make_transform_iterator< EdgeDescriptorConv, base_edge_list_iterator>(i);
}

struct EdgePointsTo : public std::unary_function<const Edge*,bool>
{
	EdgePointsTo(const Vertex* other) { m_other = other; };

	bool operator()(const Edge* e) { return e->Target() == m_other; };

	const Vertex *m_other;
};

Edge* Vertex::FindOutEdgePointingToVertex(const Vertex* target)
{
	edge_list_type::iterator the_edge;

	the_edge = std::find_if(m_out_edges.begin(), m_out_edges.end(), EdgePointsTo(target));

	if(the_edge == m_out_edges.end())
	{
		// Couldn't find it.
		return NULL;
	}

	return *the_edge;
}





