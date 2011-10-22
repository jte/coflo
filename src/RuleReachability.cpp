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

#include <iostream>

#include <boost/graph/depth_first_search.hpp>
#include <boost/foreach.hpp>

#include "controlflowgraph/ControlFlowGraph.h"
#include "controlflowgraph/depth_first_traversal.hpp"
#include "controlflowgraph/visitors/ReachabilityVisitor.h"
#include "controlflowgraph/statements/Entry.h"
#include "Function.h"
#include "RuleReachability.h"
#include "controlflowgraph/statements/Entry.h"

RuleReachability::RuleReachability(ControlFlowGraph &cfg, const Function *source, const Function *sink) : RuleDFSBase(cfg)
{
	m_source = source;
	m_sink = sink;
}

RuleReachability::RuleReachability(const RuleReachability& orig) : RuleDFSBase(orig)
{
	m_source = orig.m_source;
	m_sink = orig.m_sink;
}

RuleReachability::~RuleReachability()
{
}

bool RuleReachability::RunRule()
{
	T_CFG_VERTEX_DESC starting_vertex_desc;
	//reachability_visitor v(m_cfg, m_sink->GetEntryVertexDescriptor(), this);
	starting_vertex_desc = m_source->GetEntryVertexDescriptor();

	ReachabilityVisitor v(m_cfg, starting_vertex_desc, m_sink->GetEntryVertexDescriptor(), &m_predecessors);

	improved_depth_first_visit(m_cfg.GetT_CFG(), starting_vertex_desc, v);

	return true;
}

void RuleReachability::PrintCallChain(T_CFG &cfg, T_CFG_VERTEX_DESC v)
{
	std::deque<T_CFG_VERTEX_DESC>::iterator it;

	for(it = m_predecessors.begin(); it != m_predecessors.end(); it++ )
	//BOOST_FOREACH(T_CFG_VERTEX_DESC pred, m_predecessors)
	{
		if(cfg[*it].m_statement->IsType<Entry>())
		{
			std::cout << "Function Entry: " << cfg[*it].m_containing_function->GetIdentifier() << std::endl;
		}
	}
}
