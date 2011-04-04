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
#include <fstream>
#include <string>

#include <boost/regex.hpp>

#include "Block.h"
#include "Function.h"

using namespace boost;

int main(int argc, const char* argv[])
{
	// Regex for finding function definitions.
	static const boost::regex function_def_expression("^([[:alpha:]_][[:alnum:]_\\:<>]*) \\(.*?\\)");

	// Regex for finding block starts.  Capture 1 is the block number, 2 is the starting line in the file.
	static const boost::regex block_start_expression("[[:space:]]+# BLOCK ([[:digit:]]+)(?:, starting at line ([[:digit:]]+))?");

	// Regex for finding a "# SUCC:" statement, which ends the current block.
	// Capture 1 is the string of successors, which will be further parsed by the Block object itself.
	static const boost::regex successor_expression("[[:space:]]+# SUCC\\: (.*)");

	// Regex to find function calls. Capture 1 is the file/line no, 2 is the called function ID.
	static const boost::regex function_call_expression(".+?(\\[.+?\\]) ([[:alpha:]_][[:alnum:]_]*) \\(.*?\\);");
	
	if(argc > 0)
	{
		std::ifstream input_file(argv[1], std::ifstream::in);

		// Check if we were able to open the file.
		if(input_file.fail())
		{
			std::cerr << "ERROR: Couldn't open file \"" << argv[1] << "\"" << std::endl;
			return 1;
		}

		std::string line;
		std::string in_function_name;
		bool in_function = false;
		bool in_block = false;
		
		Function *current_function;
		Block *current_block;
		
		while(input_file.good())
		{
			// Get the next line of input.
			std::getline(input_file, line);
			
			boost::cmatch what;

			// Check for and ignore comments.
			if(line[0] == ';')
			{
				continue;
			}
			
			// Look for function definitions.
			if(regex_match(line.c_str(), what, function_def_expression))
			{
				in_function_name = what[1];
				std::cout << "Found function: " << in_function_name << std::endl;
				current_function = new Function(in_function_name);

				in_function = true;
				continue;
			}

			if(in_function)
			{
				if(line.compare("}") == 0)
				{
					// Found the end of the function.
					std::cout << "Found function end" << std::endl;
					in_function = false;
					continue;
				}

				// Look for block starts.
				if(regex_match(line.c_str(), what, block_start_expression))
				{
					long block_start_line_no;
					
					std::cout << "Found block: " << what[1] << std::endl;
					
					if(what[2].matched)
					{
						block_start_line_no = atoi(what[2].str().c_str());
					}
					else
					{
						block_start_line_no = 0;
					}
					
					current_block = new Block(current_function,
											 atoi(what[1].str().c_str()),
											 block_start_line_no);
					in_block = true;
					continue;
				}

				// Look for block ends.
				if(regex_match(line.c_str(), what, successor_expression))
				{
					std::cout << "Found SUCC:, ending block: " << what[1] << std::endl;

					// Tell the Block what its successors are.
					current_block->AddSuccessors(what[1]);
					
					// Add the block to the current function.
					current_function->AddBlock(current_block);
					in_block = false;
					continue;
				}

				if(in_block)
				{
					// Look for function calls.
					if(regex_match(line.c_str(), what, function_call_expression))
					{
						std::cout << "Found call: " << what[2] << std::endl;
						current_block->AddFunctionCall(what[2]);
						continue;
					}
				}
			}
		}
	}
  
  return 0;
}
