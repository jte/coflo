/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

/** @file Implementation file for the TranslationUnit class. */

#include "TranslationUnit.h"

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

// Include the necessary Boost libraries.
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include "debug_utils/debug_utils.hpp"

#include "Location.h"
#include "Function.h"

#include "controlflowgraph/statements/If.h"
#include "controlflowgraph/statements/FunctionCallUnresolved.h"
#include "controlflowgraph/statements/NoOp.h"
#include "controlflowgraph/statements/Switch.h"

#include "libexttools/ToolCompiler.h"
#include "libexttools/toollib.h"

#include "templates/FileTemplate.h"

#include "parsers/gcc_gimple_parser.h"

using namespace boost;
using namespace boost::filesystem;


TranslationUnit::TranslationUnit(Program *parent_program, const std::string &file_path)
{
	m_parent_program = parent_program;
	m_source_filename = file_path;
}

TranslationUnit::TranslationUnit(const TranslationUnit& orig)
{
	/// @todo Implement a copy constructor.  Not sure we really need one for this class.
}

TranslationUnit::~TranslationUnit()
{
}

bool TranslationUnit::ParseFile(const boost::filesystem::path &filename,
								T_ID_TO_FUNCTION_PTR_MAP *function_map,
								const std::string &the_filter,
								ToolCompiler *compiler,
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
		dlog_parse_gimple << "File is C++" << std::endl;
		file_is_cpp = true;
	}
	
	// Try to compile the source file into the .gimple intermediate form.
	CompileSourceFile(filename.generic_string(), the_filter, compiler, defines, include_paths);
	
	// Construct the filename of the .cfg file gcc made for us.
	// gcc puts this file in the directory it's running in.
	gcc_cfg_lineno_blocks_filename = filename.filename().generic_string();
	gcc_cfg_lineno_blocks_filename += ".coflo.gimple";
		
	// Try to open the file whose name we were passed.
	std::ifstream input_file(gcc_cfg_lineno_blocks_filename.c_str(), std::ifstream::in);

	// Check if we were able to open the file.
	if(input_file.fail())
	{
		std::cerr << "ERROR: Couldn't open file \"" << gcc_cfg_lineno_blocks_filename << "\"" << std::endl;
		return false;
	}


	// Load the given file into memory.
	std::string buffer;
	char previous_char = '\n';

	while (input_file.good())     // loop while extraction from file is possible
	{
		char c;
		c = input_file.get();       // get character from file
		if (input_file.good())
		{
			if(c == '\r')
			{
				// Strip CR's.
				continue;
			}
			else
			{
				buffer += c;
			}
			previous_char = c;
		}
	}

	// Close file
	input_file.close();

	//std::cout << "Read >>>>>" << buffer << "<<<<<" << std::endl;

	// Create a new parser.
	D_Parser *parser = new_gcc_gimple_Parser();
	D_ParseNode *tree = gcc_gimple_dparse(parser, const_cast<char*>(buffer.c_str()), buffer.length());

	if (tree && !gcc_gimple_parser_GetSyntaxErrorCount(parser))
	{
		// Parsed the .coflo.gimple file successfully.

		dlog_parse_gimple << "File \"" << filename.generic_string() << "\" parsed successfully." << std::endl;

		FunctionInfoList *fil = gcc_gimple_parser_GetUserInfo(tree)->m_function_info_list;

		// Build the Functions out of the info obtained from the parsing.
		std::cout << "Building Functions..." << std::endl;
		BuildFunctionsFromThreeAddressFormStatementLists(*fil, function_map);
	}
	else
	{
		// The parse failed.

		std::cout << "Failure: " << gcc_gimple_parser_GetSyntaxErrorCount(parser) << " syntax errors." << std::endl;
	}

	if(tree != NULL)
	{
		// Destroy the parse tree.
		free_gcc_gimple_ParseTreeBelow(parser, tree);
	}
	// Destroy the parser.
	free_gcc_gimple_Parser(parser);

	return true;
}

void TranslationUnit::Link(const std::map< std::string, Function* > &function_map,
		T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls)
{
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		fp->Link(function_map, unresolved_function_calls);
	}
}


/**
 * Template for HTML <div> for the function CFG image, with an <h2> title.
 */
