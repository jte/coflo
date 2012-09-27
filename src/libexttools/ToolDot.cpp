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

#include <iostream>
#include <sstream>

#include "ToolDot.h"

const char* ToolDot::format_strings[] = { "svg", "png" };

ToolDot::ToolDot(const std::string &cmd)
{
	SetCommand(cmd);
}

ToolDot::ToolDot(const ToolDot& orig) : ToolBase(orig) { }

ToolDot::~ToolDot() { }

bool ToolDot::CompileDotToPNG(const std::string &dot_filename, const std::string &output_filename,
		OUTPUT_FORMAT_DOT output_format) const
{
	System((" -o" + output_filename + " -T" + format_strings[output_format] + " "+ dot_filename).c_str());
	
	/// @todo Handle errors.
	return true;
}
