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

Vertex::Vertex()
{
	// TODO Auto-generated constructor stub

}

Vertex::~Vertex()
{
	// TODO Auto-generated destructor stub
}

void Vertex::AddInEdge(Edge* e)
{
	m_in_edges.insert(e);
}

void Vertex::AddOutEdge(Edge* e)
{
	m_out_edges.insert(e);
}

std::pair<Vertex::In_Edge_iterator, Vertex::In_Edge_iterator> Vertex::InEdges()
{
	return std::pair<In_Edge_iterator, In_Edge_iterator>(m_in_edges.begin(), m_in_edges.end());
}

std::pair<Vertex::Out_Edge_iterator, Vertex::Out_Edge_iterator> Vertex::OutEdges()
{
	return std::pair<Out_Edge_iterator, Out_Edge_iterator>(m_out_edges.begin(), m_out_edges.end());
}