const std::string str_template_function_cfg = std::string(""
		"<div id=\"tabs-TABNUMBER\">\n"
		"	<h2>Control Flow Graph for IDENTIFIER_FUNCTION()</h2>\n"
		"	<object class=\"svg-cfg\" type=\"image/svg+xml\" data=\"IDENTIFIER_FUNCTION.svg\"></object>\n"
		"</div>\n");


void TranslationUnit::Print(ToolDot *the_dot, const boost::filesystem::path &output_dir, FileTemplate & index_html_out)
{
	std::cout << "Translation Unit Filename: " << m_source_filename << std::endl;
	std::cout << "Number of functions defined in this translation unit: " << m_function_defs.size() << std::endl;
	std::cout << "Defined functions:" << std::endl;
	// Print the identifiers of the functions defined in this translation unit.
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		std::cout << "Function: " << fp->GetIdentifier() << std::endl;
	}
	
	// Create filenames for the index and primary css files.
	std::string index_html_filename = (output_dir / "index.html").generic_string();
/*
	index_html_out << "<p>Filename: "+m_source_filename.generic_string()+"</p>" << std::endl;
	index_html_out << "<p>Control Flow Graphs:\n<ul>" << std::endl;
*/
	std::stringstream ss;
	int i = 1;
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		ss.str("");
		ss << "<li><a href=\"#tabs-" << i << "\">" << fp->GetIdentifier();
		if(!fp->IsCalled())
		{
			ss << " (possible entry point)";
		}
		ss << "</a></li>\n";

		index_html_out.regex_append_after("<!-- TAB_LIST -->", ss.str());

		std::string cfg_image_filename;
		cfg_image_filename = fp->GetIdentifier()+".svg";
		fp->PrintControlFlowGraphBitmap(the_dot, output_dir / cfg_image_filename);

		// Output the tab panel HTML for this function.
		ss.str("");
		ss << i;
		FileTemplate function_cfg(str_template_function_cfg);
		function_cfg.regex_replace("IDENTIFIER_FUNCTION", fp->GetIdentifier());
		function_cfg.regex_replace("TABNUMBER",	ss.str());
		function_cfg.Apply();
		index_html_out.regex_append_after("<!-- TAB_PANEL_LIST -->", function_cfg.str());

		i++;

//		index_html_out << "<li><a href=\"#"+fp->GetIdentifier()+"\">"+fp->GetIdentifier()+"</a>";
//
//		if(!fp->IsCalled())
//		{
//			index_html_out << " (possible entry point)";
//		}
//		index_html_out << "</li>" << std::endl;
	}
//	index_html_out << "</ul>\n</p>" << std::endl;
	
//	BOOST_FOREACH(Function* fp, m_function_defs)
//	{
//		std::string cfg_image_filename;
//		cfg_image_filename = fp->GetIdentifier()+".svg";
//		fp->PrintControlFlowGraphBitmap(the_dot, output_dir / cfg_image_filename);
//
//		// Output the HTML for this function.
//		/*index_html_out*/ ss << regex_replace(str_template_function_cfg, "IDENTIFIER_FUNCTION", fp->GetIdentifier().c_str());
//	}

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
	
	// Do the compile.
	int compile_retval = compiler->GenerateCFG(params.c_str(), file_path);
	
	if(compile_retval != 0)
	{
		std::cerr << "ERROR: Compile string returned nonzero." << std::endl;
		exit(1);
	}
}

void TranslationUnit::BuildFunctionsFromThreeAddressFormStatementLists(const std::vector< FunctionInfo* > & function_info_list,
		T_ID_TO_FUNCTION_PTR_MAP *function_map)
{
	// Go through each FunctionInfo.
	BOOST_FOREACH(FunctionInfo *fi, function_info_list)
	{

		dlog_parse_gimple << "Processing FunctionInfo for function \"" << *(fi->m_identifier) << "\"..." << std::endl;

		// Create the function.
		Function *f = new Function(this, *(fi->m_identifier));

		// Add the new function to the list.
		m_function_defs.push_back(f);

		// Add the new Function to the program-wide function map.
		(*function_map)[*(fi->m_identifier)] = f;

		// Create the control flow graph for this function.
		f->CreateControlFlowGraph(*(fi->m_statement_list));
	}
}


