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

	// Read the whole file into a string
	/// @todo Possibly lock the file as FILE_SHARE_READ while doing this?
	/// @todo Add comment functionality.
	std::stringstream ss;
	ss << ifs.rdbuf();

	// Split the file content
	boost::char_separator<char> sep(" \n\r");
	std::string ResponsefileContents( ss.str() );
	boost::tokenizer<boost::char_separator<char> > tok(ResponsefileContents, sep);

	// Insert the options we've found into the args list.
	copy(tok.begin(), tok.end(), back_inserter(*args));
}



