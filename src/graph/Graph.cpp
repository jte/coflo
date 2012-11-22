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

#include <boost/iterator/transform_iterator.hpp>
#include <boost/tuple/tuple.hpp>

#include "Vertex.h"
#include "Edge.h"
#include <coflo_exceptions.hpp>

Graph::Graph()
{
	// Initialize the vertex_index generator.
	InitVertexIDGenerator();
}

Graph::~Graph()
{
	// TODO Auto-generated destructor stub
}

void Graph::AddVertex(Vertex* v)
{
	AssignAVertexIndexToVertex(v);
	bool inserted;
	boost::tie(boost::tuples::ignore, inserted) = m_vertices.insert(v);
	if(inserted != true)
	{
		// The vertex was not successfully inserted.  This means that either it is now not in the Graph,
		// or that it was already in the Graph and the caller attempted to add it again.
		// Either way, we'll throw an exception.
		BOOST_THROW_EXCEPTION( duplicate_add() );
	}
}

void Graph::RemoveVertex(Vertex* v)
{
	m_vertices.erase(v);
}

void Graph::ReplaceVertex(Vertex* old_vertex, Vertex* new_vertex)
{
	// Add the new vertex to the graph.
	AddVertex(new_vertex);

	// Move the edges from the old vertex to the new one.
	old_vertex->TransferOwnedResourcesTo(new_vertex);

	// Remove the old vertex from the graph.
	RemoveVertex(old_vertex);
}

void Graph::AddEdge(Vertex *source, Vertex *target, Edge* e)
{
	source->AddOutEdge(e);
	target->AddInEdge(e);
	e->SetSourceAndTarget(source, target);
	m_edges.insert(e);
}

void Graph::RemoveEdge(Edge* e)
{
	e->Source()->RemoveOutEdge(e);
	e->Target()->RemoveInEdge(e);
	e->ClearSourceAndTarget();
	m_edges.erase(e);
}

void Graph::Vertices(std::pair<Graph::vertex_iterator, Graph::vertex_iterator> *iterator_pair) const
{
	iterator_pair->first = boost::make_transform_iterator< VertexDescriptorConv, vertex_list_type::iterator>(m_vertices.begin());
	iterator_pair->second = boost::make_transform_iterator< VertexDescriptorConv, vertex_list_type::iterator>(m_vertices.end());
}

void Graph::InitVertexIDGenerator()
{
	m_vertex_id_state = 0;
}

VertexID Graph::GetNewVertexID()
{
	VertexID retval = m_vertex_id_state;
	m_vertex_id_state++;

	return retval;
}

Graph::edge_iterator Graph::EdgeListBegin() const
{
	return m_edges.begin();
}

Graph::edge_iterator Graph::EdgeListEnd() const
{
	return m_edges.end();
}

void Graph::AssignAVertexIndexToVertex(Vertex* v)
{
	VertexID vertex_index;

	vertex_index = GetNewVertexID();

	v->SetVertexIndex(vertex_index);
}

Graph::edge_descriptor Graph::FindEdge(const Graph::vertex_descriptor source, const Graph::vertex_descriptor target)
{
	Graph::edge_descriptor e;

	e = source->FindOutEdgePointingToVertex(target);

	if(e == NULL)
	{
		return null_edge();
	}
	else
	{
		return e;
	}
}

