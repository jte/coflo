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

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>


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

#include "Location.h"
#include "Block.h"
#include "If.h"
#include "Function.h"
#include "FunctionCallUnresolved.h"
#include "NoOp.h"
#include "Switch.h"
#include "TranslationUnit.h"
#include "libexttools/ToolCompiler.h"

using namespace boost;
using namespace boost::filesystem;

/// Regex string for matching C and C++ identifiers.
static const std::string f_identifier_expression("[[:alpha:]_][[:alnum:]_]*");

static const std::string f_qualifiers("const|virtual|static");

/// Regex string for matching and capturing locations.
/// Capture 1 is the path, 2 is the line number.
static const std::string f_location("(\\[[^\\]]*?[[:space:]]\\:[[:space:]][[:digit:]]+\\])");

/// .+? is the filename.
static const std::string f_static_destructors("\\(static destructors for .+?\\)");

/// .+? is the filename.
static const std::string f_static_initializers("\\(static initializers for .+?\\)");

/// Regex for finding C function definitions in gcc *.cfg output.
static const boost::regex f_c_function_def_expression("^("+f_identifier_expression+") \\(.*?\\)");

/// Regex for finding C++ function definitions in gcc *.cfg output.
/// - Capture 1 is everything before the function name (i.e. return type, qualifiers, etc.).
/// - Capture 2 is the function identifier itself.
/// - Capture 3 is the formal parameter list, just the types.
/// - Capture 4 is the formal maramter list, just the names.
static const boost::regex f_cpp_function_def_expression("^([^;]*)[[:space:]]("+f_identifier_expression+")\\(.*\\)[[:space:]]\\(.*\\)");

// Regex for finding block starts.  Capture 1 is the block number, 2 is the starting line in the file.
static const boost::regex f_block_start_expression("[[:space:]]+# BLOCK ([[:digit:]]+)(?:, starting at line ([[:digit:]]+))?");

// Regex for finding a "# SUCC:" statement, which ends the current block.
// Capture 1 is the string of successors, which will be further parsed by the Block object itself.
static const boost::regex f_successor_expression("[[:space:]]+# SUCC\\:[[:space:]]*(.*)");

// Regex to find function calls. Capture 1 and 2 is the file/line no, 3 is the called function ID.
static const boost::regex f_function_call_expression(".+?"+f_location+" ([[:alpha:]_][[:alnum:]_]*) \\(.*\\);");

// Regex to find if/else statements.  Capture 1 is the file/line no.
static const boost::regex f_if_expression(".+?"+f_location+" if \\(.*?\\)");

// Regex to find switch statements.  Capture 1 and 2 is the file/line no.
static const boost::regex f_switch_expression(".+?"+f_location+" switch \\(.*?\\)");


TranslationUnit::TranslationUnit(const std::string &file_path)
{
	m_source_filename = file_path;
}

TranslationUnit::TranslationUnit(const TranslationUnit& orig)
{
}

TranslationUnit::~TranslationUnit()
{
}

