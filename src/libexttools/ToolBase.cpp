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
#include <cstdio>
#include <sys/wait.h>
#include <glob.h>

#include <iostream>
#include <sstream>
#include <boost/regex.hpp>

#include "ToolBase.h"

ToolBase::ToolBase()
{
}

ToolBase::ToolBase(const ToolBase& orig)
{
}

ToolBase::~ToolBase()
{
}

int ToolBase::System(const std::string &params) const
{
	// Invoke the command processor, which will invoke the command.
	return ::system((m_cmd + " " + params).c_str());
}

bool ToolBase::Popen(const std::string &params, std::ostream &progs_stdout) const
{
	FILE *progs_stdout_fp;
	int status;
	// Arbitrarily picked a buffer size for fread()'s.
	const size_t buff_size = 256;
	unsigned char read_buffer[buff_size];
	size_t bytes_read;
	
	// Run the command in the specified working directory.
	progs_stdout_fp = popen((m_cmd + " " + params).c_str(), "r");
	
	if(progs_stdout_fp == NULL)
	{
		// There was some sort of error.
		std::cerr << "ERROR: popen() failed." << std::endl;
		return false;
	}
	
	// Read the text we got from the popen()'ed process and stream it to progs_stdout.
	while (bytes_read = fread(read_buffer, 1, buff_size, progs_stdout_fp), bytes_read > 0)
	{
		progs_stdout << std::string(reinterpret_cast<char*>(&(read_buffer[0])), bytes_read);
	}
	
	// Close the popen()'ed file pointer.
	status = pclose(progs_stdout_fp);

	if(status == -1)
	{
		// pclose() error.
		std::cerr << "ERROR: pclose() returned -1" << std::endl;
		return false;
	}
	else
	{
		// pclose() call was OK, let's see if the process we popen()'ed terminated
		// normally or if something failed.
		if(WIFEXITED(status))
		{
			// Child process exited normally, get its return value.
			int child_retval = WEXITSTATUS(status);
			//std::clog << "INFO: Popen()'ed process returned " << child_retval << std::endl;
			/// @todo Probably want to make handling of status more robust, differentiate between popen()
			///       failures and failures of what was popen()'ed..
			if(child_retval == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

std::vector< std::string > ToolBase::Glob(const std::string &pattern)
{
	// Find the matching filenames.
	
	// Buffer to store the globbing results.
	glob_t globbuf;
	int glob_retval;
	std::vector< std::string > retval;

	glob_retval = glob(pattern.c_str(), GLOB_NOSORT, NULL, &globbuf);
	switch(glob_retval)
	{
		case 0:
			// Found something.
			// Get the matched filenames.
			for(size_t i=0; i!=globbuf.gl_pathc; i++)
			{
				retval.push_back(globbuf.gl_pathv[i]);
			}
			break;
		case GLOB_NOMATCH:
			std::cerr << "INFO: No match to filename pattern \""
					<< pattern
					<< "\"" << std::endl;
			break;
		case GLOB_NOSPACE:
		case GLOB_ABORTED:
		default:
			std::cerr << "ERROR: glob() failed attempting to match filename pattern \""
					<< pattern
					<< "\"" << std::endl;
			break;

	}
	globfree(&globbuf);
	
	return retval;
}

std::string ToolBase::Mktemp(const std::string &filename_template, bool directory, bool rooted_in_tmp)
{
	// Copy the template string to a char array which mk{s,d}temp can work with.
	char *c_template = new char[filename_template.size()+1]();
	std::copy(filename_template.begin(), filename_template.end(),c_template);
	
	if(directory)
	{
		// Create a directory.
		if(mkdtemp(c_template) == NULL)
		{
			/// @todo Handle error.
		}
	}
	else
	{
		// Create a file.
		int fd;
		fd = mkstemp(c_template);
		if(fd == -1)
		{
			/// @todo Handle error.
		}
		close(fd);
	}
	
	// Copy the newly-created file/dir name string to a std::string for return.
	std::string retval = c_template;
	
	delete [] c_template;
	
	return retval;
}

VersionNumber ToolBase::GetVersion() const
{
	std::stringstream ss;
	boost::regex version_regex(GetVersionExtractionRegex());
	boost::cmatch capture_results;
	std::string line;
	
	// Run the program's "print version" command.
	/// @todo Handle errors.
	Popen(GetVersionOptionString(), ss);
	
	// Look for and extract the version text.
	// In reality we probably never need to look beyond the first line, but just to
	// be safe and thorough we'll check them all if we have to.
	ss.seekg(0, std::stringstream::beg);
	while(ss.good())
	{
		// Get the next line.
		std::getline(ss, line);
		
		// Try to extract the version text.
		if(boost::regex_match(line.c_str(), capture_results, version_regex))
		{
			// Set the VersionNumber object.
			m_version_number.Set(capture_results[1].str());
			return m_version_number;
		}
	}
	
	// If we get here, we couldn't find the version info.
	return VersionNumber();
}
