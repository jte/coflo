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

#ifndef IF_H
#define	IF_H

#include "FlowControlBase.h"

#include <string>

/**
 * Class representing a 2-option (true/false) decision vertex.
 */
class If : public FlowControlBase
{
public:
	If(const Location &location, const std::string &condition);
	If(const If& orig);
	virtual ~If();
	
	virtual std::string GetStatementTextDOT() const { return "if(" + m_condition + ")"; };
	
	virtual std::string GetIdentifierCFG() const { return "if(" + m_condition + ")"; };
	
	virtual std::string GetShapeTextDOT() const { return "diamond"; };
	
	virtual bool IsDecisionStatement() const { return true; };

private:

	std::string m_condition;
};

#endif	/* IF_H */

