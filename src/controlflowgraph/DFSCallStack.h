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

#ifndef DFSCALLSTACK_H_
#define DFSCALLSTACK_H_

#include "CallStackBase.h"

#include <stack>
#include <boost/tr1/unordered_set.hpp>

class Function;
class CallStackFrameBase;

/*
 *
 */
class DFSCallStack: public CallStackBase
{
public:
	DFSCallStack();
	virtual ~DFSCallStack();

	/**
	 * Push a new stack frame onto the call stack.
	 * @param cfsb
	 */
	virtual void PushCallStack(CallStackFrameBase* cfsb);

	/**
	 * Pop the topmost stack frame off the call stack.
	 */
	virtual void PopCallStack();

	/**
	 * Access the topmost stack frame of the call stack.
	 * @return
	 */
	virtual CallStackFrameBase* TopCallStack();

	virtual bool IsCallStackEmpty() const;

	virtual bool AreWeRecursing(Function* function);

private:

	/// The FunctionCall call stack.
	std::stack<CallStackFrameBase*> m_call_stack;

	/// Typedef for an unordered collection of Function pointers.
	/// Used to efficiently track which functions are on the call stack, for checking if we're going recursive.
	typedef std::tr1::unordered_set<Function*> T_FUNCTION_CALL_SET;

	/// The set of Functions currently on the call stack.
	/// This is currently used only to determine if our call stack has gone recursive.
	T_FUNCTION_CALL_SET m_call_set;
};

#endif /* DFSCALLSTACK_H_ */