bool TranslationUnit::ParseFile(const boost::filesystem::path &filename,
								T_ID_TO_FUNCTION_PTR_MAP *function_map,
								const std::string &the_filter,
								ToolCompiler *compiler,
								const std::string &the_ctags,
								const std::vector< std::string > &defines,
								const std::vector< std::string > &include_paths,
								bool debug_parse)
{
	std::string gcc_cfg_lineno_blocks_filename;
	bool file_is_cpp = false;
	
	// Save the source filename.
	m_source_filename = filename;
	
	// Check if it's a C++ file.
	if(filename.extension() == ".cpp")
	{
		std::cout << "File is C++" << std::endl;
		file_is_cpp = true;
	}
	
	// Try to compile the source file.
	CompileSourceFile(filename.string(), the_filter, compiler, defines, include_paths);
	
	// Construct the filename of the .cfg file gcc made for us.
	// gcc puts this file in the directory it's running in.
	gcc_cfg_lineno_blocks_filename = filename.filename().string()+".013t.cfg";
	
	// Try to open the file whose name we were passed.
	std::ifstream input_file(gcc_cfg_lineno_blocks_filename.c_str(), std::ifstream::in);

	// Check if we were able to open the file.
	if(input_file.fail())
	{
		std::cerr << "ERROR: Couldn't open file \"" << gcc_cfg_lineno_blocks_filename << "\"" << std::endl;
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

		static const boost::regex *function_regex;
		int function_id_offset;
		
		// GCC outputs a different function signature depending on file type.
		// Switch which regex we're using according to the source file's language.
		if(file_is_cpp)
		{
			// This is a C++ file, use the appropriate regex.
			function_regex = &f_cpp_function_def_expression;
			function_id_offset = 2;
		}
		else
		{
			// It's C.
			function_regex = &f_c_function_def_expression;
			function_id_offset = 1;
		}
		
		// Look for function definitions.
		if(regex_match(line.c_str(), capture_results, *function_regex))
		{
			in_function_name = capture_results[function_id_offset];
			std::cout << "Found function: " << in_function_name << std::endl;
			current_function = new Function(in_function_name);

			(*function_map)[in_function_name] = current_function;
			
			// Add the new function to the list.
			m_function_defs.push_back(current_function);

			in_function = true;

			continue;
		}

		if(in_function)
		{
			// We're inside a function.
			
			if(line.compare("}") == 0)
			{
				// Found the end of the function.
				std::cout << "Found function end" << std::endl;
				in_function = false;
				continue;
			}

			// Look for block starts.
			if(regex_match(line.c_str(), capture_results, f_block_start_expression))
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
			if(regex_match(line.c_str(), capture_results, f_successor_expression))
			{
				std::cout << "Found SUCC:, ending block: " << capture_results[1] << std::endl;
				
				if(current_block->NumberOfStatements() == 0)
				{
					// Make sure every block has at least one statement.
					current_block->AddStatement(new NoOp(new Location("[UNKNOWN/file.c : 0]")));
				}

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
				if(regex_match(line.c_str(), capture_results, f_if_expression))
				{
					// Add the if to the block.
					Location *loc = new Location(capture_results[1].str());
					std::cerr << "Found if at location: " << *loc << std::endl;
					If *if_stmnt = new If(loc);
					current_block->AddStatement(if_stmnt);
					
					continue;
				}
				
				// Look for switch statements.
				if(regex_match(line.c_str(), capture_results, f_switch_expression))
				{
					// Add the switch to the block.
					Location *loc = new Location(capture_results[1].str());
					std::cerr << "Found switch at location: " << *loc << std::endl;
					Switch *switch_stmnt = new Switch(loc);
					current_block->AddStatement(switch_stmnt);
					
					continue;
				}
				
				// Look for function calls.
				if(regex_match(line.c_str(), capture_results, f_function_call_expression))
				{
					std::cerr << "Found call: " << capture_results[2] << std::endl;
					
					// Add the call to the block.
					// Note that at this point, it's an Unresolved call, since we haven't
					// linked yet.
					Location loc(capture_results[1].str());
					FunctionCallUnresolved *f = new FunctionCallUnresolved(capture_results[2], &loc);
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

void TranslationUnit::Link(const std::map< std::string, Function* > &function_map,
						  std::vector< FunctionCall* > *unresolved_function_calls)
{
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		fp->Link(function_map, unresolved_function_calls);
	}
}

bool TranslationUnit::CreateControlFlowGraphs(T_CFG * cfg)
{
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		fp->CreateControlFlowGraph(*cfg);
	}
	
	return true;
}

void TranslationUnit::Print(const std::string &the_dot, const boost::filesystem::path &output_dir, std::ofstream & index_html_out)
{
	std::cout << "Translation Unit Filename: " << m_source_filename << std::endl;
	std::cout << "Number of functions defined in this translation unit: " << m_function_defs.size() << std::endl;
	std::cout << "Defined functions:" << std::endl;
	// Print the identifiers of the functions defined in this translation unit.
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		std::cout << "Function: " << fp->GetIdentifier() << std::endl;
	}
	
	std::string index_html_filename = output_dir.string()+ "index.html";
	
	index_html_out << "<p>Filename: "+m_source_filename.string()+"</p>" << std::endl;
	index_html_out << "<p>Control Flow Graphs:<ul>" << std::endl;
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		index_html_out << "<li><a href=\"#"+fp->GetIdentifier()+"\">"+fp->GetIdentifier()+"</a>";
		
		if(!fp->IsCalled())
		{
			index_html_out << " (possible entry point)";
		}
		index_html_out << "</li>" << std::endl;
	}
	index_html_out << "</ul></p>" << std::endl;
	
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		//fp->Print();
		fp->PrintDotCFG(the_dot, output_dir);
		index_html_out << "<p><h2><a name=\""+fp->GetIdentifier()+"\">Control Flow Graph for "+fp->GetIdentifier()+"()</a></h2>" << std::endl;
		index_html_out << "<div style=\"text-align: center;\"><IMG SRC=\""+fp->GetIdentifier()+".dot.png"+"\" ALT=\"image\"></div></p>" << std::endl;
	}
}

void TranslationUnit::CompileSourceFile(const std::string& file_path, const std::string &the_filter, ToolCompiler *compiler,
										const std::vector< std::string > &defines,
										const std::vector< std::string > &include_paths)
{
	// Do the filter first.
	/// \todo Add the prefilter functionality.
	
	// Create the compile command.
	std::string params;
	
	// Add the defines.
	/// @todo Make the -D's and -I's obey the ordering given on the initial command line.
	BOOST_FOREACH(std::string d, defines)
	{
		params += " -D \"" + d + "\"";
	}
	// Add the includes.
	BOOST_FOREACH(std::string ip, include_paths)
	{
		params += " -I \"" + ip + "\"";
	}
	params += " \"" + file_path + "\"";
	
	// Do the compile.
	int compile_retval = compiler->GenerateCFG(params.c_str());
	
	if(compile_retval != 0)
	{
		std::cerr << "ERROR: Compile string returned nonzero." << std::endl;
		exit(1);
	}
}
