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
 
#include <iostream>
#include <map>
#include <typeinfo>

#include <boost/foreach.hpp>

#include "Block.h"

#include "Function.h"
#include "SuccessorTypes.h"

typedef std::map< long, Block * > T_LINK_MAP;
typedef T_LINK_MAP::iterator T_LINK_MAP_ITERATOR;
typedef T_LINK_MAP::value_type T_LINK_MAP_VALUE;
 
Function::Function(const std::string &function_id)
{
	m_function_id = function_id;
	m_entry_block = NULL;
}
 
Function::~Function()
{
}
 
void Function::AddBlock(Block *block)
{
	m_block_list.push_back(block);
}

void Function::LinkBlocks()
{
	// A map of the blocks constituting this function.
	T_LINK_MAP linkmap;
	std::vector< Successor* > successor_list;

	// Go through all the blocks and add them to the link map.
	BOOST_FOREACH(Block *bp, m_block_list)
	{
		// Add this block to the number->Block* map.
		linkmap[bp->GetBlockNumber()] = bp;

		// Collect all the blocks' Successors into a list.
		long i;
		Successor *s;
		for(i=0; s = bp->GetSuccessorAtIndex(i), s != NULL; i++)
		{
			if(NULL != dynamic_cast<SuccessorExit*>(s))
			{
				std::cout << "INFO: Found EXIT successor." << std::endl;
				s->SetSuccessorBlockPtr(NULL);
			}
			else
			{
				successor_list.push_back(s);
			}
		}
	}

	// Block 2 appears to always be the entry block, set it.
	/// \todo Check for "PRED: ENTRY" to make sure of this.
	T_LINK_MAP_ITERATOR lmit = linkmap.find(2);
	if(lmit == linkmap.end())
	{
		std::cerr << "ERROR: Can't find Block 2." << std::endl;
	}
	m_entry_block = lmit->second;

	// Now go through all the Successors and link them to the Blocks they refer to.
	std::vector< Successor* >::iterator it2;
	for(it2 = successor_list.begin(); it2 != successor_list.end(); it2++)
	{
		long block_no;
		Block *block_ptr;
		T_LINK_MAP_ITERATOR linkmap_it;

		block_no = (*it2)->GetSuccessorBlockNumber();

		// Look up the block in the map.
		linkmap_it = linkmap.find(block_no);

		if(linkmap_it == linkmap.end())
		{
			std::cerr << "ERROR: Can't find Block " << block_no << " in linkmap." << std::endl;
		}
		else if(linkmap_it->second == NULL)
		{
			std::cerr << "ERROR: Found a Successor with no Block pointer." << std::endl;
		}
		else
		{
			// Found the referenced block, assign a pointer directly to it.
			(*it2)->SetSuccessorBlockPtr(linkmap_it->second);
		}
	}
}

void Function::Print()
{
	// Print the function info.
	std::cout << "Function Definition: " << m_function_id << std::endl;

	// Print the function's blocks.
	m_entry_block->PrintBlock(1);
}
