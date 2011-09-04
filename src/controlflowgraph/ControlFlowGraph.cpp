/*
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include "ControlFlowGraph.h"

static T_CFG_VERTEX_DESC f_dummy_vertex;

void InitializeControlFlowGraph(T_CFG &cfg)
{
	// Create a dummy node.
	f_dummy_vertex = boost::add_vertex(cfg);
}

bool IsDummyVertex(const T_CFG_VERTEX_DESC &v)
{
	return (v == f_dummy_vertex);
}

T_CFG_VERTEX_DESC GetDummyVertex()
{
	return f_dummy_vertex;
}

void PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg)
{
	T_CFG_OUT_EDGE_ITERATOR ei, eend;
	
	boost::tie(ei, eend) = boost::out_edges(vdesc, cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(cfg[*ei].m_edge_type)).name() << std::endl;
	}
}

void PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg)
{
	T_CFG_IN_EDGE_ITERATOR ei, eend;
	
	boost::tie(ei, eend) = boost::in_edges(vdesc, cfg);
	for(;ei!=eend; ++ei)
	{
		std::cout << typeid(*(cfg[*ei].m_edge_type)).name() << std::endl;
	}
}
