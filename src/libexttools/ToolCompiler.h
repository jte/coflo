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

class ToolCompiler : public ToolBase
{

public:
	ToolCompiler(const std::string &cmd);
	ToolCompiler(const ToolCompiler& orig);
	virtual ~ToolCompiler();
	
	int GenerateCFG(const std::string &params, const std::string &source_filename);
	
	std::pair< std::string, bool > CheckIfVersionIsUsable() const;
	
protected:
	
	virtual std::string GetVersionOptionString() const { return "--version"; };
	virtual std::string GetVersionExtractionRegex() const { return "[^[:space:]]+[[:space:]]\\(GCC\\)[[:space:]]([\\d\\.]+).*"; };
	
private:

};

#endif	/* TOOLCOMPILER_H */

