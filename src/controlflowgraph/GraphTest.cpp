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

#include "GraphTest.h"

#include "Graph.h"


int PullInMyLibrary() { return 0; }


TEST_F(GraphTest, GraphNewDelete)
{
	Graph *g;

	g = new Graph();
	delete g;
}

TEST_F(GraphTest, AddVertsAndEdge)
{
	Graph *g;
	Vertex *v1, *v2;
	Edge *e1;

	// Create a new Graph.
	g = new Graph();
	ASSERT_TRUE( g != NULL );

	// Create two Vertexes.
	v1 = new Vertex();
	ASSERT_TRUE( v1 != NULL);
	v2 = new Vertex();
	ASSERT_TRUE( v2 != NULL);

	// Add the vertices to the graph.
	g->AddVertex(v1);
	g->AddVertex(v2);

	// Check to be sure the vertices have different vertex indices.
	EXPECT_NE(v1->GetVertexIndex(), v2->GetVertexIndex());

	EXPECT_EQ(g->NumVertices(), 2);

	e1 = new Edge();
	ASSERT_TRUE(e1 != NULL);

	// Add the edge between the vertices.
	ASSERT_NO_THROW( g->AddEdge(v1, v2, e1) );

	// check if the edge is connected correctly.
	ASSERT_EQ(e1->Source(), v1);
	ASSERT_EQ(e1->Target(), v2);
	ASSERT_EQ(v1->InDegree(), 0);
	ASSERT_EQ(v1->OutDegree(), 1);
	ASSERT_EQ(v2->InDegree(), 1);
	ASSERT_EQ(v2->OutDegree(), 0);

	// Remove the edge from the graph.
	ASSERT_NO_THROW( g->RemoveEdge(e1) );

	// Delete the edge.
	delete e1;

	// Remove the vertices from the graph.
	g->RemoveVertex(v2);
	g->RemoveVertex(v1);

	// Delete the vertices.
	delete v2;
	delete v1;

	// Delete the graph.
	delete g;
}

TEST_F(GraphTest, AddVertsAndEdgeBoost)
{
	Graph *g;
	Vertex *v1, *v2;
	Edge *e1;

	// Create a new Graph.
	g = new Graph();
	ASSERT_TRUE( g != NULL );

	// Create two Vertexes.
	v1 = new Vertex();
	ASSERT_TRUE( v1 != NULL);
	v2 = new Vertex();
	ASSERT_TRUE( v2 != NULL);

	// Add the vertices to the graph.
	g->AddVertex(v1);
	g->AddVertex(v2);

	// Check to be sure the vertices have different vertex indices.
	EXPECT_NE(v1->GetVertexIndex(), v2->GetVertexIndex());

	EXPECT_EQ(g->NumVertices(), 2);

	e1 = new Edge();
	ASSERT_TRUE(e1 != NULL);

	// Add the edge between the vertices.
	ASSERT_NO_THROW( g->AddEdge(v1, v2, e1) );

	// check if the edge is connected correctly.
	ASSERT_EQ(boost::source(e1, *g), v1);
	ASSERT_EQ(boost::target(e1, *g), v2);
	ASSERT_EQ(boost::in_degree(v1, *g), 0);
	ASSERT_EQ(boost::out_degree(v1, *g), 1);
	ASSERT_EQ(boost::in_degree(v2, *g), 1);
	ASSERT_EQ(boost::out_degree(v2, *g), 0);

	// Remove the edge from the graph.
	ASSERT_NO_THROW( g->RemoveEdge(e1) );

	// Delete the edge.
	delete e1;

	// Remove the vertices from the graph.
	g->RemoveVertex(v2);
	g->RemoveVertex(v1);

	// Delete the vertices.
	delete v2;
	delete v1;

	// Delete the graph.
	delete g;
}

