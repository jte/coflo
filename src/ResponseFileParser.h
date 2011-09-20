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

#ifndef RESPONSEFILEPARSER_H
#define RESPONSEFILEPARSER_H

#include <iosfwd>
#include <string>
#include <vector>

/**
 * Class which parses any "response files" specified on the command line.
 *
 * Response files are simply text files containing command line parameters exactly as they would appear on
 * the command line.
 */
class ResponseFileParser
{
public:
	ResponseFileParser();
	virtual ~ResponseFileParser();

	/**
	 * Parse the given @a filename for command line arguments.  Push the discovered arguments onto @a args.
	 *
	 * @param filename Name of the response file to parse.
	 * @param args Pointer to the vector of strings where we'll push the arguments we find.
	 */
	void Parse(const std::string &filename, std::vector<std::string> *args);

private:

	/**
	 * Read the entire file into a vector of strings to make things like single-line comments easier to remove.
	 *
	 * @param filename Name of the response file.
	 * @param vec_of_str Pointer to the vector of strings to populate with the lines of the file.
	 */
	void LoadFileIntoVectorOfStrings(std::ifstream &ifs, std::vector<std::string> *vec_of_str);
};

#endif /* RESPONSEFILEPARSER_H */
