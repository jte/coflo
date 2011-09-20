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

#include "ResponseFileParser.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>


ResponseFileParser::ResponseFileParser()
{
	// TODO Auto-generated constructor stub

}

ResponseFileParser::~ResponseFileParser()
{
	// TODO Auto-generated destructor stub
}

void ResponseFileParser::Parse(const std::string & filename, std::vector<std::string> *args)
{
	// Load the file and tokenize it.
	std::ifstream ifs(filename.c_str());
	if (!ifs)
	{
		std::cerr << "ERROR: Could not open response file \"" << filename << "\"." << std::endl;

		/// @todo Handle error, probably throw.
		return;
	}

	std::vector<std::string> response_file_lines;

	// Read in the lines of the file.
	LoadFileIntoVectorOfStrings(ifs, &response_file_lines);

	// Split the file content
	boost::escaped_list_separator<char> sep("","\t \r\n","\"\'");

	BOOST_FOREACH(std::string s, response_file_lines)
	{
		boost::tokenizer<boost::escaped_list_separator<char> > tok(s, sep);

		std::string token;
		BOOST_FOREACH(token, tok)
		{
			// Skip token if it's all space.
			/// @todo We actually shouldn't be skipping these, there are valid reasons why someone might put a "" on the command line.
			size_t char_index = token.find_first_not_of("\t \r\n");
			if(char_index == std::string::npos)
			{
				// It's a blank line.
				continue;
			}

			args->push_back(token);
		}
	}
}

void ResponseFileParser::LoadFileIntoVectorOfStrings(std::ifstream &ifs, std::vector<std::string> *vec_of_str)
{
	std::string line;
	while (getline(ifs, line))
	{
		size_t char_index;

		// Find the first non-whitespace character.
		char_index = line.find_first_not_of("\t \r\n");

		// Check for blank lines.
		if(char_index == std::string::npos)
		{
			// It's a blank line.
			continue;
		}

		// Check for comment lines.
		if(line[char_index] == ';')
		{
			// It's a comment, skip it.
			continue;
		}

		// Otherwise it's something we should parse as a command line option.
		vec_of_str->push_back(line);
	}
}
