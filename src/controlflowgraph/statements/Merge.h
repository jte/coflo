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

#ifndef MERGE_H
#define MERGE_H

#include "PseudoStatement.h"

/**
 * A Merge statement, the dual of the If statement.  Merges two branches together.
 */
class Merge: public PseudoStatement
{
public:
	Merge(const Location &location);
	Merge(const Merge &orig);
	virtual ~Merge();

	virtual std::string GetStatementTextDOT() const { return "MERGE"; };

	virtual std::string GetIdentifierCFG() const { return "MERGE"; };

	virtual std::string GetShapeTextDOT() const { return "diamond"; };
};

#endif /* MERGE_H */
