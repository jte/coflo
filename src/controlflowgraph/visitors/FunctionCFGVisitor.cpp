/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include "FunctionCFGVisitor.h"

#include <iostream>

#include "../edges/edge_types.h"
#include "../../Function.h"

static void indent(long i)
{
	while (i > 0)
	{
		std::cout << "    ";
		i--;
	};
}

static long filtered_in_degree(ControlFlowGraph::vertex_descriptor v)
{
	StatementBase::in_edge_iterator ieit, ieend;

	v->InEdges(&ieit, &ieend);

	long i = 0;
	bool saw_function_call_already = false;
	for (; ieit != ieend; ++ieit)
	{
		if ((*ieit)->IsBackEdge())
		{
			// Always skip anything marked as a back edge.
			continue;
		}

		// Count up all the incoming edges, with two exceptions:
		// - Ignore Return edges.  They will always have exactly one matching Fallthrough in edge from a FunctionCallResolved,
		//   which is what we'll count instead.
		// - Ignore all but the first CFGEdgeTypeFunctionCall.  The situation here is that we'd be
		//   looking at a vertex v that's an ENTRY statement, with a predecessor of type FunctionCallResolved.
		//   Any particular instance of an ENTRY has at most only one valid FunctionCall edge.
		//   For our current purposes, we only care about this one.
		if ((dynamic_cast<CFGEdgeTypeReturn*>(*ieit) == NULL)
				&& (saw_function_call_already == false))
		{
			i++;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(*ieit)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return i;
}


FunctionCFGVisitor::~FunctionCFGVisitor()
{

}

vertex_return_value_t FunctionCFGVisitor::start_vertex(ControlFlowGraph::edge_descriptor u)
{
	// The very first vertex has been popped.

	// We're at the first function entry point.
	m_call_stack->PushCallStack(new CallStackFrameBase(NULL, u->Target()->GetOwningFunction()->GetCFGPointer()));
	m_indent_level = 0;

	return vertex_return_value_t::ok;
};

vertex_return_value_t FunctionCFGVisitor::discover_vertex(ControlFlowGraph::vertex_descriptor u, ControlFlowGraph::edge_descriptor e)
{
	// We found a new vertex.

	StatementBase *p = u;

	if(p->IsType<Entry>())
	{
		// We're visiting a function entry point.
		// Push a new call stack frame.

		indent(m_indent_level);
		std::cout << "[" << std::endl;
		//PushCallStack(m_next_function_call_resolved);
		m_indent_level++;
	}

	// Check if this vertex is the first vertex of a new branch of the control flow graph.
	long fid = filtered_in_degree(u);
	if(fid==1)
	{
		ControlFlowGraph::vertex_descriptor predecessor;
		predecessor = e->Source();
		if(predecessor->IsDecisionStatement())
		{
			// Predecessor was a decision statement, so this vertex starts a new branch.
			// Print a block start marker at the current indent level minus one.
			indent(m_indent_level);
			std::cout << "{" <<std::endl;
			m_indent_level++;
		}
	}
	else if (fid > 2)
	{
		// This vertex has some extra incoming edges that haven't been outdented.
		for(long i=fid-2; i>0; --i)
		{
			m_indent_level--;
			indent(m_indent_level);
			std::cout << "}" << std::endl;
		}
	}

	// Check if this vertex meets the criteria for printing the statement.
	if(m_cfg_verbose || (p->IsDecisionStatement() || (p->IsFunctionCall())))
	{
		// Indent and print the statement corresponding to this vertex.
		indent(m_indent_level);
		std::cout << p->GetIdentifierCFG();
		if(m_cfg_vertex_ids)
		{
			// Print the vertex ID.
			std::cout << " [" << u << "]";
		}
		std::cout << " <" << p->GetLocation() << ">" << std::endl;
	}

	if (u == m_last_statement)
	{
		//std::clog << "INFO: Found last statement of function" << std::endl;
		// We've reached the end of the function, terminate the search.
		// We should never have to do this, the topological search should always
		// terminate on the EXIT vertex unless there is a branch which erroneously terminates.
		//return terminate_search;
	}

	if (p->IsType<FunctionCallResolved>())
	{
		// This is a function call which has been resolved (i.e. has a link to the
		// actual Function that's being called).  Track the call context, and
		// check if we're going recursive.
		FunctionCallResolved *fcr;

		fcr = dynamic_cast<FunctionCallResolved*>(p);

		// Assume we're not.
		m_last_discovered_vertex_is_recursive = false;

		if(m_call_stack->AreWeRecursing(fcr->m_target_function))
		{
			// We're recursing, we need to treat this vertex as if it were a FunctionCallUnresolved.
			std::cout << "RECURSION DETECTED: Function \"" << fcr->m_target_function << "\"" << std::endl;
			m_last_discovered_vertex_is_recursive = true;
		}
		else
		{
			// We're not recursing, push a normal stack frame and do the call.
			m_call_stack->PushCallStack(new CallStackFrameBase(fcr, fcr->m_target_function->GetCFGPointer()));
		}
	}

	return vertex_return_value_t::ok;
}

edge_return_value_t FunctionCFGVisitor::examine_edge(ControlFlowGraph::edge_descriptor ed)
{
	// Filter out any edges that we want to pretend aren't even part of the
	// graph we're looking at.
	CFGEdgeTypeBase *edge_type;
	CFGEdgeTypeFunctionCall *fc;
	CFGEdgeTypeReturn *ret;
	CFGEdgeTypeFunctionCallBypass *fcb;

	edge_type = ed;

	// Attempt dynamic casts to call/return types to see if we need to handle
	// these specially.
	fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(edge_type);
	ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);
	fcb = dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(edge_type);

	if(ed->Source() == ed->Target())
	{
		// Skip all edges which have the same source and target.  That means they're ENTRY or EXIT pseudostatements.
		return edge_return_value_t::terminate_branch;
	}

	if (edge_type->IsBackEdge())
	{
		// Skip all back edges.
		return edge_return_value_t::terminate_branch;
	}

	if (ret != NULL)
	{
		// This is a return edge.

		if(m_call_stack->IsCallStackEmpty())
		{
			// Should never get here.
			std::cout << "EMPTY" <<std::endl;
		}
		else if(m_call_stack->TopCallStack()->GetPushingCall() == NULL)
		{
			// We're at the top of the call stack, and we're trying to return.
			std::cout << "NULL" <<std::endl;
			return edge_return_value_t::terminate_branch;
		}
		else if(ret->m_function_call != m_call_stack->TopCallStack()->GetPushingCall())
		{
			// This edge is a return, but not the one corresponding to the FunctionCall
			// that brought us here.  Or, the call stack is empty, indicating that we got here
			// by means other than tracing the control-flow graph (e.g. we started tracing the
			// graph at an internal vertex).
			// Skip it.
			/// @todo An empty call stack here could also be an error in the program.  We should maybe
			/// add a fake "call" when starting a cfg trace from an internal vertex.
			return edge_return_value_t::terminate_branch;
		}
	}

	// Handle recursion.
	// We deal with recursion by deciding here which path to take out of a FunctionCallResolved vertex.
	// Note that this is currently the only vertex type which can result in recursion.
	if(ed->Source()->IsType<FunctionCallResolved>())
	{
		if(m_last_discovered_vertex_is_recursive && (fc != NULL))
		{
			// We are in danger of infinite recursion.
			// Skip this function call.
			std::cout << "t3" <<std::endl;
			return edge_return_value_t::terminate_branch;
		}
		else if(!m_last_discovered_vertex_is_recursive && (ed->IsType<CFGEdgeTypeFallthrough>()))
		{
			// Last vertex wasn't recursive, don't take the fallthrough.
			return edge_return_value_t::terminate_branch;
		}
	}

	return edge_return_value_t::ok;
}

void FunctionCFGVisitor::vertex_visit_complete(ControlFlowGraph::vertex_descriptor u, long num_vertices_pushed, ControlFlowGraph::edge_descriptor e)
{
	// Check if we're leaving an Exit vertex.
	StatementBase *p = u;
	if(p->IsType<Exit>())
	{
		// We're leaving the function we were in, pop the call stack entry it pushed.
		m_call_stack->PopCallStack();

		// Outdent.
		m_indent_level--;
		indent(m_indent_level);
		std::cout << "]" << std::endl;

		if(m_last_statement == u)
		{
			// This is the last statement of the function we were printing.
			// No need to check if we need to outdent due to a branch termination.
			return;
		}
	}

	// Check if the vertex we terminate on has more than just us coming in.
	if(num_vertices_pushed == 0)
	{
		// No target vertices pushed by this vertex.  That means that some other vertex did push our target vertex,
		// or that we have no out edges.  Either way we terminate the branch.
		m_indent_level--;
		indent(m_indent_level);

		// We're leaving a branch indent context.
		std::cout << "}" << std::endl;
	}

	if	((num_vertices_pushed == 1) && (filtered_in_degree(e->Target()) > 1))
	{
		// The edge will end on a merge vertex.  Outdent.
		m_indent_level--;
		indent(m_indent_level);
		std::cout << "}" << std::endl;
	}
}
