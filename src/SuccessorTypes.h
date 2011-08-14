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

#ifndef SUCCESSORTYPES_H
#define	SUCCESSORTYPES_H

#include "Successor.h"

/**
 * \class Successor
 * 
 * Class used for edge types in the Basic Block CFG.
 */

/// Macro for easily declaring the multiple subclasses.
#define M_DECLARE_SUCCESSOR_SUBCLASS(s, indent, has_edge_label) \
class Successor##s : public Successor \
{ \
public: \
    Successor##s(long block_number) : Successor(block_number) { }; \
    Successor##s(const std::string &block_number_as_string) : Successor(block_number_as_string) {}; \
    Successor##s(const Successor##s& orig) : Successor(orig) {}; \
    virtual ~Successor##s() {}; \
	/** Returns true if this edge has a label. */ \
	virtual bool HasEdgeLabel() const { return has_edge_label; }; \
	/** Returns text suitable for labelling the edge. */ \
	virtual std::string GetEdgeLabel() const { return std::string(#s); }; \
	virtual bool GetIndent() const { return indent; }; \
}

/// @name The various types of successors.
//@{
M_DECLARE_SUCCESSOR_SUBCLASS(Exit, false, false);
M_DECLARE_SUCCESSOR_SUBCLASS(Fallthru, false, false);
M_DECLARE_SUCCESSOR_SUBCLASS(True, true, true);
M_DECLARE_SUCCESSOR_SUBCLASS(False, true, true);
M_DECLARE_SUCCESSOR_SUBCLASS(Undecorated, true, false);
M_DECLARE_SUCCESSOR_SUBCLASS(NoReturn, false, true);
//@]

#endif	/* SUCCESSORTYPES_H */

