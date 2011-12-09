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


#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/foreach.hpp>

#include "Program.h"
#include "TranslationUnit.h"
//#include "RuleReachability.h"
#include "controlflowgraph/statements/FunctionCall.h"
#include "Function.h"

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

void Program::Print(const std::string &output_path)
{
	boost::filesystem::path output_dir = output_path;
	
	std::cout << "Creating output dir: " << output_dir << std::endl;
	mkdir(output_dir.string().c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
	
	std::string index_html_filename = (output_dir /= "index.html").generic_string();
	std::ofstream index_html_out(index_html_filename.c_str());

	index_html_out << \
"\
<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
	<meta charset=\"utf-8\">\n\
	<title>CoFlo Analysis Results</title>\n\
</head>\n\
<body>\n\
<h1>CoFlo Analysis Results</h1>" << std::endl;
	
	BOOST_FOREACH(TranslationUnit *tu, m_translation_units)
	{
		tu->Print(m_the_dot, output_path, index_html_out);
	}
	
	index_html_out << \
"\
</body>\n\
</html>" << std::endl;
}

void Program::PrintUnresolvedFunctionCalls(T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls)
{
	/// @todo Pass in.
	bool only_list_ids = true;

	// See if we have any unresolved calls.
	if(unresolved_function_calls->size() > 0)
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
