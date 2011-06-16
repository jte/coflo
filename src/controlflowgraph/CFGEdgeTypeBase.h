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

#ifndef CFGEDGETYPEBASE_H
#define	CFGEDGETYPEBASE_H

/**
 * Base class for control flow graph edge types.
 */
class CFGEdgeTypeBase
{

public:
	CFGEdgeTypeBase();
	CFGEdgeTypeBase(const CFGEdgeTypeBase& orig);
	/// Destructor made pure virtual to force this to be an abstract base class.
	virtual ~CFGEdgeTypeBase() = 0;
	
	/**
	 * Indicates if this is a back edge in the control flow graph, as would be
	 * caused by a loop.
	 * 
     * @return true if this is a back-edge, false if it isn't.
     */
	virtual bool IsBackEdge() const { return false; };

private:

};

#endif	/* CFGEDGETYPEBASE_H */
