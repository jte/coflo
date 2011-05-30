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
/// Capture 1 is the path, 2 is the line number.
static const boost::regex f_location_expression("\\[(.*) \\: ([[:digit:]]+)\\]");

Location::Location(const std::string &location_string)
{
	boost::cmatch capture_results;
	if(boost::regex_match(location_string.c_str(), capture_results, f_location_expression))
	{
		m_passed_file_path = capture_results[1].str();
		/// @todo Make this really determine the abs path.
		m_absolute_file_path = m_passed_file_path;
		m_line_number = atoi(capture_results[2].str().c_str());
		m_column = 0;
	}
	else
	{
		std::cerr << "WARNING: UNPARSABLE LOCATION" << std::endl;
	}
}

Location::Location(const Location& orig)
{
	m_passed_file_path = orig.m_passed_file_path;
	m_absolute_file_path = orig.m_absolute_file_path;
	m_line_number = orig.m_line_number;
	m_column = orig.m_column;
}

Location::~Location()
{
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
 * @param os
 * @param dt
 * @return 
 */
std::ostream& operator<<(std::ostream& os, const Location& loc)
{
    os << loc.m_passed_file_path << ":" << loc.m_line_number;
    return os;
}
