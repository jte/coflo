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
 *		- 4.3.4: Always in working directory, ignores -o directory.
 *		- i686-w64-mingw32-gcc (GCC) 4.5.2: In same directory as <file>.s
 * - Output <file>.c.012t.cfg:
 *		- 4.5.2: In same directory as <file>.s (i.e. follows -o).
 * - Output <file>.s:
 *		- In working directory by default.
 *		- If "-o file.s", then relative to working directory.
 *
 *	The code in GenerateCFG() normalizes this so that the .nnnt.cfg file:
 *	- Always has an extension of ".coflo.cfg".
 *	- Always is put in the current working directory.
 */

#include <cstdio>
#include <iostream>
#include <fstream>

// Tell <boost/filesystem.hpp> that we want the new interface.
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "ToolCompiler.h"

ToolCompiler::ToolCompiler(const std::string &cmd) 
{
	SetCommand(cmd);
}

ToolCompiler::ToolCompiler(const ToolCompiler& orig) : ToolBase(orig)
{
}

ToolCompiler::~ToolCompiler()
{
}

int ToolCompiler::GenerateCFG(const std::string &params, const std::string &source_filename)
{
	int system_retval;
	std::vector< std::string > matching_filenames;

	// Create the compile command.
	std::string compile_to_cfg_command;
	
	// Note that the "-blocks" option is required to make both gcc 4.3.4 and 4.4.1 emit
	// the same BLOCK/PRED/SUCC notations in the .cfg file (4.3.4 does it without -blocks).
	compile_to_cfg_command = " -S -fdump-tree-cfg-lineno-blocks";
	
	std::cout << "Running gcc with params: " << params << "..." << std::endl;
	
	// Call the compiler to generate the CFG file.
	system_retval = System(compile_to_cfg_command + params);

	if(system_retval != 0)
	{
		// Either the System command failed, or gcc did.  Propagate the error to the caller.
		return system_retval;
	}

	// Normalize the output filename by removing the three-digit compile stage number,
	// which can vary between gcc versions and builds.

	// Create a pattern to glob for.
	boost::filesystem::path source_filename_only = source_filename;
	source_filename_only = source_filename_only.filename();
	std::string filename_to_glob_for = source_filename_only.generic_string()+".????.cfg";
	matching_filenames = Glob(filename_to_glob_for);

	// Check for errors.
	if(matching_filenames.size() > 1)
	{
		// We matched more than one file, there must be some leftover CFG files.
		std::cerr << "ERROR: Matched " << matching_filenames.size() << " files with pattern \"" << filename_to_glob_for << "\", should only be one." << std::endl;
		return 1;
	}
	else if(matching_filenames.size() < 1)
	{
		// We couldn't match the file, something must have gone wrong.
		std::cerr << "ERROR: Couldn't match any files with pattern \"" << filename_to_glob_for << "\", should be one." << std::endl;
		return 1;
	}

	// We're OK, rename the file.
	rename(matching_filenames[0].c_str(), (source_filename_only.generic_string()+".coflo.cfg").c_str());
	//std::cout << matching_filenames.size() << " " << matching_filenames[0] << " " << filename_to_glob_for << std::endl;
	//exit (0);

	return 0;
}

std::pair< std::string, bool > ToolCompiler::CheckIfVersionIsUsable() const
{
	std::pair<std::string, bool> retval = std::make_pair(std::string("Ok"), true);
	
	// gcc 3.x.x doesn't support the "-fdump-tree-cfg" functionality.
	if(GetVersion() < VersionNumber("4.0.0"))
	{
		retval = std::make_pair(std::string("CoFlo requires a GCC version greater than 4.0.0"), false);
	}

	return retval;
}
