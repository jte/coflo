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

#include "gtest/gtest.h"

#include <boost/tr1/random.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/graph_concepts.hpp>

#include "Graph.h"
#include "GraphAdapter.h"

int PullInMyLibrary() { return 0; }

/**
 * CppUnit test fixture for the Graph class.
 */
class GraphTest : public ::testing::Test
{
protected:
	GraphTest() {};
	virtual ~GraphTest() {};

	virtual void SetUp() {};
	virtual void TearDown() {};

	/**
	 * Checks that Graph @a g is consistent.
	 *
	 * @param g
	 */
	void CheckGraphForConsistency(Graph *g);

	Graph *m_test_graph;
	Vertex *m_test_vert1, *m_test_vert2;
};

void GraphTest::CheckGraphForConsistency(Graph *g)
{

}

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
	EXPECT_NE(v1->GetIndex(), v2->GetIndex());

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
	EXPECT_NE(v1->GetIndex(), v2->GetIndex());

	EXPECT_EQ(g->NumVertices(), 2);

	e1 = new Edge();
	ASSERT_TRUE(e1 != NULL);

	// Add the edge between the vertices.
	ASSERT_NO_THROW( g->AddEdge(v1, v2, e1) );

	// check if the edge is connected correctly.
	ASSERT_EQ(source(e1, *g), v1);
	ASSERT_EQ(target(e1, *g), v2);
	ASSERT_EQ(in_degree(v1, *g), 0);
	ASSERT_EQ(out_degree(v1, *g), 1);
	ASSERT_EQ(in_degree(v2, *g), 1);
	ASSERT_EQ(out_degree(v2, *g), 0);

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

TEST_F(GraphTest, CreateRandomGraphWithBoost_generate_random_graph)
{
	Graph *g;

	// Create a new Graph.
	ASSERT_NO_THROW( g = new Graph() );
	ASSERT_TRUE( g != NULL );

	// Generate a random graph with no parallel edges and no self edges.
	std::tr1::mt19937 rng;
	ASSERT_NO_THROW( boost::generate_random_graph(*g, 10, 20, rng, false, false) );

	// Make sure we have the number of vertices and edges we asked for.
	ASSERT_EQ(num_vertices(*g), 10);
	ASSERT_EQ(num_edges(*g), 20);

	// Try to serialize the vertices of the graph "by hand".
	Graph::vertex_iterator i, j;
	boost::tie(i,j) = vertices(*g);
	std::cout << "Vertex Index, In Degree, Out Degree" << std::endl;
	for(; i != j; ++i)
	{
		std::cout << "Vertex Index: " << (*i)->GetIndex() << std::endl;
		std::cout << "In Degree:    " << in_degree(*i, *g) << std::endl;
		std::cout << "Out Degree:   " << out_degree(*i, *g) << std::endl;
		std::cout << "  Source vertices:";
		if(out_degree(*i, *g) > 0)
		{
			Graph::in_edge_iterator ei, eend;
			boost::tie(ei, eend) = in_edges(*i, *g);
			for(; ei != eend; ei++)
			{
				std::cout << " " << source(*ei, *g)->GetIndex();
			}
		}
		else
		{
			std::cout << "none.";
		}
		std::cout << std::endl;
		std::cout << "  Target vertices:";
		if(out_degree(*i, *g) > 0)
		{
			Graph::out_edge_iterator ei, eend;
			boost::tie(ei, eend) = out_edges(*i, *g);
			for(; ei != eend; ei++)
			{
				std::cout << " "  << target(*ei, *g)->GetIndex();
			}
		}
		else
		{
			std::cout << "none.";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}

	delete g;
}
