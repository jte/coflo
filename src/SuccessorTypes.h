/**
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

#ifndef SUCCESSORTYPES_H
#define	SUCCESSORTYPES_H

#include "Successor.h"

/// Macro for easily declaring the multiple subclasses.
#define M_DECLARE_SUCCESSOR_SUBCLASS(s, indent) \
class Successor##s : public Successor \
{ \
public: \
    Successor##s(long block_number) : Successor(block_number) { }; \
    Successor##s(const std::string &block_number_as_string) : Successor(block_number_as_string) {}; \
    Successor##s(const Successor##s& orig) : Successor(orig) {}; \
    virtual ~Successor##s() {}; \
	/** Returns text suitable for labelling the edge. */ \
	virtual std::string GetSuccessorText() const { return std::string(#s); }; \
	virtual bool GetIndent() const { return indent; }; \
}

/// \name The various types of successors
//@{
M_DECLARE_SUCCESSOR_SUBCLASS(Exit, false);
M_DECLARE_SUCCESSOR_SUBCLASS(Fallthru, false);
M_DECLARE_SUCCESSOR_SUBCLASS(True, true);
M_DECLARE_SUCCESSOR_SUBCLASS(False, true);
M_DECLARE_SUCCESSOR_SUBCLASS(Undecorated, true);
//@]

#endif	/* SUCCESSORTYPES_H */

