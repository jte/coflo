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

#include "RuleReachability.h"

#include <iostream>

#include <boost/graph/depth_first_search.hpp>
#include <boost/foreach.hpp>

#include "../ControlFlowGraph.h"
#include "../ControlFlowGraphTraversalDFS.h"
#include "../visitors/ReachabilityVisitor.h"
#include "../statements/Entry.h"
#include "Function.h"

#include "../statements/Entry.h"

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

/**
 * This is the predicate which will be called by the ReachabilityVisitor as it visits each vertex.
 * It will return true to indicate that the target vertex was discovered.
 */
struct ReachabilityPredicateSpecificVertex
{
	ReachabilityPredicateSpecificVertex(T_CFG_VERTEX_DESC sink) { m_sink = sink; };

	bool operator()(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC &v)
	{
		if(v == m_sink)
		{
			return true;
		}

		return false;
	}

	/// The vertex we're trying to find.
	T_CFG_VERTEX_DESC m_sink;
};

bool RuleReachability::RunRule()
{
	T_CFG_VERTEX_DESC starting_vertex_desc;

	// Get the starting vertex.
	starting_vertex_desc = m_source->GetEntryVertexDescriptor();

	// Push a fake edge onto the predecessor stack, since our last vertex will try to pop it.
	m_predecessors.push_back(m_source->GetEntrySelfEdgeDescriptor());

	// Set up a visitor.
	ReachabilityPredicateSpecificVertex pred(m_sink->GetEntryVertexDescriptor());
	ReachabilityVisitor v(m_cfg, starting_vertex_desc, pred, &m_predecessors);

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
		std::cout << "Couldn't find a violation of constraint: "
				<< m_source->GetIdentifier()
				<< "() -x "
				<< m_sink->GetIdentifier() << "()"
				<< std::endl;
	}

	return true;
}

void RuleReachability::PrintCallChain()
{
	long indent_level = 0;
	long bypass_call_depth = 0;
	bool ignore_function_call = false;

	std::deque<T_CFG_EDGE_DESC> m_new_predecessors;

	// First strip the call chain of all function calls that returned with no matches.
	BOOST_REVERSE_FOREACH(T_CFG_EDGE_DESC pred, m_predecessors)
	{
		StatementBase *sb = m_cfg.GetStatementPtr(pred.m_source);

		if (sb->IsType<FunctionCallUnresolved>())
		{
			// By definition, unresolved calls will never be in our call stack.
			continue;
		}

#if 0
		if(sb->IsType<Exit>())
		{
			// Seeing an exit vertex means we're at a return from a successful call.
			// We don't care about these here.
			bypass_call_depth++;
		}
		else if(sb->IsType<Entry>() && bypass_call_depth>0)
		{
			bypass_call_depth--;
			// The previous predecessor will be a FunctionCall, ignore it.
			ignore_function_call = true;
		}
		else if (ignore_function_call)
		{
			ignore_function_call = false;
		}
		else if(bypass_call_depth == 0)
		{
#endif
			m_new_predecessors.push_front(pred);
/**
		}
**/
	}


	m_predecessors.swap(m_new_predecessors);

	BOOST_FOREACH(T_CFG_EDGE_DESC pred, m_predecessors)
	{
		StatementBase *sb = m_cfg.GetStatementPtr(pred.m_source);
		if(sb->IsType<FunctionCall>())
		{
			PrintStatement(sb, indent_level);
		}
		else if(sb->IsDecisionStatement())
		{
			// It's a decision statement
			PrintStatement(sb, m_cfg.GetEdgeTypePtr(pred), indent_level);
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

void RuleReachability::PrintStatement(StatementBase *sb, CFGEdgeTypeBase *eb, long  indent_level)
{
	std::cout << sb->GetLocation().asGNUCompilerMessageLocation() << ": warning: ";
	indent(indent_level);
	std::cout << sb->GetIdentifierCFG() << ", taking out edge \"" << eb->GetLabel() << "\"" << std::endl;
}




