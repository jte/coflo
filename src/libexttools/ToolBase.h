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

/**
 * Base class for external tools we'll invoke.
 */
class ToolBase
{

public:
	ToolBase();
	ToolBase(const ToolBase& orig);
	virtual ~ToolBase();
	
	void SetCommand(const std::string &cmd) { m_cmd = cmd; };
	
	virtual std::string GetVersion() const = 0;
	
protected:
	
	/// The filename of the program.
	std::string m_cmd;
		
	/**
	 * Wrapper around the system() call.
	 * 
     * @param params Parameters to give the command.  Note that you should not
	 *	specify the command itself - m_cmd will be prepended automatically.
     * @return 
     */
	int System(const std::string &params) const;

};

#endif	/* TOOLBASE_H */

