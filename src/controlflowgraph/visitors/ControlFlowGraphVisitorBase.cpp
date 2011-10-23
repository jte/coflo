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

#include "ControlFlowGraphVisitorBase.h"
#include "../../Function.h"


ControlFlowGraphVisitorBase::ControlFlowGraphVisitorBase(ControlFlowGraph & cfg) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(cfg.GetConstT_CFG()), m_cfg(cfg)
{
}


ControlFlowGraphVisitorBase::ControlFlowGraphVisitorBase(const ControlFlowGraphVisitorBase& orig) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(orig), m_cfg(orig.m_cfg)
{
}

ControlFlowGraphVisitorBase::~ControlFlowGraphVisitorBase()
{
}

vertex_return_value_t ControlFlowGraphVisitorBase::initialize_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok; 
};

vertex_return_value_t ControlFlowGraphVisitorBase::start_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok;
};

vertex_return_value_t ControlFlowGraphVisitorBase::discover_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::examine_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::tree_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::back_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::forward_or_cross_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

vertex_return_value_t ControlFlowGraphVisitorBase::finish_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok;
};

void ControlFlowGraphVisitorBase::PushCallStack(FunctionCallResolved* pushing_function_call)
{
	m_call_stack.push(pushing_function_call);
}

void ControlFlowGraphVisitorBase::PopCallStack()
{
	// Remove the function we're returning from from the functions-on-the-call-stack set.
	m_call_set.erase(m_call_stack.top()->m_target_function);

	// Pop the call stack.
	m_call_stack.pop();
}

FunctionCallResolved *ControlFlowGraphVisitorBase::TopCallStack()
{
	return m_call_stack.top();
}

bool ControlFlowGraphVisitorBase::IsCallStackEmpty() const
{
	return m_call_stack.empty();
}

bool ControlFlowGraphVisitorBase::AreWeRecursing(Function *function)
{
	bool wasnt_already_there;

	boost::tie(boost::tuples::ignore, wasnt_already_there) =
						m_call_set.insert(function);

	return !wasnt_already_there;
}




