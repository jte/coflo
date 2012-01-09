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

#include "CFGEdgeTypeBase.h"

CFGEdgeTypeBase::CFGEdgeTypeBase()
{
	// We're not a back edge until told otherwise.
	m_is_back_edge = false;
}

CFGEdgeTypeBase::CFGEdgeTypeBase(const CFGEdgeTypeBase& orig)
{
	m_is_back_edge = orig.m_is_back_edge;
}

CFGEdgeTypeBase::~CFGEdgeTypeBase() 
{
}

std::string CFGEdgeTypeBase::GetDotStyle() const
{
	if(m_is_back_edge)
	{
		// This is a back edge, make it a dashed line.
		return "dashed";
	}
	else
	{
		return "solid";
	}
}
