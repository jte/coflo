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

#ifndef STATEMENT_H
#define	STATEMENT_H

#include <string>

#include "../Location.h"

/**
 * Abstract base class for all statements in the control flow graph.
 */
class StatementBase
{
public:
	StatementBase(const Location *location);
	StatementBase(const StatementBase& orig);
	virtual ~StatementBase();
	
	static StatementBase* Parse(std::istream &input_stream);
	
	/**
	 * Get text suitable for setting the statement's attributes in a dot file.
     * @return 
     */
	virtual std::string GetStatementTextDOT() const = 0;
	
	/**
	 * Get text suitable for a text-based CFG printout.
	 * 
     * @return 
     */
	virtual std::string GetIdentifierCFG() const = 0;
	
	/**
	 * Returns a string suitable for use in a Dot "color=" node border attribute.
	 * 
	 * The SVG color scheme is assumed.  Defaults to "black" if this function
	 * is not overridden in derived classes.
	 * 
     * @return A DOT color for the node's border.
     */
	virtual std::string GetDotSVGColor() const { return "black"; };
	
	const Location* GetLocation() const { return m_location; };
	
	virtual std::string GetShapeTextDOT() const { return "rectangle"; };
	
private:

	/// The Location of this statement.
	Location *m_location;
};

#endif	/* STATEMENT_H */

