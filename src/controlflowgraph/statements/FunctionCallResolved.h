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

#ifndef FUNCTIONCALLRESOLVED_H
#define	FUNCTIONCALLRESOLVED_H

#include "FunctionCall.h"

class Function;
class FunctionCallUnresolved;

/**
 * A resolved function call.
 */
class FunctionCallResolved : public FunctionCall
{

public:
	FunctionCallResolved(Function *target_function, FunctionCallUnresolved *fcu);
	FunctionCallResolved(const FunctionCallResolved& orig);
	virtual ~FunctionCallResolved();
	
	/**
	 * Get text suitable for setting the statement's attributes in a dot file.
     * @return 
     */
	virtual std::string GetStatementTextDOT() const;
	
	virtual std::string GetIdentifier() const;
	
//private:
	
	/// Pointer to the function we're calling.
	Function *m_target_function;
};

#endif	/* FUNCTIONCALLRESOLVED_H */

