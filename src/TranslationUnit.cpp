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
/*
const std::string str_template_function_cfg = std::string(""
		"<div id=\"tabs-TABNUMBER\" class=\"cfg-pane\">\n"
		"	<h2>Control Flow Graph for IDENTIFIER_FUNCTION()</h2>\n"
		"	<object class=\"svg-cfg\" type=\"image/svg+xml\" data=\"IDENTIFIER_FUNCTION.svg\"></object>\n"
		"</div>\n");
*/

const std::string str_template_function_cfg = std::string(""
	"<div id=\"@IDENTIFIER_FUNCTION@\">\n"
	"		<!-- Surround the function name with an anchor in an attempt at having reasonable <noscript> nav fallback. -->\n"
	"		<p><a id=\"file1-@IDENTIFIER_FUNCTION@\">@IDENTIFIER_FUNCTION@()</a></p>\n"
	"		<object class=\"svg-cfg\" type=\"image/svg+xml\" data=\"@IDENTIFIER_FUNCTION@.svg\"></object>\n"
	"	</div>");

const char f_str_template_nav_tree_function_entry[] =
		"			<li data-nav-tree-node-type=\"function\">\n"
		"				<a href=\"#@IDENTIFIER_FUNCTION@\">@IDENTIFIER_FUNCTION@()</a>\n"
		"			</li>\n";

const char f_str_template_nav_tree_file_entry[] =
		"	<li id=\"@UNIQUE_FILE_ID@\" class=\"coflo-nav-tree-file\" data-nav-tree-node-type=\"source_file\">\n"
		"		<a href=\"#\">@FILENAME@</a>\n"
		"		<ul>\n"
		"			<!-- NAV_FUNCTION_ENTRY_START -->\n"
		"			<!-- NAV_FUNCTION_ENTRY_END -->\n"
		"		</ul>\n"
		"	</li>";

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

	// Create an entry for this file in the navigation tree.
	FileTemplate nav_tree_table_entry(f_str_template_nav_tree_file_entry);

	// Create a unique ID for this file which can be referenced by the other entries in the nav tree.
	std::string unique_file_id = std::string("file_") + m_source_filename.generic_string();
	unique_file_id = regex_replace(unique_file_id, "[./-]", "_");

	nav_tree_table_entry.regex_replace("@UNIQUE_FILE_ID@", unique_file_id);
	nav_tree_table_entry.regex_replace("@FILENAME@", m_source_filename.filename().generic_string());

	// Now insert it into the navigation tree.
	index_html_out.regex_insert_before("<!-- NAV_END -->", nav_tree_table_entry.str());

	std::stringstream ss;
	int i = 1;
	BOOST_FOREACH(Function* fp, m_function_defs)
	{
		ss.str("");
		ss << i;
		/*if(!fp->IsCalled())
		{
			ss << " (possible entry point)";
		}*/

		// Generate the tab list item for this function.
		FileTemplate nav_tree_table_entry_function(f_str_template_nav_tree_function_entry);
		nav_tree_table_entry_function.regex_replace("@UNIQUE_FILE_ID@", unique_file_id);
		nav_tree_table_entry_function.regex_replace("@IDENTIFIER_FUNCTION@", fp->GetIdentifier());
		nav_tree_table_entry_function.regex_replace("@UNIQUE_FUNCTION_ID@", fp->GetIdentifier());
		nav_tree_table_entry_function.regex_replace("@TABNUMBER@", ss.str());
		index_html_out.regex_insert_before("<!-- NAV_FUNCTION_ENTRY_END -->", nav_tree_table_entry_function.str());

		std::string cfg_image_filename;
		cfg_image_filename = fp->GetIdentifier()+".svg";
		fp->PrintControlFlowGraphBitmap(the_dot, output_dir / cfg_image_filename);

		// Output the tab panel HTML for this function.
		FileTemplate function_cfg(str_template_function_cfg);
		function_cfg.regex_replace("@IDENTIFIER_FUNCTION@", fp->GetIdentifier());
		function_cfg.regex_replace("TABNUMBER",	ss.str());
		index_html_out.regex_insert_before("<!-- TAB_PANEL_LIST_END -->", function_cfg.str());

		i++;
	}
}

void TranslationUnit::CompileSourceFile(const std::string& file_path, const std::string &the_filter, ToolCompiler *compiler,
										const std::vector< std::string > &defines,
										const std::vector< std::string > &include_paths)
{
	// Do the filter first.
	/// @todo Add the prefilter functionality.
	
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
		/// @todo This is rather inelegant error handling.
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


