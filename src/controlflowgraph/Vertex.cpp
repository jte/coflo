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

#include "coflo_exceptions.hpp"

#include <boost/foreach.hpp>

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
	BOOST_FOREACH(Edge* i, m_in_edges)
	{
		i->SetTarget(other);
	}
	BOOST_FOREACH(Edge* i, m_out_edges)
	{
		i->SetSource(other);
	}

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
	return std::pair<in_edge_iterator, in_edge_iterator>(m_in_edges.begin(), m_in_edges.end());
}

std::pair<Vertex::out_edge_iterator, Vertex::out_edge_iterator> Vertex::OutEdges()
{
	return std::pair<out_edge_iterator, out_edge_iterator>(m_out_edges.begin(), m_out_edges.end());
}

void Vertex::SetVertexIndex(VertexID vertex_index)
{
	m_vertex_index = vertex_index;
}



