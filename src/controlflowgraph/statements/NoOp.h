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

#ifndef NOOP_H
#define	NOOP_H

#include "PseudoStatement.h"

/**
 * NoOp statement.  Used primarily to give basic blocks which have no actual Statements
 * at least one Statement, so that graph manipulations can work while retaining these
 * "empty" blocks.
 */
class NoOp : public PseudoStatement
{
public:
	NoOp(const Location &location);
	NoOp(const NoOp& orig);
	virtual ~NoOp();
	
	virtual std::string GetStatementTextDOT() const { return "UNKNOWN"; };

	virtual std::string GetIdentifierCFG() const { return "UNKNOWN"; };
	
private:

};

#endif	/* NOOP_H */

