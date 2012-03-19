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

#include "Graph.h"

#include "Vertex.h"
#include "Edge.h"

Graph::Graph()
{
	// TODO Auto-generated constructor stub

}

Graph::~Graph()
{
	// TODO Auto-generated destructor stub
}

void Graph::AddVertex(Vertex* v)
{
	m_vertices.insert(v);
}

void Graph::RemoveVertex(Vertex* v)
{
	m_vertices.erase(v);
}

void Graph::AddEdge(Vertex *source, Vertex *target, Edge* e)
{
	source->AddOutEdge(e);
	target->AddInEdge(e);
}

void Graph::RemoveEdge(Edge* e)
{
}

std::pair<Graph::Vertex_iterator, Graph::Vertex_iterator> Graph::Vertices()
{
	return std::pair< Vertex_iterator, Vertex_iterator >(m_vertices.begin(), m_vertices.end());
}


