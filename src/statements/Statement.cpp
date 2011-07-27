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

#include <string>
#include <boost/regex.hpp>

#include "Statement.h"
#include "If.h"
#include "Switch.h"
#include "FunctionCallUnresolved.h"
#include "../Location.h"

/// Regex string for matching and capturing locations.
/// Capture 1 is the path, 2 is the line number.
static const std::string f_location("(\\[[^\\]]*?[[:space:]]\\:[[:space:]][[:digit:]]+(?:[[:space:]]?\\:[[:space:]]?[[:digit:]]+)?\\])");

/// Regex to find function calls. Capture 1 is the file/line no, 2 is the called function identifier.
static const boost::regex f_function_call_expression(".+?"+f_location+" (?:.+? = )?([[:alpha:]_][[:alnum:]_]*) \\(.*\\);");

/// Regex to find if/else statements.  Capture 1 is the file/line no.
static const boost::regex f_if_expression(".+?"+f_location+" if \\(.*\\)");

/// Regex to find switch statements.  Capture 1 and 2 is the file/line no.
static const boost::regex f_switch_expression(".+?"+f_location+" switch \\(.*?\\)");

/// Regex to find non-"#WHATEVER" statements to break us out of the loop.
static const boost::regex f_succ_expression("[[:space:]]+#.*");

// Static members.

StatementBase* StatementBase::Parse(std::istream &input_stream)
{
	std::string line;
	std::ios::streampos start_pos;
	
	start_pos = input_stream.tellg();
	
	while(input_stream.good())
	{
		// Get the next line of input.
		std::getline(input_stream, line);
		boost::cmatch capture_results;
		
		// Check for and ignore comments.
		if(line[0] == ';')
		{
			start_pos = input_stream.tellg();
			continue;
		}

		// Look for if statements.
		if(boost::regex_match(line.c_str(), capture_results, f_if_expression))
		{
			// Add the if to the block.
			Location *loc = new Location(capture_results[1].str());
			std::cerr << "Found if at location: " << *loc << std::endl;
			If *if_stmnt = new If(loc);
			std::cout << "IF=" << line << std::endl;
			return if_stmnt;
		}

		// Look for switch statements.
		else if(boost::regex_match(line.c_str(), capture_results, f_switch_expression))
		{
			// Add the switch to the block.
			Location *loc = new Location(capture_results[1].str());
			std::cerr << "Found switch at location: " << *loc << std::endl;
			Switch *switch_stmnt = new Switch(loc);
			std::cout << "SW=" << line << std::endl;
			return switch_stmnt;
		}

		// Look for function calls.
		else if(boost::regex_match(line.c_str(), capture_results, f_function_call_expression))
		{
			std::cerr << "Found call: " << capture_results[2] << std::endl;

			// Add the call to the block.
			// Note that at this point, it's an Unresolved call, since we haven't
			// linked yet.
			Location loc(capture_results[1].str());
			FunctionCallUnresolved *f = new FunctionCallUnresolved(capture_results[2], &loc);
			std::cout << "CALL=" << line << std::endl;
			return f;
		}
		else if(boost::regex_match(line.c_str(), capture_results, f_succ_expression))
		{
			// This is the start of a "#SUCC".  Reset the stream and return.
			std::cout << "SUCC=" << line << std::endl;
			input_stream.seekg(start_pos);
		
			return NULL;
		}

		// Else it is an unknown statement.  Absorb it.
		/// @todo Parse these as well.
		std::cout << "UNK=" << line << std::endl;
		start_pos = input_stream.tellg();
	}
	
	// Stream no longer reports good.		
	return NULL;
}

// Normal members.

StatementBase::StatementBase(const Location *location) 
{
	m_location = new Location(*location);
}

StatementBase::StatementBase(const StatementBase& orig) 
{
	// Do a deep copy of the Location object.
	m_location = new Location(*orig.m_location);
}

StatementBase::~StatementBase()
{
}
