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

#include "Location.h"

// Include the necessary Boost libraries.
#include <boost/regex.hpp>

/// Regex string for matching and capturing locations.
/// Capture 1 is the path, 2 is the line number, and 3 is the possibly-missing column number.
static const boost::regex f_location_expression("\\[([^\\]]*?)[[:space:]]\\:[[:space:]]([[:digit:]]+)(?:[[:space:]]?\\:[[:space:]]?([[:digit:]]+))?\\]");

Location::Location(const std::string &location_string)
{
	boost::cmatch capture_results;
	if(boost::regex_match(location_string.c_str(), capture_results, f_location_expression))
	{
		m_passed_file_path = capture_results[1].str();
		/// @todo Make this really determine the abs path.
		m_absolute_file_path = m_passed_file_path;
		m_line_number = atoi(capture_results[2].str().c_str());
		if(capture_results[3].matched)
		{
			m_column = atoi(capture_results[3].str().c_str());
		}
		else
		{
			m_column = -1;
		}
	}
	else
	{
		std::cerr << "WARNING: UNPARSABLE LOCATION: " << location_string << std::endl;
	}
}

Location::Location(const Location& orig)
{
	DeepCopy(orig);
}

Location::~Location()
{
}

Location Location::operator=(const Location &other)
{
	// Check for self-assignment.
	if(this == &other)
	{
		// This was an attempt to assign to ourself.  Just return this instance.
		return *this;
	}

	// Otherwise do the copy.
	DeepCopy(other);

	return *this;
}


std::string Location::GetPassedFilePath() const
{
	return m_passed_file_path;
}

std::string Location::GetAbsoluteFilePath() const
{
	return m_absolute_file_path;
}

/**
 * Insertion operator for Location.
 * 
 * @param os  Stream to insert location info into.
 * @param loc Reference to the Location object.
 * @return 
 */
std::ostream& operator<<(std::ostream& os, const Location& loc)
{
    os << loc.m_passed_file_path << ":" << loc.m_line_number;
	if(loc.m_column != -1)
	{
		os << ":" << loc.m_column;
	}
    return os;
}

void Location::DeepCopy(const Location &orig)
{
	// Do a deep (vs. the default shallow) copy of the object.
	m_passed_file_path = orig.m_passed_file_path;
	m_absolute_file_path = orig.m_absolute_file_path;
	m_line_number = orig.m_line_number;
	m_column = orig.m_column;
}
