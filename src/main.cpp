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

#include <iostream>
#include <fstream>
#include <string>

#include <boost/regex.hpp>

#include "Block.h"
#include "Function.h"

using namespace boost;

int main(int argc, const char* argv[])
{
	static const boost::regex expression("^([[:alpha:]_][[:alnum:]_]*) \\(.*?\\)");
	static const boost::regex block_start_expression("[[:space:]]+# BLOCK ([[:digit:]]+)");
	static const boost::regex block_end_expression("[[:space:]]+# SUCC\\: (.*)");
	static const boost::regex function_call_expression(".+?([[:alpha:]_][[:alnum:]_]*) \\(.*?\\);");
	
	if(argc > 0)
	{
		std::ifstream input_file(argv[1], std::ifstream::in);
		std::string line;
		std::string in_function_name;
		
		Function *current_function;
		Block *current_block;
		
		while(input_file.good())
		{
			// Get the next line of input.
			std::getline(input_file, line);
			
			boost::cmatch what;
			
			// Look for function definitions.
			if(regex_match(line.c_str(), what, expression)) 
			{
				in_function_name = what[1];
				std::cout << "Found function: " << in_function_name << std::endl;
				current_function = new Function(in_function_name);
			}
			
			// Look for block starts.
			if(regex_match(line.c_str(), what, block_start_expression)) 
			{
				std::cout << "Found block: " << what[1] << std::endl;
				current_block = new Block(atoi(what[1].str().c_str()));
			}
			
			// Look for block ends.
			if(regex_match(line.c_str(), what, block_end_expression)) 
			{
				std::cout << "Found block end: " << what[1] << std::endl;
				current_function->AddBlock(current_block);
			}
			
			// Look for function calls.
			if(regex_match(line.c_str(), what, function_call_expression)) 
			{
				std::cout << "Found call: " << what[1] << std::endl;
				current_block->AddFunctionCall(what[1]);
			}
		}
	}
  
  return 0;
}
