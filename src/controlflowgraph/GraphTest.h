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

#ifndef GRAPHTEST_H_
#define GRAPHTEST_H_

#include "gtest/gtest.h"

class Graph;
class Vertex;

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

	Graph *m_test_graph;
	Vertex *m_test_vert1, *m_test_vert2;
	enum UnitTestLevel
	{
		GraphNewDelete,
		VertexNewDelete,
		VertexAddRemove,
		EdgeNewDelete,
		EdgeAddRemove,
	};
};


#endif /* GRAPHTEST_H_ */