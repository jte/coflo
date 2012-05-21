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

#include <boost/graph/graphviz.hpp>

#include "GraphAdapter.h"
#include "ControlFlowGraph.h"
#include "statements/Goto.h"
#include "statements/Label.h"
#include "edges/CFGEdgeTypeGoto.h"
#include "edges/CFGEdgeTypeFallthrough.h"


int GetMeToo() {return 5; };

/**
 * Test fixture for the ControlFlowGraph class.
 */
class ControlFlowGraphTest : public ::testing::Test
{
protected:
	ControlFlowGraphTest() {};
	virtual ~ControlFlowGraphTest() {};

	virtual void SetUp() {};
	virtual void TearDown() {};

	ControlFlowGraph* CreateSimplestCFG();

	ControlFlowGraph *m_test_graph;
	StatementBase *m_test_vert1, *m_test_vert2;
};


TEST_F(ControlFlowGraphTest, GraphNewDelete)
{
	ControlFlowGraph *g;

	g = new ControlFlowGraph();
	delete g;
}

TEST_F(ControlFlowGraphTest, AddVertsAndEdge)
{
	ControlFlowGraph *g;
	StatementBase *v1, *v2;
	CFGEdgeTypeBase *e1;

	// Create a new Graph.
	g = new ControlFlowGraph();
	ASSERT_TRUE( g != NULL );

	// Create two Vertexes.
	v1 = new Goto(Location());
	ASSERT_TRUE( v1 != NULL);
	v2 = new Label(Location(), "nowhere");
	ASSERT_TRUE( v2 != NULL);

	// Add the vertices to the graph.
	g->AddVertex(v1);
	g->AddVertex(v2);

	// Check to be sure the vertices have different vertex indices.
	EXPECT_NE(v1->GetIndex(), v2->GetIndex());

	EXPECT_EQ(g->NumVertices(), 2);

	e1 = new CFGEdgeTypeGoto();
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

TEST_F(ControlFlowGraphTest, AddVertsAndEdgeBoost)
{
	ControlFlowGraph *g;
	StatementBase *v1, *v2;
	CFGEdgeTypeBase *e1;

	// Create a new Graph.
	g = new ControlFlowGraph();
	ASSERT_TRUE( g != NULL );

	// Create two Vertexes.
	v1 = new Goto(Location());
	ASSERT_TRUE( v1 != NULL);
	v2 = new Label(Location(), "nowhere");
	ASSERT_TRUE( v2 != NULL);

	// Add the vertices to the graph.
	g->AddVertex(v1);
	g->AddVertex(v2);

	// Check to be sure the vertices have different vertex indices.
	EXPECT_NE(v1->GetIndex(), v2->GetIndex());

	EXPECT_EQ(g->NumVertices(), 2);

	e1 = new CFGEdgeTypeGoto();
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

ControlFlowGraph* ControlFlowGraphTest::CreateSimplestCFG()
{
	// Create the simplest possible CFG.

	ControlFlowGraph *g;
	Entry* entry;
	NoOp* s1;
	Exit* exit_vertex;

	g = new ControlFlowGraph;
	entry = new Entry(Location());
	s1 = new NoOp(Location());
	exit_vertex = new Exit(Location());

	// Add vertices.
	g->AddVertex(entry);
	g->AddVertex(s1);
	g->AddVertex(exit_vertex);

	CFGEdgeTypeFallthrough *e1 = new CFGEdgeTypeFallthrough;
	CFGEdgeTypeFallthrough *e2 = new CFGEdgeTypeFallthrough;

	// Add edges.
	g->AddEdge(entry, s1, e1);
	g->AddEdge(s1, exit_vertex, e2);

	return g;
}

TEST_F(ControlFlowGraphTest, SimpleCFG_NewAndDelete)
{
	SCOPED_TRACE("");

	ControlFlowGraph *g;

	ASSERT_NO_FATAL_FAILURE(g = CreateSimplestCFG());

	delete g;
}

TEST_F(ControlFlowGraphTest, SimpleCFG_graphviz_out)
{
	SCOPED_TRACE("");

	ControlFlowGraph *g;

	ASSERT_NO_FATAL_FAILURE(g = CreateSimplestCFG());

	ASSERT_NO_THROW( boost::write_graphviz(std::cout, *((Graph*)g)) );

	delete g;
}
