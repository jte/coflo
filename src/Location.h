/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

/**
 * Class which encapsulates the location of an item in a source file.
 */
class Location
{
public:

	/**
	 * Default constructor, creates an "UNKNOWN" location.
	 */
	Location();

	/**
	 * Constructor which creates a Location object from a filename, line number, and optionally a column number.
	 *
	 * @param file_name The name of the file this Location refers to.
	 * @param line      The 1-based line number of the location.
	 * @param column    The optional 1-based column number of the location.
	 */
	Location(const std::string &file_name, long line, long column=-1);

	/**
	 * Constructor which takes a string with the following format as a parameter:
	 * "[filepath : line_no(:column_no)?]".
	 *
	 * Used mainly while parsing the output of gcc.
	 *
	 * @param location_string
	 */
	explicit Location(const std::string &location_string);

	/**
	 * Templated constructor primarily to allow conversion from DParser's d_loc_t type.
	 *
	 * @param other
	 */
	template <typename T>
	explicit Location(const T& other) :
		m_passed_file_path(other.pathname), m_line_number(other.line), m_column(other.col) {};

	/**
	 * Copy constructor.
	 *
	 * @param orig The Location object to copy.
	 */
	Location(const Location& orig);

	/**
	 * Destructor.
	 */
	virtual ~Location();
	
	/// @name Overloaded operators.
	///@{

	/**
	 * Assignment from another Location object.
	 * 
     * @param other
     * @return
     */
	const Location& operator=(const Location &other);
	///@}

	/// @name Accessors for various components of the Location.
	///@{

	/**
	 * Returns the path that was initially passed during the construction of this Location.
	 *
	 * @return The original path passed to the constructor.
	 */
	std::string GetPassedFilePath() const;
	
	std::string GetAbsoluteFilePath() const;
	
	long GetLineNumber() const { return m_line_number; };

	long GetColumn() const { return m_column; };

	///@}
	
	/**
	 * Returns the Location as a string formatted in the manner of the GNU recommendation for compilers
	 * here: <http://www.gnu.org/prep/standards/html_node/Errors.html#Errors>.
	 *
	 * @return The Location in the following format: source-file-name:lineno(:column)?.  Intention is that a ": message"
	 * would then be appended.
	 */
	std::string asGNUCompilerMessageLocation() const;

	/**
	 * Returns the Location as a string containing only the line number and column, separated by a colon.
	 *
	 * @return A string representing the line and column of the Location in the following format: lineno(:column)?.
	 */
	std::string asLineColumn() const;

	/**
	 * Stream insertion operator.  Inserts @a loc into stream @a os in the following format:
	 * "filepath : line_no(:column_no)?"
	 *
	 * @param os
	 * @param loc
	 * @return Reference to @a os.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Location& loc);
	
private:
	
	/**
	 * Do a deep (vs. the default shallow) copy of the object.
	 * Does the work for copy constructors and assignment operators.
	 *
     * @param other The other Location to copy into this one.
     */
	void DeepCopy(const Location &orig);

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
