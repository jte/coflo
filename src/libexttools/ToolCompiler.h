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

#ifndef TOOLCOMPILER_H
#define	TOOLCOMPILER_H

#include <utility>

#include "ToolBase.h"

/**
 * Class which wraps the compiler executable.
 */
class ToolCompiler : public ToolBase
{

public:
	ToolCompiler(const std::string &cmd);
	ToolCompiler(const ToolCompiler& orig);
	virtual ~ToolCompiler();
	
	/**
	 * Parse the specified @a source_filename and generate the file containing the SSA representation.
	 * The resulting file will be named <source_filename>.coflo.cfg.
	 *
	 * @param params
	 * @param source_filename
	 * @return
	 */
	int GenerateCFG(const std::string &params, const std::string &source_filename);
	
	std::pair< std::string, bool > CheckIfVersionIsUsable() const;
	
protected:
	
	virtual std::string GetVersionOptionString() const { return "-dumpversion"; };
	virtual std::string GetVersionExtractionRegex() const { return "([\\d\\.]+).*"; };
	
private:

};

#endif	/* TOOLCOMPILER_H */

