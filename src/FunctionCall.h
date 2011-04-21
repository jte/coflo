/**
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

#include "Statement.h"

class Function;

/**
 * Class representing a function call statement.
 * @param identifier
 */
class FunctionCall : public Statement
{
public:
	FunctionCall(std::string identifier);
	FunctionCall(const FunctionCall& orig);
	virtual ~FunctionCall();
	
	virtual std::string GetStatementText() const { return m_identifier + "()"; };
	
	/// Returns the name of the function being called.
	std::string GetIdentifier() const;

private:
	
	/// Identifier of the function we're calling.
	std::string m_identifier;
	
	/// Pointer to the function we're calling.
	/// Will be NULL at least until we've linked, and
	/// may not be resolvable even then.
	Function *m_function;
};

#endif	/* FUNCTIONCALL_H */
