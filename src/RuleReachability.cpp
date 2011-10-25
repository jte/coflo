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
//#include "controlflowgraph/depth_first_traversal.hpp"
#include "controlflowgraph/ControlFlowGraphTraversalDFS.h"
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

	starting_vertex_desc = m_source->GetEntryVertexDescriptor();

	// Set up a visitor.
	ReachabilityVisitor v(m_cfg, starting_vertex_desc, m_sink->GetEntryVertexDescriptor(), &m_predecessors);

	// Create a depth-first-search graph traversal object.
	ControlFlowGraphTraversalDFS traversal(m_cfg);

	// Traverse the CFG.
	traversal.Traverse(starting_vertex_desc, &v);

	if(!m_predecessors.empty())
	{
		StatementBase *violating_statement = m_cfg.GetStatementPtr(m_predecessors.rbegin()->m_source);
		std::cout << m_source->GetDefinitionFilePath() << ": In function " << m_source->GetIdentifier() << ":" << std::endl;
		std::cout << violating_statement->GetLocation().asGNUCompilerMessageLocation()
				<< ": warning: constraint violation: path exists in control flow graph to " << violating_statement->GetIdentifierCFG() << std::endl;
		std::cout << violating_statement->GetLocation().asGNUCompilerMessageLocation() << ": warning: violating path follows" << std::endl;
		PrintCallChain();
	}
	else
	{
		std::cout << "Couldn't find a constraint violation." << std::endl;
	}

	return true;
}

void RuleReachability::PrintCallChain()
{
	long indent_level = 0;

	BOOST_FOREACH(T_CFG_EDGE_DESC pred, m_predecessors)
	{
		StatementBase *sb = m_cfg.GetStatementPtr(pred.m_source);
		if(sb->IsType<FunctionCall>())
		{
			PrintStatement(sb, indent_level);
		}
		else if(sb->IsType<Entry>())
		{
			indent_level++;
		}
		else if(sb->IsType<Exit>())
		{
			indent_level--;
		}
	}
}

void RuleReachability::PrintStatement(StatementBase *fc, long indent_level)
{
	std::cout << fc->GetLocation().asGNUCompilerMessageLocation() << ": warning: ";
	indent(indent_level);
	std::cout << fc->GetIdentifierCFG() << std::endl;
}


