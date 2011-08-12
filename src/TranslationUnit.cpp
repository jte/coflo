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
#include <glob.h>


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
#include "Function.h"

#include "statements/If.h"
#include "statements/FunctionCallUnresolved.h"
#include "statements/NoOp.h"
#include "statements/Switch.h"

#include "libexttools/ToolCompiler.h"

using namespace boost;
using namespace boost::filesystem;

/// Regex string for matching C and C++ identifiers.
static const std::string f_identifier_expression("[[:alpha:]_][[:alnum:]_]*");

static const std::string f_qualifiers("const|virtual|static");

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
/// - Capture 4 is the formal parameter list, just the names.
static const boost::regex f_cpp_function_def_expression("^([^;]*)[[:space:]]("+f_identifier_expression+")\\(.*\\)[[:space:]]\\(.*\\)");

/// Regex for finding block starts.  Capture 1 is the block number, 2 is the starting line in the file (possibly "-1").
static const boost::regex f_block_start_expression("[[:space:]]+# BLOCK ([[:digit:]]+)(?:, starting at line ([-]?[[:digit:]]+))?(?:, discriminator [[:digit:]]+)?");



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
		std::cerr << "File is C++" << std::endl;
		file_is_cpp = true;
	}
	
	// Try to compile the source file.
	CompileSourceFile(filename.generic_string(), the_filter, compiler, defines, include_paths);
	
	// Construct the filename of the .cfg file gcc made for us.
	// gcc puts this file in the directory it's running in.
	gcc_cfg_lineno_blocks_filename = filename.filename().generic_string();
	
	// The "*" has so far been observed to be either 012 or 013, depending on the version
	// of gcc.
	gcc_cfg_lineno_blocks_filename += ".*t.cfg";
		
	// Find the actual file name.	
	{
		// Buffer to store the globbing results.
		glob_t globbuf;
		int retval;
		
		retval = glob(gcc_cfg_lineno_blocks_filename.c_str(), GLOB_NOSORT, NULL, &globbuf);
		switch(retval)
		{
			case 0:
				// Found something.
				if(globbuf.gl_pathc > 1)
				{
					// More than one match, that shouldn't be.
					std::cerr << "ERROR: More than one match to file \""
						<< gcc_cfg_lineno_blocks_filename
						<< "\"" << std::endl;
				}
				// Otherwise everything is OK, get the real filename.
				gcc_cfg_lineno_blocks_filename.assign(globbuf.gl_pathv[0]);
				std::cerr << "INFO: Successfully matched \""
					<< gcc_cfg_lineno_blocks_filename << "\" with \""
					<< globbuf.gl_pathv[0] << "\"" << std::endl;
				break;
			case GLOB_NOMATCH:
				std::cerr << "ERROR: No match to file \""
						<< gcc_cfg_lineno_blocks_filename
						<< "\"" << std::endl;
				break;
			case GLOB_NOSPACE:
			case GLOB_ABORTED:
			default:
				std::cerr << "ERROR: glob() failed attempting to match \""
						<< gcc_cfg_lineno_blocks_filename
						<< "\"" << std::endl;
				break;

		}
		globfree(&globbuf);
	}
	
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
			current_function = new Function(this, in_function_name);

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
				bool block_parsed_successfully;

				std::cout << "Found block: " << capture_results[1] << std::endl;

				// See if there was a starting line number for this block.
				if(capture_results[2].matched)
				{
					// There was, pass it to the Block() constructor.
					block_start_line_no = atoi(capture_results[2].str().c_str());
					/// @todo For some reason, gcc 4.5.3 will sometimes have a "-1" for the
					/// block starting line.  Clamp this to zero until/unless we figure out
					/// if this is actually telling us something useful.
					if(block_start_line_no < 0)
					{
						block_start_line_no = 0;
					}
				}
				else
				{
					block_start_line_no = 0;
				}
				std::cout << "BLOCK LINE NO: " << block_start_line_no << std::endl;
				current_block = new Block(current_function,
										 atoi(capture_results[1].str().c_str()),
										 block_start_line_no);
				
				block_parsed_successfully = current_block->Parse(input_file);
				if(!block_parsed_successfully)
				{
					std::cerr << "ERROR: Block parse failed." << std::endl;
				}
				
				// Add the block to the current function.
				current_function->AddBlock(current_block);
				
				continue;
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
		T_UNRESOLVED_FUNCTION_CALL_MAP *unresolved_function_calls)
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

void TranslationUnit::Print(ToolDot *the_dot, const boost::filesystem::path &output_dir, std::ofstream & index_html_out)
{
	std::cout << "Translation Unit Filename: " << m_source_filename << std::endl;
	std::cout << "Number of functions defined in this translation unit: " << m_function_defs.size() << std::endl;
	std::cout << "Defined functions:" << std::endl;
	// Print the identifiers of the functions defined in this translation unit.
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		std::cout << "Function: " << fp->GetIdentifier() << std::endl;
	}
	
	std::string index_html_filename = (output_dir / "index.html").generic_string();
	
	index_html_out << "<p>Filename: "+m_source_filename.generic_string()+"</p>" << std::endl;
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
