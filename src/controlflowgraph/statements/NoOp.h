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
 * NoOp statement.  Used primarily to as a statement type for statements that aren't relevant to the CFG
 * structure or analysis, such as the "Comment statements" emitted by GCC 4.5.3 of the form:
 * "[<location>] // predicted unlikely by continue predictor.".
 */
class NoOp : public PseudoStatement
{
public:
	NoOp(const Location &location);
	NoOp(const NoOp& orig);
	virtual ~NoOp();
	
	virtual std::string GetStatementTextDOT() const { return "NoOp"; };

	virtual std::string GetIdentifierCFG() const { return "NoOp"; };
	
private:

};

#endif	/* NOOP_H */

