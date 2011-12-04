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
// Include the TR1 <functional> header.
#include <boost/tr1/functional.hpp>

#include "ControlFlowGraphVisitorBase.h"


/**
 * Control flow graph visitor for determining a path, if one exists, from one specified vertex to another.
 */
class ReachabilityVisitor: public ControlFlowGraphVisitorBase
{
public:

	typedef std::tr1::function<bool (ControlFlowGraph &, T_CFG_VERTEX_DESC &)> T_VERTEX_VISITOR_PREDICATE;

	ReachabilityVisitor(ControlFlowGraph &g, T_CFG_VERTEX_DESC source,
			T_VERTEX_VISITOR_PREDICATE inspect_vertex, std::deque<T_CFG_EDGE_DESC> *predecessor_list);
	ReachabilityVisitor(const ReachabilityVisitor& orig);
	virtual ~ReachabilityVisitor();

	virtual vertex_return_value_t discover_vertex(T_CFG_VERTEX_DESC u);
	virtual edge_return_value_t tree_edge(T_CFG_EDGE_DESC e);
	virtual vertex_return_value_t finish_vertex(T_CFG_VERTEX_DESC u);

private:

	/// The starting vertex.
	T_CFG_VERTEX_DESC m_source;

	/// The vertex we're trying to find.
	//T_CFG_VERTEX_DESC m_sink;

	/// The predicate we will use to inspect each vertex of the control flow graph.  When we find the one we're looking
	/// for, we'll return true.
	T_VERTEX_VISITOR_PREDICATE m_inspect_vertex;

	/// Pointer to the list of predecessors.
	std::deque<T_CFG_EDGE_DESC> *m_predecessor_list;
};

#endif /* REACHABILITYVISITOR_H */
