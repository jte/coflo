/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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


#include "ReachabilityVisitor.h"


ReachabilityVisitor::ReachabilityVisitor(ControlFlowGraph &g, ControlFlowGraph::vertex_descriptor source,
		T_VERTEX_VISITOR_PREDICATE inspect_vertex, std::deque<ControlFlowGraph::edge_descriptor> *predecessor_list)
	: ControlFlowGraphVisitorBase(g), m_source(source), m_inspect_vertex(inspect_vertex)
{
	m_predecessor_list = predecessor_list;
}

ReachabilityVisitor::ReachabilityVisitor(const ReachabilityVisitor & orig) : ControlFlowGraphVisitorBase(orig),
		m_source(orig.m_source), m_inspect_vertex(orig.m_inspect_vertex)
{
	m_predecessor_list = orig.m_predecessor_list;
}

ReachabilityVisitor::~ReachabilityVisitor()
{
}

edge_return_value_t ReachabilityVisitor::examine_edge(ControlFlowGraph::edge_descriptor u)
{
	if(u->IsImpossible() || u->IsBackEdge())
	{
		// Ignore Impossible edges.
		return edge_return_value_t::terminate_branch;
	}
	else
	{
		return edge_return_value_t::ok;
	}
}

vertex_return_value_t ReachabilityVisitor::discover_vertex(ControlFlowGraph::vertex_descriptor u)
{
	if(m_inspect_vertex(m_cfg, u) == true)
	{
		// We found the vertex we were looking for, terminate the search.
		return vertex_return_value_t::terminate_search;
	}

	return vertex_return_value_t::ok;
}

edge_return_value_t ReachabilityVisitor::tree_edge(ControlFlowGraph::edge_descriptor e)
{
	// Add this edge to the predecessor list.
	m_predecessor_list->push_back(e);
	//std::cout << "PUSH, size=" << m_predecessor_list->size() << std::endl;

	return edge_return_value_t::ok;
}


vertex_return_value_t ReachabilityVisitor::finish_vertex(ControlFlowGraph::vertex_descriptor u)
{
	// Remove this vertex from the predecessor stack.
	//std::cout << "POP, size=" << m_predecessor_list->size() << std::endl;
	m_predecessor_list->pop_back();

	return vertex_return_value_t::ok;
}





