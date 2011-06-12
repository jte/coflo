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

/*
 * GCC file paths for "gcc -S -fdump-tree-cfg-lineno-blocks <file>.c":
 * - Source path: relative to working directory.
 * - Output <file>.c.013t.cfg:
 *		- 4.3.4: Always in working directory.
 *		- i686-w64-mingw32-gcc (GCC) 4.5.2: In same directory as <file>.s
 * - Output <file>.s:
 *		- In working directory by default.
 *		- If "-o file.s", then relative to working directory.
 */

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

ToolCompiler::ToolCompiler(const ToolCompiler& orig) : ToolBase(orig)
{
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
	System(std::string("--version > ") + temp_filename);
	
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

int ToolCompiler::GenerateCFG(const std::string &params)
{
	// Create the compile command.
	std::string compile_to_cfg_command;
	
	// Note that the "-blocks" option is required to make both gcc 4.3.4 and 4.4.1 emit
	// the same BLOCK/PRED/SUCC notations in the .cfg file (4.3.4 does it without -blocks).
	compile_to_cfg_command = " -S -fdump-tree-cfg-lineno-blocks";
	
	std::cout << "Compiling with " << params << "..." << std::endl;
	
	return System(compile_to_cfg_command+params);
}
