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
 
#include "Block.h"

#include <string>
#include <iostream>
#include <iomanip>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include "SuccessorTypes.h"

using namespace boost;

Block::Block(Function * parent_function, long block_number, long block_starting_line_in_src)
{
	m_parent_function = parent_function;
	m_block_number = block_number;
}
 
Block::~Block()
{
}
 
void Block::AddFunctionCall(const std::string &function_name)
{
	m_function_calls.push_back(function_name);
}

void Block::AddSuccessors(std::string successors_string)
{
	static const boost::regex is_exit("EXIT[[:space:]]*(.*)");
	static const boost::regex is_fallthru("([[:digit:]]+)[[:space:]]*\\(fallthru\\)[[:space:]]*(.*)");
	static const boost::regex is_true("([[:digit:]]+)[[:space:]]*\\(true\\)[[:space:]]*(.*)");
	static const boost::regex is_false("([[:digit:]]+)[[:space:]]*\\(false\\)[[:space:]]*(.*)");
	static const boost::regex is_undecorated("([[:digit:]]+)[[:space:]]*(.*)");

	boost::cmatch match_results;
	 
	while(!successors_string.empty())
	{
		// Pick out the successor entries.

		if(regex_match(successors_string.c_str(), match_results, is_exit))
		{
			// Add EXIT (i.e. exit function) link to list.
			m_successor_list.push_back(new SuccessorExit(1));

			// Consume the substring we just parsed.
			successors_string = match_results[1];
		}
		else if(regex_match(successors_string.c_str(), match_results, is_fallthru))
		{
			// Add a Fallthru link to list.
			m_successor_list.push_back(new SuccessorFallthru(match_results[1]));

			// Consume the substring we just parsed.
			successors_string = match_results[2];
		}
		else if(regex_match(successors_string.c_str(), match_results, is_true))
		{
			// Add a Fallthru link to list.
			m_successor_list.push_back(new SuccessorTrue(match_results[1]));

			// Consume the substring we just parsed.
			successors_string = match_results[2];
		}
		else if(regex_match(successors_string.c_str(), match_results, is_false))
		{
			// Add a Fallthru link to list.
			m_successor_list.push_back(new SuccessorFalse(match_results[1]));

			// Consume the substring we just parsed.
			successors_string = match_results[2];
		}
		else if(regex_match(successors_string.c_str(), match_results, is_undecorated))
		{
			// Add a Fallthru link to list.
			m_successor_list.push_back(new SuccessorUndecorated(match_results[1]));

			// Consume the substring we just parsed.
			successors_string = match_results[2];
		}
		else
		{
			// Couldn't match any type of successor we know about, bail out of
			// this while loop.
			std::cerr << "WARNING: Unknown successor type in: \"";
			std::cerr << successors_string << "\"" << std::endl;
			break;
		}
	}
}

Successor* Block::GetSuccessorAtIndex(std::vector< Successor * >::size_type index) const
{
	if( index < m_successor_list.size())
	{
		return m_successor_list[index];
	}
	else
	{
		return NULL;
	}
}

void Block::PrintBlock(long indent_level)
{
	// Print out all function calls in this block.
	BOOST_FOREACH(std::string fp, m_function_calls)
	{
		std::cout << std::setfill('-') << std::setw(indent_level)<< "-" << fp << std::endl;
	}

	// Print successors.
	BOOST_FOREACH(Successor *s, m_successor_list)
	{
		Block *bp;

		bp = s->GetSuccessorBlockPtr();

		std::cout << std::setfill('-') << std::setw(indent_level)<< "-" << s->GetSuccessorText() << std::endl;

		if(bp == NULL)
		{
			//std::cerr << "ERROR: Found Successor with NULL BlockPtr." << std::endl;
		}
		else
		{
			//std::cerr << "INFO: Printing block " << bp->GetBlockNumber() << std::endl;
			// See if we should indent.
			bp->PrintBlock(indent_level + static_cast<long>(s->GetIndent()));
		}
	}
}
