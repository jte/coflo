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

#ifndef CALLSTACKFRAMEBASE_H_
#define CALLSTACKFRAMEBASE_H_

#include <boost/graph/properties.hpp>

#include "SparsePropertyMap.h"
#include "ControlFlowGraph.h"

class FunctionCallResolved;

/**
 * Base class for CallStackBase stack frames.
 */
class CallStackFrameBase
{
public:
	typedef SparsePropertyMap< T_CFG_VERTEX_DESC, boost::default_color_type, boost::white_color > T_COLOR_MAP;

	explicit CallStackFrameBase(FunctionCallResolved *function_call_which_pushed_this_frame);
	virtual ~CallStackFrameBase();

	FunctionCallResolved* GetPushingCall() { return m_function_call_which_pushed_this_frame; };

	T_COLOR_MAP* GetColorMap() { return m_color_map; };


private:

	/// The FunctionCallResolved vertex which resulted in this frame being pushed onto the stack.
	/// All stack frames need at least this information so that the return edges can be determined.
	FunctionCallResolved *m_function_call_which_pushed_this_frame;

	T_COLOR_MAP *m_color_map;

};

#endif /* CALLSTACKFRAMEBASE_H_ */
