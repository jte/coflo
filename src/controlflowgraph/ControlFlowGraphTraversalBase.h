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

#ifndef CONTROLFLOWGRAPHTRAVERSALBASE_H_
#define CONTROLFLOWGRAPHTRAVERSALBASE_H_

#include <boost/graph/graph_traits.hpp>

#include "ControlFlowGraph.h"
#include "CallStackFrameBase.h"
#include "visitors/ControlFlowGraphVisitorBase.h"

class CallStackFrameBase;

/**
 * Base class for ControlFlowGraph traversals.
 */
class ControlFlowGraphTraversalBase
{
public:
	ControlFlowGraphTraversalBase(ControlFlowGraph &control_flow_graph);
	virtual ~ControlFlowGraphTraversalBase();

	/**
	 * Traverse the control flow graph, starting at vertex @a source and continuing until @a visitor tells the
	 * traversal to stop.  The actual traversal algorithm is defined in derived classes.
	 *
	 * @param source  The vertex to start the traversal from.
	 * @param visitor The ControlFlowGraphVisitorBase()-derived visitor which will examine the nodes traversed and stop the traversal.
	 */
	virtual void Traverse(boost::graph_traits<T_CFG>::vertex_descriptor source,
			ControlFlowGraphVisitorBase *visitor) = 0;

protected:

	/// @name Interface for maintaining a call stack.
	//@{

	/**
	 * Push a new stack frame onto the call stack.
	 * @param cfsb
	 */
	void PushCallStack(CallStackFrameBase* cfsb);

	/**
	 * Pop the topmost stack frame off the call stack.
	 */
	void PopCallStack();

	/**
	 * Access the topmost stack frame of the call stack.
	 * @return
	 */
	CallStackFrameBase* TopCallStack();
	bool IsCallStackEmpty() const;
	bool AreWeRecursing(Function* function);

	//@}

	/// Reference to the ControlFlowGraph we're visiting.
	ControlFlowGraph &m_control_flow_graph;

private:

	/// The FunctionCall call stack.
	std::stack<CallStackFrameBase*> m_call_stack;

	/// Typedef for an unordered collection of Function pointers.
	/// Used to efficiently track which functions are on the call stack, for checking if we're going recursive.
	typedef boost::unordered_set<Function*> T_FUNCTION_CALL_SET;

	/// The set of Functions currently on the call stack.
	/// This is currently used only to determine if our call stack has gone recursive.
	T_FUNCTION_CALL_SET m_call_set;
};

#endif /* CONTROLFLOWGRAPHTRAVERSALBASE_H_ */
