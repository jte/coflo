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

// Include the necessary Boost libraries.
#include <boost/regex.hpp>
// Tell <boost/filesystem.hpp> that we want the new interface.
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include "TranslationUnit.h"

#include "Block.h"
#include "If.h"
#include "Function.h"
#include "FunctionCall.h"
#include "Switch.h"
#include "TranslationUnit.h"

using namespace boost;
using namespace boost::filesystem;

// Regex for finding function definitions.
static const boost::regex function_def_expression("^([[:alpha:]_][[:alnum:]_\\:<>]*) \\(.*?\\)");

// Regex for finding block starts.  Capture 1 is the block number, 2 is the starting line in the file.
static const boost::regex block_start_expression("[[:space:]]+# BLOCK ([[:digit:]]+)(?:, starting at line ([[:digit:]]+))?");

// Regex for finding a "# SUCC:" statement, which ends the current block.
// Capture 1 is the string of successors, which will be further parsed by the Block object itself.
static const boost::regex successor_expression("[[:space:]]+# SUCC\\:[[:space:]]*(.*)");

// Regex to find function calls. Capture 1 is the file/line no, 2 is the called function ID.
static const boost::regex function_call_expression(".+?(\\[.+?\\]) ([[:alpha:]_][[:alnum:]_]*) \\(.*?\\);");

// Regex to find if/else statements.  Capture 1 is the file/line no.
static const boost::regex if_expression(".+?(\\[.+?\\]) if \\(.*?\\)");

// Regex to find switch statements.  Capture 1 is the file/line no.
static const boost::regex switch_expression(".+?(\\[.+?\\]) switch \\(.*?\\)");


TranslationUnit::TranslationUnit()
{
}

TranslationUnit::TranslationUnit(const TranslationUnit& orig)
{
}

TranslationUnit::~ TranslationUnit()
{
}

bool TranslationUnit::ParseFile(const boost::filesystem::path &filename, bool debug_parse /* = false */)
{
	// Save the filename.
	m_filename = filename;
	
	// Try to open the file whose name we were passed.
	std::ifstream input_file(filename.string().c_str(), std::ifstream::in);

	// Check if we were able to open the file.
	if(input_file.fail())
	{
		std::cerr << "ERROR: Couldn't open file \"" << filename << "\"" << std::endl;
		return false;
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

		boost::cmatch capture_results;

		// Check for and ignore comments.
		if(line[0] == ';')
		{
			continue;
		}

		// Look for function definitions.
		if(regex_match(line.c_str(), capture_results, function_def_expression))
		{
			in_function_name = capture_results[1];
			std::cout << "Found function: " << in_function_name << std::endl;
			current_function = new Function(in_function_name);

			// Add the new function to the list.
			m_function_defs.push_back(current_function);

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
			if(regex_match(line.c_str(), capture_results, block_start_expression))
			{
				long block_start_line_no;

				std::cout << "Found block: " << capture_results[1] << std::endl;

				// See if there was a starting line number for this block.
				if(capture_results[2].matched)
				{
					block_start_line_no = atoi(capture_results[2].str().c_str());
				}
				else
				{
					block_start_line_no = 0;
				}

				current_block = new Block(current_function,
										 atoi(capture_results[1].str().c_str()),
										 block_start_line_no);
				in_block = true;
				continue;
			}

			// Look for block ends.
			if(regex_match(line.c_str(), capture_results, successor_expression))
			{
				std::cout << "Found SUCC:, ending block: " << capture_results[1] << std::endl;

				// Tell the Block what its successors are.
				current_block->AddSuccessors(capture_results[1]);

				// Add the block to the current function.
				current_function->AddBlock(current_block);
				in_block = false;
				continue;
			}

			if(in_block)
			{
				// Look for if statements.
				if(regex_match(line.c_str(), capture_results, if_expression))
				{
					std::cerr << "Found if at location: " << capture_results[1] << std::endl;
					
					// Add the if to the block.
					If *if_stmnt = new If();
					current_block->AddStatement(if_stmnt);
					
					continue;
				}
				
				// Look for switch statements.
				if(regex_match(line.c_str(), capture_results, switch_expression))
				{
					std::cerr << "Found switch at location: " << capture_results[1] << std::endl;
					
					// Add the switch to the block.
					Switch *switch_stmnt = new Switch();
					current_block->AddStatement(switch_stmnt);
					
					continue;
				}
				
				// Look for function calls.
				if(regex_match(line.c_str(), capture_results, function_call_expression))
				{
					std::cerr << "Found call: " << capture_results[2] << std::endl;
					
					// Add the call to the block.
					FunctionCall *f = new FunctionCall(capture_results[2]);
					current_block->AddStatement(f);
					
					continue;
				}
			}
		}
	}

	return true;
}

bool TranslationUnit::LinkBasicBlocks()
{
	// Go through each function in the translation unit and do the block linking.
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		fp->LinkBlocks();
		
		/// Link the basic blocks into a BlockGraph.
		fp->LinkIntoGraph();
	}

	return true;
}

void TranslationUnit::Print()
{
	std::cout << "Translation Unit Filename: " << m_filename << std::endl;
	std::cout << "Number of functions defined in this translation unit: " << m_function_defs.size() << std::endl;
	std::cout << "Defined functions:" << std::endl;
	
	// Print the identifiers of the functions defined in this translation unit.
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		std::cout << "Function: " << fp->GetIdentifier() << std::endl;
	}
	
	std::cout << std::endl;

	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		fp->Print();
	}
}
