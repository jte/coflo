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

#include <string>

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
	
	/**
	 * Returns a string suitable for use as an edge label in a Dot graph.
	 * 
	 * Unless overridden in derived classes, returns an empty string.
	 * 
     * @return A string suitable for use as an edge label in a Dot graph.
     */
	virtual std::string GetDotLabel() const { return std::string(); };
	
	/**
	 * Returns a string suitable for use in a Dot "color=" edge attribute.
	 * 
	 * The SVG color scheme is assumed.  Defaults to "black" if this function
	 * is not overridden in derived classes.
	 * 
     * @return 
     */
	virtual std::string GetDotSVGColor() const { return "black"; };

private:

};

#endif	/* CFGEDGETYPEBASE_H */
