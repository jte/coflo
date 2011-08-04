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
 
#include "Block.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "debug/debug_utils.hpp"

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include "Location.h"
#include "SuccessorTypes.h"
#include "statements/Statement.h"
#include "statements/NoOp.h"
#include "statements/Entry.h"
#include "statements/Exit.h"
#include "statements/Placeholder.h"

using namespace boost;

// Regex for finding a "# SUCC:" statement, which ends the current block.
// Capture 1 is the string of successors, which will be further parsed by the Block object itself.
static const boost::regex f_successor_expression("[[:space:]]+# SUCC\\:[[:space:]]*(.*)");

static const boost::regex f_pred_expression("[[:space:]]+# PRED\\:.*");


Block::Block(Function * parent_function, long block_number, long block_starting_line_in_src)
{
	m_parent_function = parent_function;
	m_block_number = block_number;
	m_block_starting_line_in_src = block_starting_line_in_src;
	
	// Blocks 0 and 1 are special cases: the ENTRY and EXIT blocks, resp.
	// We have to treat them a bit differently than the other blocks.
	// Both are not actually created from text in the source file, but are implicit.
	// Therefore there are no actual blocks they're created from, so things like the
	// line number have to be stubbed in.
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
			std::stringstream oss;
			// Make the label the block number.
			oss << m_block_number;
			m_block_label = oss.str();
			break;
		}
	}
}
 
Block::~Block()
{
}

bool Block::Parse(std::istream &input_stream)
{
	std::string line;
	
	while(input_stream.good())
	{
		// Get the next line of input.
		std::getline(input_stream, line);
		//dout << "LINE=" << line << std::endl;
		
		boost::cmatch capture_results;
		
		// Check for and ignore comments.
		if(line[0] == ';')
		{
			continue;
		}
		
		if(regex_match(line.c_str(), capture_results, f_pred_expression))
		{
			// Found the "#PRED:" expression, skip for now (maybe we'll use this some time in the future).
			//std::cout << "SKIPPING PRED=" << line << std::endl;
			break;
		}
	}
	
	// Look for Statements.
	StatementBase *next_statement;
	do
	{
		next_statement = StatementBase::Parse(input_stream);

		if(next_statement != NULL)
		{
			// Add the statement to the block's statement list.
			AddStatement(next_statement);
		}
	} while (next_statement != NULL);
		
	while(input_stream.good())
	{
		// Get the next line of input.
		std::getline(input_stream, line);
		//std::cout << "LINE=" << line << std::endl;
		
		boost::cmatch capture_results;
		
		// Check for and ignore comments.
		if(line[0] == ';')
		{
			continue;
		}
		
		// Look for block ends.
		if(regex_match(line.c_str(), capture_results, f_successor_expression))
		{
			//std::clog << "Found \"SUCC:\" line: \"" << capture_results[1] << "\", ending block "
			//	<< GetBlockNumber() << ", with "
			//	<< NumberOfStatements() << " statements." << std::endl;

			if(NumberOfStatements() == 0)
			{
				// Make sure every block has at least one statement.
				std::stringstream oss;
				oss << "[UNKNOWN/file.c : " << GetBlockStartingLineNo() << "]";
				AddStatement(new Placeholder(new Location(oss.str())));
			}

			// Parse the block's successors.
			AddSuccessors(capture_results[1]);

			return true;
		} 
	}

	// Got through the entire file but didn't find a block end, something's wrong.
	std::cerr << "ERROR: Couldn't find block end" << std::endl;
	return false;
}
 
void Block::AddStatement(StatementBase *statement)
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
	
	if(successors_string.empty())
	{
		// There are no successors listed.  This means that this is a No-Return block.
		// Add a NoReturn edge to the EXIT block.
		std::cerr << "WARNING: EMPTY SUCCESSOR LIST" << std::endl;
		m_successor_list.push_back(new SuccessorNoReturn(1));
	}
	
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
	BOOST_FOREACH(StatementBase *sp, m_statement_list)
	{
		indent(indent_level);
		std::cout << sp->GetStatementTextDOT() << std::endl;
	}
}
