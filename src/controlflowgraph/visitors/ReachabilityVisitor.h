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

	/**
	 * Typedef for the predicate which this class will call on each vertex it visits to check
	 * for a match.
	 */
	typedef std::tr1::function<bool (ControlFlowGraph &, T_CFG_VERTEX_DESC &)> T_VERTEX_VISITOR_PREDICATE;

	ReachabilityVisitor(ControlFlowGraph &g, T_CFG_VERTEX_DESC source,
			T_VERTEX_VISITOR_PREDICATE inspect_vertex, std::deque<T_CFG_EDGE_DESC> *predecessor_list);
	ReachabilityVisitor(const ReachabilityVisitor& orig);
	virtual ~ReachabilityVisitor();

	/**
	 * For reachability traversal, we want to filter out certain edges, in particular Impossible edges.
	 *
	 * @param u
	 * @return
	 */
	virtual edge_return_value_t examine_edge(T_CFG_EDGE_DESC u);
	virtual vertex_return_value_t discover_vertex(T_CFG_VERTEX_DESC u);
	virtual edge_return_value_t tree_edge(T_CFG_EDGE_DESC e);
	virtual vertex_return_value_t finish_vertex(T_CFG_VERTEX_DESC u);

private:

	/// The starting vertex.
	T_CFG_VERTEX_DESC m_source;

	/// The predicate we will use to inspect each vertex of the control flow graph.  When we find the one we're looking
	/// for, we'll return true.
	T_VERTEX_VISITOR_PREDICATE m_inspect_vertex;

	/// Pointer to the list of predecessors.
	std::deque<T_CFG_EDGE_DESC> *m_predecessor_list;
};

#endif /* REACHABILITYVISITOR_H */
