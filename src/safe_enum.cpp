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

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>



SafeEnumBaseClass::SafeEnumBaseClass(const std::string& enum_names)
{
	// Convert the stringized enumerator list into a vector of strings.
	EnumeratorStringToVectorOfStrings(enum_names);
}

SafeEnumBaseClass::~SafeEnumBaseClass()
{
}

void SafeEnumBaseClass::EnumeratorStringToVectorOfStrings(const std::string &enum_names)
{
	static const std::string delimiters = "\t ,";

	boost::split(m_enumerator_names, enum_names, boost::is_any_of(delimiters), boost::token_compress_on);
}

std::string SafeEnumBaseClass::asString(int value) const
{
	return m_enumerator_names[value];
}

std::string SafeEnumBaseClass::GetEnumeratorsAsString() const
{
	std::string retval;

	std::cout << "Enumerators:" << std::endl;
	BOOST_FOREACH(std::string s, m_enumerator_names)
	{
		retval += s + ", ";
	}
}
