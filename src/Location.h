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

#ifndef LOCATION_H
#define	LOCATION_H

#include <string>
#include <sstream>
#include <iostream>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

/**
 * Class which encapsulates a location in a source file, generally an identifier.
 */
class Location
{
public:
	Location(const std::string &location_string);
	Location(boost::filesystem::path path, long lineno, long column = 0);
	Location(const Location& orig);
	virtual ~Location();
	
	//std::string GetLineNumber() const { std::stringstream ss; ss << m_line_number; return ss.str(); };
	
	friend std::ostream& operator<<(std::ostream& os, const Location& loc);
	
private:
	
	/// The file containing the location.
	boost::filesystem::path m_file_path;
	
	/// The line number of the location.
	long m_line_number;
	
	/// The column of the location.
	long m_column;
};

#endif	/* LOCATION_H */
