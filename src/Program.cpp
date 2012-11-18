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

#include "Program.h"

#include <fstream>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

#include <boost/foreach.hpp>


#include "TranslationUnit.h"
//#include "RuleReachability.h"
#include "controlflowgraph/statements/FunctionCall.h"
#include "Function.h"

// Include the templates for the output HTML, CSS, etc. files.
#include "templates/templates.h"
#include "templates/FileTemplate.h"

#include "libexttools/toollib.h"

Program::Program()
{

}

Program::Program(const Program& orig)
{
}

Program::~Program()
{
}

void Program::SetTheDot(ToolDot *the_dot)
{
	m_the_dot = the_dot;
}

void Program::SetTheGcc(ToolCompiler *the_compiler)
{
	m_compiler = the_compiler;
}

void Program::SetTheFilter(const std::string &the_filter)
{
	m_the_filter = the_filter;
}

void Program::AddSourceFiles(const std::vector< std::string > &file_paths)
{
	BOOST_FOREACH(std::string input_file_path, file_paths)
	{
		m_translation_units.push_back(new TranslationUnit(this, input_file_path));
	}
}

bool Program::Parse(const std::vector< std::string > &defines,
		const std::vector< std::string > &include_paths,
		T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls,
		bool debug_parse)
{
	BOOST_FOREACH(TranslationUnit *tu, m_translation_units)
	{
		// Parse this file.
		std::cout << "Parsing \"" << tu->GetFilePath() << "\"..." << std::endl;
		bool retval = tu->ParseFile(tu->GetFilePath(), &m_function_map,
								 m_the_filter, m_compiler,
								 defines, include_paths, debug_parse);
		if(retval == false)
		{
			std::cerr << "ERROR: Couldn't parse \"" << tu->GetFilePath() << "\"" << std::endl;
			return false;
		}
	}

	// Link the function calls.
	std::cout << "Linking function calls..." << std::endl;

	BOOST_FOREACH(TranslationUnit *tu, m_translation_units)
	{
		tu->Link(m_function_map, unresolved_function_calls);
	}

	// Parsing was successful.
	return true;
}

Function *Program::LookupFunction(const std::string &function_id)
{
	T_ID_TO_FUNCTION_PTR_MAP::iterator fit;
	
	// Look up the function by name.
	fit = m_function_map.find(function_id);
	
	if(fit == m_function_map.end())
	{
		// Couldn't find the function.
		return NULL;
	}
	else
	{
		// Found it.
		return fit->second;
	}
}


static const char f_html_index_head[] = ""
		"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"    <!-- Generated @DATETIME@ by CoFlo @VERSION@ -->\n"
		"    <meta charset=\"utf-8\">\n"
		"    <link rel=\"stylesheet\" type=\"text/css\" href=\"css/index.css\" />\n"
		"    <title>CoFlo Analysis Results</title>\n"
		"</head>\n"
		"";

#define M_STRINGIZE_HELPER(t) #t
#define M_STRINGIZE(s) M_STRINGIZE_HELPER(s)


