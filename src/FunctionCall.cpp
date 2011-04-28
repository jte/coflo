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

#include "FunctionCall.h"


FunctionCall::FunctionCall(std::string identifier, Location *location) : Statement(location)
{
	m_identifier = identifier;
	m_function = NULL;
}

FunctionCall::FunctionCall(const FunctionCall& orig) : Statement(orig)
{
	m_identifier = orig.m_identifier;
	m_function = orig.m_function;
}

FunctionCall::~FunctionCall()
{
}

std::string FunctionCall::GetIdentifier() const
{
	if(m_function == NULL)
	{
		// Haven't linked yet, return the identifier we found.
		return m_identifier;
	}
	else
	{
		/// \todo Not implemented.
		return std::string("NOT YET IMPLEMENTED");
	}
}
