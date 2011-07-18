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

/** @file */

#include <algorithm>
#include <sstream>

#include "VersionNumber.h"

VersionNumber::VersionNumber(const std::string &version_string)
{
	// We'll use stringstream to parse the integers out of version_string.
	std::istringstream parser(version_string);
	int i = 0;
	int digit;
	
	while(parser.good())
	{
		// Get the next integer.
		parser >> digit;
		m_version_digits.push_back(digit);
		// Skip the period.
		parser.get();
		i++;
	}
}

VersionNumber::VersionNumber(const VersionNumber& orig) { }

VersionNumber::~VersionNumber() { }

bool VersionNumber::operator==(const VersionNumber &other) const
{
	return std::equal(m_version_digits.begin(), m_version_digits.end(), other.m_version_digits.begin());
}

bool VersionNumber::operator<(const VersionNumber &other) const
{
	return std::lexicographical_compare(m_version_digits.begin(), m_version_digits.end(),
									 other.m_version_digits.begin(), other.m_version_digits.end());
}
