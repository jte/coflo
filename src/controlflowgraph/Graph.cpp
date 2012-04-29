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
	m_vertices.insert(v);
}

void Graph::RemoveVertex(Vertex* v)
{
	m_vertices.erase(v);
}

void Graph::ReplaceVertex(Vertex* old_vertex, Vertex* new_vertex)
{
	AddVertex(new_vertex);
	old_vertex->TransferOwnedResourcesTo(new_vertex);
	RemoveVertex(old_vertex);
	delete old_vertex;
}

void Graph::AddEdge(Vertex *source, Vertex *target, Edge* e)
{
	source->AddOutEdge(e);
	target->AddInEdge(e);
	e->SetSourceAndTarget(source, target);
}

void Graph::RemoveEdge(Edge* e)
{
	e->Source()->RemoveOutEdge(e);
	e->Target()->RemoveInEdge(e);
	e->ClearSourceAndTarget();
}

void Graph::Vertices(std::pair<Graph::vertex_iterator, Graph::vertex_iterator> *iterator_pair) const
{
	std::pair<Graph::vertex_iterator, Graph::vertex_iterator> it_pair;

	it_pair = GetVertexIteratorPair<Graph::vertex_iterator>();

	iterator_pair->first = it_pair.first; //m_vertices.begin();
	iterator_pair->second = it_pair.second; //m_vertices.end();
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

void Graph::AssignAVertexIndexToVertex(Vertex* v)
{
	VertexID vertex_index;

	vertex_index = GetNewVertexID();

	v->SetVertexIndex(vertex_index);
}

/// @name Free-function definitions for adapting this graph class to the Boost graph library.
//@{
namespace boost
{
	Graph::vertex_descriptor target(const Graph::edge_descriptor &e, const Graph &/*g*/) { return e->Target(); };
	Graph::vertex_descriptor source(const Graph::edge_descriptor &e, const Graph &/*g*/) { return e->Source(); };

	Graph::degree_size_type out_degree(Graph::vertex_descriptor u, const Graph& /*g*/) { return u->OutDegree(); };
	Graph::degree_size_type in_degree(Graph::vertex_descriptor u, const Graph& /*g*/) { return u->InDegree(); };

	std::pair<Graph::out_edge_iterator, Graph::out_edge_iterator> out_edges(Graph::vertex_descriptor u, const Graph &/*g*/) { return u->OutEdges(); };
	std::pair<Graph::in_edge_iterator, Graph::in_edge_iterator> in_edges(Graph::vertex_descriptor u, const Graph &/*g*/) { return u->InEdges(); };

	std::pair<Graph::vertex_iterator, Graph::vertex_iterator> vertices(const Graph& g)
	{
		std::pair<Graph::vertex_iterator, Graph::vertex_iterator> retval;

		g.Vertices(&retval);

		return retval;
	}

	Graph::vertices_size_type num_vertices(const Graph& g)
	{
		return g.NumVertices();
	}

	/// @name Free function definitions for implementing the MutableGraph concept.
	//@{

	boost::graph_traits<Graph>::vertex_descriptor add_vertex(Graph& g)
	{
		/// @todo We probably need to use the "virtual constructor" pattern here.
		Vertex *v = new Vertex();

		g.AddVertex(v);

		return v;
	}

	std::pair<Graph::edge_descriptor, bool> add_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g)
	{
		Edge *e = new Edge();

		g.AddEdge(u, v, e);

		/// @todo From the BGL docs: "If the graph disallows parallel edges, and the edge (u,v) is already in the graph,
		/// then the bool flag returned is false and the returned edge descriptor points to the already existing edge.".

		return std::make_pair(e, true);
	}

	void remove_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g)
	{

	}

	void remove_edge(Graph::edge_descriptor e, Graph &g)
	{

	}

	void clear_vertex(Graph::vertex_descriptor u, Graph &g)
	{

	}

	void remove_vertex(Graph::vertex_descriptor u, Graph &g)
	{

	}

	//@}
}
//@}

std::ostream& operator<<(std::ostream& os, const VertexDescriptor& vd)
{
	// Stream out the pointer.
    os << vd.m_v;
    return os;
}

