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

#ifndef REACHABILITYVISITOR_H
#define REACHABILITYVISITOR_H

#include <deque>

#include "ControlFlowGraphVisitorBase.h"

class RuleReachability;

/**
 * Control flow graph visitor for determining a path, if one exists, from one specified vertex to another.
 */
class ReachabilityVisitor: public ControlFlowGraphVisitorBase
{
public:
	ReachabilityVisitor(ControlFlowGraph &g, T_CFG_VERTEX_DESC source, T_CFG_VERTEX_DESC sink, std::deque<T_CFG_VERTEX_DESC> *predecessor_list);
	ReachabilityVisitor(const ReachabilityVisitor& orig);
	~ReachabilityVisitor();

	vertex_return_value_t discover_vertex(T_CFG_VERTEX_DESC u);
	vertex_return_value_t finish_vertex(T_CFG_VERTEX_DESC u);

private:

	/// The starting vertex.
	T_CFG_VERTEX_DESC m_source;

	/// The vertex we're trying to find.
	T_CFG_VERTEX_DESC m_sink;

	/// Pointer to the list of predecessors.
	std::deque<T_CFG_VERTEX_DESC> *m_predecessor_list;

	/// The reachability rule.
	RuleReachability *m_reachability;
};

#endif /* REACHABILITYVISITOR_H */
