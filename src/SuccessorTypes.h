/**
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of coflo.
 *
 * coflo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * coflo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * coflo.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SUCCESSORTYPES_H
#define	SUCCESSORTYPES_H

#include "Successor.h"

/// Macro for easily declaring the multiple subclasses.
#define M_DECLARE_SUCCESSOR_SUBCLASS(s) \
class Successor##s : public Successor \
{ \
public: \
    Successor##s(long block_number) : Successor(block_number) { }; \
    Successor##s(const std::string &block_number_as_string) : Successor(block_number_as_string) {}; \
    Successor##s(const Successor##s& orig) : Successor(orig) {}; \
    virtual ~Successor##s() {}; \
}

/// \name The various types of successors
//@{
M_DECLARE_SUCCESSOR_SUBCLASS(Exit);
M_DECLARE_SUCCESSOR_SUBCLASS(Fallthru);
M_DECLARE_SUCCESSOR_SUBCLASS(True);
M_DECLARE_SUCCESSOR_SUBCLASS(False);
M_DECLARE_SUCCESSOR_SUBCLASS(Undecorated);
//@]

#endif	/* SUCCESSORTYPES_H */