void Program::Print(const std::string &output_path)
{
	boost::filesystem::path template_dir;
	boost::filesystem::path output_dir = output_path;
	
	const char *pkgdatadir = getenv("COFLO_PKGDATA_DIR");
	if(pkgdatadir != NULL)
	{
		template_dir = std::string(pkgdatadir) + "/templates";
	}
	else
	{
		template_dir = std::string(M_STRINGIZE(COFLO_PKGDATA_DIR) "/templates");
	}

	/*
	 * Create the report output directory structure.
	 */
	std::cout << "Creating output dir: " << output_dir << std::endl;
	mkdir(output_dir.string().c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
	mkdir((output_dir.string() + "/css").c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
	mkdir((output_dir.string() + "/img").c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
	mkdir((output_dir.string() + "/js").c_str(), S_IRWXU | S_IRWXG | S_IRWXO );

	
	std::string index_html_filename = (output_dir / "index.html").generic_string();
	std::string index_css_template_filename = (template_dir / "css" / "index.css").generic_string();
	std::string index_css_filename = (output_dir / "css/index.css").generic_string();
	std::ofstream index_html_out(index_html_filename.c_str());

	std::cout << "TEMPLATE=" << index_css_template_filename << std::endl;

	// Load the template strings.
	std::string index_css = std::string(css_index_template_css);
	FileTemplate index_htmlt(index_template_html);
	FileTemplate index_csst(css_index_template_css);

	// Remove the sections which are for development only.
	index_htmlt.regex_replace("(?s:<!-- REMOVE_START.*?REMOVE_END -->)", "<!-- REMOVED DEV TEXT -->");
	// Insert the title.
	/// @todo Make this settable from the command line.
	index_htmlt.regex_replace("@INDEX_TITLE@", "CoFlo Analysis Results");
	index_htmlt.regex_replace("@REPORT_HEADER@", "CoFlo Analysis Results");
	// Change the name of the referenced stylesheet from "index.template.css" to "index.css".
	index_htmlt.regex_replace("index.template.css", "index.css");

	// Generate the resulting report files and add the appropriate markup for each translation unit.
	BOOST_FOREACH(TranslationUnit *tu, m_translation_units)
	{
		tu->Print(m_the_dot, output_path, index_htmlt);
	}
	
	// Create the actual index.html file.
	index_html_out << index_htmlt << std::endl;

	// Create the primary stylesheet from the template.
	std::ifstream primary_css_template(index_css_template_filename.c_str());
	std::ofstream primary_css(index_css_filename.c_str());

	/// @todo Just copy it for now.
	primary_css << index_css << std::endl;

	// Copy the JavaScript and supporting css verbatim.
	{
		FileTemplate file(js_coflo_resizer_js);
		file.SaveAs((output_dir / "js/coflo.resizer.js").generic_string());
	}

	{
		FILE *fp;

		// Extract the report boilerplate files (e.g. jQuery UI js and css theme files) to the destination directory.
		fp = fopen((output_dir / "report_boilerplate.tar").generic_string().c_str(), "w");
		fwrite(report_boilerplate_tar, report_boilerplate_tar_len, 1, fp);
		fclose(fp);
		::system(("cd " + output_dir.generic_string() + " && tar -xaf report_boilerplate.tar").c_str());
	}

	// Set permissions on the generated report appropriately.
	/// @todo This is for development only at the moment.  We'll probably want to remove this.
	//::system(("cd " + output_dir.generic_string() + " && chmod -R 666 .").c_str());
}

void Program::PrintUnresolvedFunctionCalls(T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls)
{
	/// @todo Pass in.
	bool only_list_ids = true;

	// See if we have any unresolved calls.
	if(!unresolved_function_calls->empty())
	{
		// We couldn't link some function calls.
		std::cout << "WARNING: Unresolved function calls:" << std::endl;
		T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP::iterator it;
		for(it=(*unresolved_function_calls).begin(); it!=(*unresolved_function_calls).end();)
		{
			FunctionCallUnresolved *fc = it->second;
			if(!only_list_ids)
			{
				std::cout << "[" << fc->GetLocation() << "]: " << fc->GetIdentifier() << std::endl;
				++it;
			}
			else
			{
				std::cout << fc->GetIdentifier() << std::endl;
				it = unresolved_function_calls->upper_bound(fc->GetIdentifier());
			}
		}
	}
}

bool Program::PrintFunctionCFG(const std::string &function_identifier, bool cfg_verbose, bool cfg_vertex_ids)
{
	Function *function;
		
	function = LookupFunction(function_identifier);
	
	if(function == NULL)
	{
		// Couldn't find the function.
		std::cerr << "ERROR: Couldn't find function: "+function_identifier << std::endl;
		return false;
	}
	
	// Found it, now let's print its control-flow graph.
	std::cout << "Control Flow Graph of function " << function->GetIdentifier() << ":" << std::endl;
	
	// Print the CFG.
	function->PrintControlFlowGraph(cfg_verbose, cfg_vertex_ids);
	
	return true;
}
