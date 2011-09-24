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

#ifndef SUCCESSOR_H
#define	SUCCESSOR_H

#include <string>

class Block;

/**
 * Basic Block successor edge class.
 */
class Successor
{
public:
    Successor(long block_number);
    Successor(const std::string &block_number_as_string);
    Successor(const Successor& orig);
    virtual ~Successor();

	/** Returns true if this edge has a label. */
	virtual bool HasEdgeLabel() const = 0;
	
	/** Returns text suitable for labelling the edge. */
	virtual std::string GetEdgeLabel() const = 0;
	
	virtual bool GetIndent() const = 0;

	long GetSuccessorBlockNumber() const { return m_block_number; };

	Block *GetSuccessorBlockPtr() const { return m_block_ptr; };

	void SetSuccessorBlockPtr(Block *block) { m_block_ptr = block; };

private:

    /// The block number of the successor.
    long m_block_number;

	/// Pointer to the successor block.  Only valid after block-linking.
	Block *m_block_ptr;
};

#endif	/* SUCCESSOR_H */

