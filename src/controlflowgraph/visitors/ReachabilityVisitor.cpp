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

#include "ReachabilityVisitor.h"

ReachabilityVisitor::ReachabilityVisitor(T_CFG &g, T_CFG_VERTEX_DESC source, T_CFG_VERTEX_DESC sink, std::vector<T_CFG_VERTEX_DESC> *predecessor_list)
	: ControlFlowGraphVisitorBase(g), m_source(source), m_sink(sink), m_predecessor_list(predecessor_list)
{

}

ReachabilityVisitor::ReachabilityVisitor(const ReachabilityVisitor & orig) : ControlFlowGraphVisitorBase(orig),
		m_source(orig.m_source), m_sink(orig.m_sink)
{
}

ReachabilityVisitor::~ReachabilityVisitor()
{
}

vertex_return_value_t ReachabilityVisitor::discover_vertex(T_CFG_VERTEX_DESC u)
{
	// Add this vertex to the predecessor list.
	m_predecessor_list->push_back(u);
}

vertex_return_value_t ReachabilityVisitor::finish_vertex(T_CFG_VERTEX_DESC u)
{
	// Remove
	m_predecessor_list->pop_back();
}





