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

#include "CFGEdgeTypeFunctionCall.h"

#include "../ControlFlowGraph.h"
#include "../../Function.h"

CFGEdgeTypeFunctionCall::CFGEdgeTypeFunctionCall(FunctionCallResolved *function_call) : CFGEdgeTypeBase()
{
	m_function_call = function_call;

	// We need to know the ControlFlowGraph the target vertex is in.  Get it from the Function
	// that the FunctionCallResolved vertex is calling.
	m_target_cfg = m_function_call->GetCalledFunction()->GetCFGPointer();
}

CFGEdgeTypeFunctionCall::CFGEdgeTypeFunctionCall(const CFGEdgeTypeFunctionCall& orig) : CFGEdgeTypeBase(orig)
{
	Copy(orig);
}

CFGEdgeTypeFunctionCall::~CFGEdgeTypeFunctionCall()
{
	// Nothing to destroy.
}

void CFGEdgeTypeFunctionCall::Copy(const CFGEdgeTypeFunctionCall& orig)
{
	m_function_call = orig.m_function_call;
	m_target_cfg = orig.m_target_cfg;
}

CFGEdgeTypeFunctionCall& CFGEdgeTypeFunctionCall::operator =(
		const CFGEdgeTypeFunctionCall& orig)
{
	Copy(orig);
	return *this;
}
