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

#ifndef FUNCTIONCALLUNRESOLVED_H
#define	FUNCTIONCALLUNRESOLVED_H

#include "FunctionCall.h"

class FunctionCallUnresolved : public FunctionCall
{

public:
	FunctionCallUnresolved(std::string identifier, const Location &location, const std::string &params);
	FunctionCallUnresolved(const FunctionCallUnresolved& orig);
	virtual ~FunctionCallUnresolved();
	
	/**
	 * Get text suitable for setting the statement's attributes in a dot file.
     * @return 
     */
	virtual std::string GetStatementTextDOT() const;
	
	/**
	 * Draw this node with a red border to indicate that it hasn't been resolved.
	 * 
     * @return 
     */
	virtual std::string GetDotSVGColor() const { return "red"; };
	
	virtual std::string GetIdentifier() const { return m_identifier; };
	
private:
	
	/// Identifier of the function we're calling.
	std::string m_identifier;
};

#endif	/* FUNCTIONCALLUNRESOLVED_H */

