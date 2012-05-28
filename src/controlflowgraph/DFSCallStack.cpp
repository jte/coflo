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

#include "DFSCallStack.h"

#include "CallStackFrameBase.h"

DFSCallStack::DFSCallStack()
{
}

DFSCallStack::~DFSCallStack()
{
}

void DFSCallStack::PushCallStack(CallStackFrameBase* cfsb)
{
	m_call_stack.push(cfsb);
}

void DFSCallStack::PopCallStack()
{
	// Remove the function we're returning from from the functions-on-the-call-stack set.
	m_call_set.erase(m_call_stack.top()->GetPushingCall()->m_target_function);

	// Delete the CallStackFrameBase object before popping it.
	delete m_call_stack.top();

	// Pop the call stack.
	m_call_stack.pop();
}

CallStackFrameBase* DFSCallStack::TopCallStack()
{
	return m_call_stack.top();
}

bool DFSCallStack::IsCallStackEmpty() const
{
	return m_call_stack.empty();
}

bool DFSCallStack::AreWeRecursing(Function* function)
{
	bool wasnt_already_there;

	boost::tie(boost::tuples::ignore, wasnt_already_there) =
						m_call_set.insert(function);

	return !wasnt_already_there;
}
