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

#ifndef CALLSTACKBASE_H
#define CALLSTACKBASE_H

class CallStackFrameBase;
class Function;

/**
 * Abstract base class for call stacks used in the various graph traversals.
 */
class CallStackBase
{
public:
	virtual ~CallStackBase() = 0;

	/**
	 * Push a new stack frame onto the call stack.
	 * @param cfsb
	 */
	virtual void PushCallStack(CallStackFrameBase* cfsb) = 0;

	/**
	 * Pop the topmost stack frame off the call stack.
	 */
	virtual void PopCallStack() = 0;

	/**
	 * Access the topmost stack frame of the call stack.
	 * @return
	 */
	virtual CallStackFrameBase* TopCallStack() = 0;

	virtual bool IsCallStackEmpty() const = 0;

	virtual bool AreWeRecursing(Function* function) = 0;
};

#endif /* CALLSTACKBASE_H */
