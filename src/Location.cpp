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

#include "Location.h"

Location::Location(boost::filesystem::path path, long lineno, long column /* = 0 */)
{
	m_file_path = path;
	m_line_number = lineno;
	m_column = column;
}

Location::Location(const Location& orig)
{
}

Location::~Location()
{
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
    os << loc.m_file_path << ":" << loc.m_line_number;
    return os;
}
