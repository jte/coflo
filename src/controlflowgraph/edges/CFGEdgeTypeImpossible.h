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

#ifndef CFGEDGETYPEFALSEEXIT_H
#define	CFGEDGETYPEFALSEEXIT_H

#include <string>

#include "CFGEdgeTypeBase.h"

/**
 * Edge type representing CFG edges from vertices with an out-degree of zero
 * to a Function's EXIT vertex.  These are used to ensure that certain algorithms
 * such as topological traversals operate correctly.
 * Vertices which need an edge of this type added are e.g. the last statement of
 * a loop, which results in a back edge, which then gets ignored by a topological sort.
 */
class CFGEdgeTypeImpossible : public CFGEdgeTypeBase
{
public:
	CFGEdgeTypeImpossible();
	CFGEdgeTypeImpossible(const CFGEdgeTypeImpossible& orig);
	virtual ~CFGEdgeTypeImpossible();
	
	virtual std::string GetDotLabel() const { return "Impossible"; };
	virtual std::string GetDotSVGColor() const { return "gray"; };
	
	bool IsImpossible() const { return true; };

private:

};

#endif	/* CFGEDGETYPEFALSEEXIT_H */

