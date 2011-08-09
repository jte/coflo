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

#ifndef ENTRY_H
#define	ENTRY_H

#include "PseudoStatement.h"

class Entry : public PseudoStatement
{

public:
	Entry(const Location &location);
	Entry(const Entry& orig);
	virtual ~Entry();
	
	virtual std::string GetStatementTextDOT() const;
	
	virtual std::string GetIdentifierCFG() const { return "ENTRY"; };
	
private:

};

#endif	/* ENTRY_H */

