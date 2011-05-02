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

#include <string>

#include "Statement.h"
#include "Location.h"

Statement::Statement(Location *location) 
{
	m_location = location;
}

Statement::Statement(const Statement& orig) 
{
	m_location = orig.m_location;
}

Statement::~Statement()
{
}

std::string Statement::GetLineNumber() const
{
	if(m_location == NULL)
	{
		return "UNKNOWN";
	}
	else
	{
		return m_location->GetLineNumber(); 
	}
}
