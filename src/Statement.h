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

//class Location;
#include "Location.h"

/**
 * Abstract base class for all statements in the control flow graph.
 */
class Statement
{
public:
	Statement(const Location *location);
	Statement(const Statement& orig);
	virtual ~Statement();
	
	/**
	 * Get text suitable for setting the statement's attributes in a dot file.
     * @return 
     */
	virtual std::string GetStatementTextDOT() const = 0;
	
	const Location* GetLocation() const { return m_location; };
	
private:

	/// The Location of this statement.
	Location *m_location;
};

#endif	/* STATEMENT_H */

