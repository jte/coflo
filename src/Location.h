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

#ifndef LOCATION_H
#define	LOCATION_H

#include <string>
#include <sstream>
#include <iostream>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

/**
 * Class which encapsulates a location of something in a source file.
 */
class Location
{
public:
	Location(const std::string &location_string);
	Location(const Location& orig);
	virtual ~Location();
	
	std::string GetPassedFilePath() const;
	
	std::string GetAbsoluteFilePath() const;
	
	long GetLineNumber() const { return m_line_number; };
	
	friend std::ostream& operator<<(std::ostream& os, const Location& loc);
	
private:
	
	/// The file path we were given on construction.
	std::string m_passed_file_path;
	
	/// The absolute file path.
	std::string m_absolute_file_path;
	
	/// The line number of the location.
	long m_line_number;
	
	/// The column of the location.
	long m_column;
};

#endif	/* LOCATION_H */
