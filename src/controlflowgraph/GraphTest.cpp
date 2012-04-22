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

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( GraphTest );

void
GraphTest::setUp()
{
}


void
GraphTest::tearDown()
{
}


void
GraphTest::testConstructor()
{
	CPPUNIT_ASSERT_NO_THROW( m_test_graph = new Graph() );
	CPPUNIT_ASSERT( m_test_graph != NULL );
}

void GraphTest::testCreateVertex()
{
	CPPUNIT_ASSERT_NO_THROW( m_test_vert1 = new Vertex() );
	CPPUNIT_ASSERT( m_test_vert1 != NULL );
	CPPUNIT_ASSERT_NO_THROW( m_test_vert2 = new Vertex() );
}

void GraphTest::testAddVertex()
{
	CPPUNIT_ASSERT_NO_THROW( m_test_graph = new Graph() );
	CPPUNIT_ASSERT( m_test_graph != NULL );
	CPPUNIT_ASSERT_NO_THROW( m_test_vert1 = new Vertex() );
	CPPUNIT_ASSERT( m_test_vert1 != NULL );
	CPPUNIT_ASSERT_NO_THROW( m_test_vert2 = new Vertex() );
	CPPUNIT_ASSERT_NO_THROW( m_test_graph->AddVertex(m_test_vert1) );
	CPPUNIT_ASSERT( m_test_graph->NumVertices() == 1 );
	CPPUNIT_ASSERT_NO_THROW( m_test_graph->AddVertex(m_test_vert2) );
	CPPUNIT_ASSERT( m_test_graph->NumVertices() == 2 );
}


