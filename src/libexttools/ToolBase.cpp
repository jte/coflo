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
	std::string cwd_cmd;
	
	// Were we given a working directory?
	if(!m_working_directory.empty())
	{
		// We were, create a "cd" commnad to switch to it.
		cwd_cmd = "cd " + m_working_directory + " && ";
	}
	
	// Invoke the command processor, which will invoke the command.
	return ::system((cwd_cmd + m_cmd + " " + params).c_str());
}

void ToolBase::SetWorkingDirectory(const std::string &working_directory)
{
	// Save the working directory for use later.
	m_working_directory = working_directory;
}
