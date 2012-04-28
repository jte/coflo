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

#ifndef CFGDFSVISITOR_H
#define	CFGDFSVISITOR_H

#include <stack>
#include <boost/unordered_set.hpp>

#include "ImprovedDFSVisitorBase.h"
#include "../ControlFlowGraph.h"

/// @todo This should probably be a template parameter.
class FunctionCallResolved;
class Function;

/**
 * Base class for ControlFlowGraph visitors.
 * ControlFlowGraphVisitorBase adds a call stack usable by derived classes.
 */
class ControlFlowGraphVisitorBase : public ImprovedDFSVisitorBase<ControlFlowGraph::vertex_descriptor, ControlFlowGraph::edge_descriptor, ControlFlowGraph>
{
public:

	ControlFlowGraphVisitorBase(ControlFlowGraph &cfg);
	ControlFlowGraphVisitorBase(const ControlFlowGraphVisitorBase& orig);
	virtual ~ControlFlowGraphVisitorBase();
	
	virtual vertex_return_value_t initialize_vertex(ControlFlowGraph::vertex_descriptor u);
	virtual vertex_return_value_t start_vertex(ControlFlowGraph::vertex_descriptor u);
	virtual vertex_return_value_t discover_vertex(ControlFlowGraph::vertex_descriptor u);
	virtual edge_return_value_t examine_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t tree_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t back_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t forward_or_cross_edge(ControlFlowGraph::edge_descriptor u);
	virtual vertex_return_value_t finish_vertex(ControlFlowGraph::vertex_descriptor u);

protected:

	void PushCallStack(FunctionCallResolved* pushing_function_call);
	void PopCallStack();
	FunctionCallResolved* TopCallStack();
	bool IsCallStackEmpty() const;
	bool AreWeRecursing(Function* function);

	/// Reference to the ControlFlowGraph we're traversing.
	ControlFlowGraph &m_cfg;

private:

	/// The FunctionCall call stack.
	std::stack<FunctionCallResolved*> m_call_stack;

	/// Typedef for an unordered collection of Function pointers.
	/// Used to efficiently track which functions are on the call stack, for checking if we're going recursive.
	typedef boost::unordered_set<Function*> T_FUNCTION_CALL_SET;

	/// The set of Functions currently on the call stack.
	/// This is currently used only to determine if our call stack has gone recursive.
	T_FUNCTION_CALL_SET m_call_set;
};

#endif	/* CFGDFSVISITOR_H */

