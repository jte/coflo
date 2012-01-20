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

/*
 * GCC file paths for "gcc -fno-builtin -S -fdump-tree-gimple-lineno <file>.c":
 * - Source path: relative to working directory.
 * - Output <file>.c.NNNt.gimple:
 *		- 4.3.4: Always in working directory, ignores -o directory.
 *		- i686-w64-mingw32-gcc (GCC) 4.5.2: In same directory as <file>.s
 * - Output <file>.c.NNNt.gimple:
 *		- 4.5.2: In same directory as <file>.s (i.e. follows -o).
 * - Output <file>.s:
 *		- In working directory by default.
 *		- If "-o file.s", then relative to working directory.
 *
 *	The code in GenerateCFG() normalizes this so that the .NNNt.gimple file:
 *	- Always has an extension of ".coflo.gimple".
 *	- Always is put in the current working directory.
 */

#include "ToolCompiler.h"

#include <cstdio>
#include <iostream>
#include <fstream>

// We don't need a "#define BOOST_FILESYSTEM_VERSION 3" here.  configure.ac sets that in AM_CPPFLAGS.
#include <boost/filesystem.hpp>


ToolCompiler::ToolCompiler(const std::string &cmd) 
{
	SetCommand(cmd);
}

ToolCompiler::ToolCompiler(const ToolCompiler& orig) : ToolBase(orig)
{
	// No members to copy.
}

ToolCompiler::~ToolCompiler()
{
}

int ToolCompiler::GenerateCFG(const std::string &params, const std::string &source_filename)
{
	int system_retval;
	int rename_retval;
	std::vector< std::string > matching_filenames;

	// Create the compile command.
	std::string compile_to_cfg_command;
	
	// Flags we're sending to GCC:
	// -fno-builtin = Don't silently use builtins for things like alloca, memcpy, etc.  This would make CoFlo's
	//                output harder to interpret.
	compile_to_cfg_command = " -fno-builtin -S -fdump-tree-gimple-lineno";
	
	// Call the compiler to generate the CFG file.
	system_retval = System(compile_to_cfg_command + params + " " + source_filename);

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
	std::string filename_to_glob_for = source_filename_only.generic_string()+".????.gimple";
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
	rename_retval = rename(matching_filenames[0].c_str(), (source_filename_only.generic_string()+".coflo.gimple").c_str());

	return rename_retval;
}

std::pair< std::string, bool > ToolCompiler::CheckIfVersionIsUsable() const
{
	std::pair<std::string, bool> retval = std::make_pair(std::string("Ok"), true);
	
	// gcc versions < 4.5.0 don't support the "-fdump-tree-gimple" functionality.
	if(GetVersion() < VersionNumber("4.5.0"))
	{
		retval = std::make_pair(std::string("CoFlo requires a GCC version greater than 4.5.0"), false);
	}

	return retval;
}
