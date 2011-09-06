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

#ifndef FUNCTIONCALL_H
#define	FUNCTIONCALL_H

#include <string>

#include "StatementBase.h"

class Function;
class Location;

/**
 * Class representing a function call statement.
 */
class FunctionCall : public StatementBase
{
public:
	FunctionCall(const Location &location, const std::string &params);
	FunctionCall(const FunctionCall& orig);
	virtual ~FunctionCall();
	
	/// Returns the name of the function being called.
	virtual std::string GetIdentifier() const = 0;
	
	virtual std::string GetIdentifierCFG() const;

	/**
	 * Anything derived from this class is a function call of some sort.
	 *
	 * @return true, indicating this is a function call.
	 */
	virtual bool IsFunctionCall() const { return true; };

	/// The parameters passed to the function.
	/// @todo We need to handle this better in many ways.
	std::string m_params;

private:


};

#endif	/* FUNCTIONCALL_H */
