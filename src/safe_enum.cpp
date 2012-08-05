/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include "safe_enum.h"

#include <string>
#include <iostream>

SafeEnumBaseClass::SafeEnumBaseClass(const std::string& enum_names)
{
	m_names = enum_names;
}

SafeEnumBaseClass::~SafeEnumBaseClass()
{
}


std::string SafeEnumBaseClass::asString(int value) const
{
	std::string::const_iterator i;
	std::string::const_iterator substring_start = m_names.begin();
	int commas_left_to_find = static_cast<int>(value);
	for(i=m_names.begin(); i!=m_names.end(); i++)
	{
		// Eat whitespace.
		if(*i == ' ' || *i == '\t')
		{
			substring_start=i+1;
			continue;
		}

		if(*i == ',' || (i+1) == m_names.end())
		{
			if(commas_left_to_find == 0)
			{
				if((i+1) == m_names.end())
				{
					i++;
				}
				return std::string(substring_start, i);
			}
			else
			{
				substring_start=i;
				substring_start++;
			}
			commas_left_to_find--;
		}
	}
	/* If we get here, we couldn't find the string, which shouldn't be possible. */
	std::cout << "ERROR: " << value << " " << m_names << std::endl;
	return "ERROR";
}
