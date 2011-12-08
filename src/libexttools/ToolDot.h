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

#ifndef TOOLDOT_H
#define	TOOLDOT_H

#include "ToolBase.h"

#include <string>

class ToolDot : public ToolBase
{
public:
	ToolDot(const std::string &cmd);
	ToolDot(const ToolDot& orig);
	virtual ~ToolDot();
	
	bool CompileDotToPNG(const std::string &dot_filename, const std::string &output_filename) const;
	
protected:
	
	virtual std::string GetVersionOptionString() const { return "-V 2>&1"; };
	virtual std::string GetVersionExtractionRegex() const { return ".*?([\\d\\.]+).*"; };

};

#endif	/* TOOLDOT_H */

