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

#include "Program.h"

#include <iostream>
#include <boost/foreach.hpp>

#include "TranslationUnit.h"
#include "RuleReachability.h"

Program::Program() { }

Program::Program(const Program& orig) { }

Program::~Program() { }


void Program::SetTheCtags(std::string the_ctags)
{
	m_the_ctags = the_ctags;
}

void Program::SetTheDot(std::string the_dot)
{
	m_the_dot = the_dot;
}

void Program::SetTheGcc(std::string the_gcc)
{
	m_the_gcc = the_gcc;
}

void Program::SetTheFilter(std::string the_filter)
{
	m_the_filter = the_filter;
}

void Program::AddSourceFiles(const std::vector< std::string > &file_paths)
{
	BOOST_FOREACH(std::string input_file_path, file_paths)
	{
		m_translation_units.push_back(new TranslationUnit(input_file_path));
	}
}

bool Program::Parse(const std::vector< std::string > &defines,
		const std::vector< std::string > &include_paths,
		bool debug_parse)
{
	BOOST_FOREACH(TranslationUnit *tu, m_translation_units)
	{
		T_ID_TO_FUNCTION_PTR_MAP function_map;

		// Parse this file.
		std::cout << "Parsing \"" << tu->GetFilePath() << "\"..." << std::endl;
		bool retval = tu->ParseFile(tu->GetFilePath(), &function_map,
								 m_the_filter, m_the_gcc, m_the_ctags, 
								 defines, include_paths, debug_parse);
		if(retval == false)
		{
			std::cerr << "ERROR: Couldn't parse \"" << tu->GetFilePath() << "\"" << std::endl;
			return false;
		}

		// Link the blocks in the functions in the file.
		std::cout << "Linking basic blocks..." << std::endl;
		retval = tu->LinkBasicBlocks();
		if(retval == false)
		{
			std::cerr << "ERROR: Couldn't parse \"" << tu->GetFilePath() << "\"" << std::endl;
			return false;
		}

		// Create the control-flow graphs.
		std::cout << "Creating function control-flow graphs..." << std::endl;
		tu->CreateControlFlowGraphs();

		// Link the function calls.
		std::cout << "Linking function calls..." << std::endl;
		tu->Link(function_map);	
#if 0


		/// @todo This is a test.
		Function *start, *stop;
		start = function_map["main"];
		stop = function_map["another_level_deep"];
		//stop = function_map["ISR1"];
		RuleReachability rr(start, stop);
		rr.RunRule(tu->m_cfg);
#endif	
	}
}

void Program::Print(const std::string &the_dot, const std::string &output_path)
{
	BOOST_FOREACH(TranslationUnit *tu, m_translation_units)
	{
		tu->Print(m_the_dot, output_path);
	}
}
