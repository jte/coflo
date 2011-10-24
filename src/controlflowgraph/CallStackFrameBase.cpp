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

#include "CallStackFrameBase.h"

CallStackFrameBase::CallStackFrameBase(FunctionCallResolved *function_call_which_pushed_this_frame)
{
	m_function_call_which_pushed_this_frame = function_call_which_pushed_this_frame;

	// Create a new color map.
	m_color_map = new T_COLOR_MAP;
}

CallStackFrameBase::~CallStackFrameBase()
{
	// We own the color map, so destroy it.
	delete m_color_map;
}

