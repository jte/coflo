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

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <boost/regex.hpp>

#include "ToolCompiler.h"

/// Regex for extracting the version string.
static const boost::regex f_version_regex("[^[:space:]]+[[:space:]]\\(GCC\\)[[:space:]]([\\d\\.]+).*");

ToolCompiler::ToolCompiler(const std::string &cmd) 
{
	SetCommand(cmd);
}

ToolCompiler::ToolCompiler(const ToolCompiler& orig) 
{
	m_cmd = orig.m_cmd;
}

ToolCompiler::~ToolCompiler() { }

std::string ToolCompiler::GetVersion() const
{
	// Create a temp file to dump the version info into.
	char temp_filename[] = "/tmp/fileXXXXXX";
	int fd;
	std::ifstream input_file;
	std::string line;
	std::string retval("UNKNOWN");

	fd = mkstemp(temp_filename);
	
	// Invoke the compiler and redirect the version info to the file.
	::system((m_cmd + " --version > " + temp_filename).c_str());
	
	input_file.open(temp_filename, std::ifstream::in);
	if(input_file.good())
	{
		// Only need the first line of the file.
		std::getline(input_file, line);

		boost::cmatch capture_results;
		
		// Extract the version text.
		if(boost::regex_match(line.c_str(), capture_results, f_version_regex))
		{
			retval = capture_results[1];
		}
	}
	
	close(fd);

	return retval;
}
