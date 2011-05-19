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
#include <sstream>
#include <iomanip>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include "Location.h"
#include "SuccessorTypes.h"
#include "Statement.h"
#include "NoOp.h"
#include "Entry.h"
#include "Exit.h"

using namespace boost;

Block::Block(Function * parent_function, long block_number, long block_starting_line_in_src)
{
	m_parent_function = parent_function;
	m_block_number = block_number;
	
	switch(block_number)
	{
		case 0:
		{
			// This is the entry block.
			m_block_label = "ENTRY";
			m_statement_list.push_back(new Entry(new Location("[UNKNOWN/file.c : 0]")));
			break;
		}
		case 1:
		{
			// This is the exit block.
			m_block_label = "EXIT";
			m_statement_list.push_back(new Exit(new Location("[UNKNOWN/file.c : 0]")));
			break;
		}
		default:
		{
			// Make the label the block number.
			std::stringstream oss;
			oss << m_block_number;
			m_block_label = oss.str();
			break;
		}
	}
}
 
Block::~Block()
{
}
 
void Block::AddStatement(Statement *statement)
{
	m_statement_list.push_back(statement);
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

static void indent(int level)
{
	while(level > 0)
	{
		std::cout << "    ";
		level--;
	}
}

void Block::PrintBlock(long indent_level)
{
	// Print the block number.
	indent(indent_level);
	std::cout << m_block_number << std::endl;
	
	// Print out all function calls in this block.
	BOOST_FOREACH(Statement *sp, m_statement_list)
	{
		indent(indent_level);
		std::cout << sp->GetStatementTextDOT() << std::endl;
	}
}
