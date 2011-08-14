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

#ifndef TOOLBASE_H
#define	TOOLBASE_H

#include <string>
#include <iosfwd>

#include "VersionNumber.h"

/**
 * Base class for wrappers around external executables we need to run.
 */
class ToolBase
{

public:
	ToolBase();
	ToolBase(const ToolBase& orig);
	virtual ~ToolBase();
	
	void SetCommand(const std::string &cmd) { m_cmd = cmd; };
	
	/**
	 * Returns the VersionNumber of the tool.
	 * 
	 * The version is generally obtained by running the tool with a "--version"
	 * parameter and parsing the output appropriately.
	 *
     * @return The VersionNumber of the tool.
     */
	VersionNumber GetVersion() const;
	
	/// @name Static utility functions.
	/// @todo Not sure if this is the best place for all (or any) of these.  May make sense to have these not
	///       connected to ToolBase at all.
	//@{
	
	/**
	 * C++ wrapper around POSIX glob().
	 * 
     * @param pattern The pattern to match.
     * @return A vector of matching filenames.
     */
	static std::vector< std::string > Glob(const std::string &pattern);
	
	/**
	 * A mktemp(1) work-alike.  Creates a temporary file or directory in a race-free
	 * manner and returns the name.
	 * 
     * @param filename_template
     * @param directory If true, create a directory instead of a file.
     * @param 
     * @return 
     */
	static std::string Mktemp(const std::string &filename_template = "", bool directory = false, bool rooted_in_tmp = true);
	
	//@}
	
protected:
	
	/**
	 * Overload this in derived classes to return the options given on the
	 * command line to cause the program to print its version info to stdout.
	 * May include sh-style stderr redirection if necessary.
	 * 
     * @return The parameters to pass to m_cmd to make it print its version info.
     */
	virtual std::string GetVersionOptionString() const = 0;
	
	/**
	 * Overload this in derived classes to return a regular expression which extracts
	 * the version number from a string in the version info output by the program
	 * when passed m_version_options.
	 * 
     * @return 
     */
	virtual std::string GetVersionExtractionRegex() const = 0;

		
	/**
	 * Wrapper around the system() call.
	 * 
     * @param params Parameters to give the command.  Note that you should not
	 *	specify the command itself - m_cmd will be prepended automatically.
     * @return Return value of the command.
     */
	int System(const std::string &params) const;
	
	/**
	 * Wrapper around popen("???", "r")/pclose().
	 * 
     * @param params
     * @param progs_stdout
     * @return 
     */
	bool Popen(const std::string &params, std::ostream &progs_stdout) const;
	
	/// The filename of the command.
	std::string m_cmd;
	
private:
	
	/// The options to pass to the program to make it print its version info to stdout.
	std::string m_version_options;
	
	/// The regular expression used to extract the actual version number from the version string.
	std::string m_version_regex;
	
	/// The version number of the tool.
	mutable VersionNumber m_version_number;

};

#endif	/* TOOLBASE_H */

