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

#ifndef CFGEDGETYPEFALLTHROUGH_H
#define	CFGEDGETYPEFALLTHROUGH_H

#include "CFGEdgeTypeBase.h"

class CFGEdgeTypeFallthrough : public CFGEdgeTypeBase
{
public:
	CFGEdgeTypeFallthrough();
	CFGEdgeTypeFallthrough(const CFGEdgeTypeFallthrough& orig);
	virtual ~CFGEdgeTypeFallthrough();
	
	virtual std::string GetDotLabel() const { return ""; };

private:
	
};

#endif	/* CFGEDGETYPEFALLTHROUGH_H */
