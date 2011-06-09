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

#ifndef TRANSLATIONUNIT_H
#define	TRANSLATIONUNIT_H

#include <string>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "ControlFlowGraph.h"
#include "Program.h"

class Function;
class FunctionCall;

/**
 * Class representing a single source file.
 * 
 * @param file_path
 */
class TranslationUnit
{
public:
    TranslationUnit(const std::string &file_path);
    TranslationUnit(const TranslationUnit& orig);
    virtual ~TranslationUnit();

	/**
	 * Parse the given source file, extracting the basic blocks.
	 * 
	 * @param filename The file to parse.
	 * @param function_map Map of identifiers to Function pointers to which the
	 *		functions found in this TranslationUnit are to be added.
	 * @param the_filter The filter command to invoke.
	 * @param the_gcc The compiler command to invoke.
	 * @param the_ctags The ctags command to invoke.
	 * @param debug_parse Whether to output debugging info during the parse stage.
	 * 
	 * @return true if the parse succeeded, false if it fails.
	 */
	bool ParseFile(const boost::filesystem::path &filename,
		T_ID_TO_FUNCTION_PTR_MAP *function_map,
		const std::string &the_filter,
		ToolCompiler *compiler,
		const std::string &the_ctags,
		const std::vector< std::string > &defines,
		const std::vector< std::string > &include_paths,
		bool debug_parse = false);

	bool LinkBasicBlocks();
	
	void Link(const std::map< std::string, Function* > &function_map,
		std::vector< FunctionCall* > *unresolved_function_calls);
	
	bool CreateControlFlowGraphs(T_CFG * cfg);

	void Print(const std::string &the_dot, const boost::filesystem::path &output_dir, std::ofstream & index_html_stream);
	
	std::string GetFilePath() const { return m_source_filename.string(); };
	
	long GetNumberOfFunctionDefinitions() const { return m_function_defs.size(); };

private:
	
	/**
	 * Compile the file with GCC to get the control flow decomposition we need.
	 * 
     * @param file_path  Path to the source file to be compiled.
     */
	void CompileSourceFile(const std::string& file_path, const std::string &the_filter, ToolCompiler *compiler,
						 const std::vector< std::string > &defines,
						const std::vector< std::string > &include_paths);

	/// The source filename.
	boost::filesystem::path m_source_filename;

	/// List of function definitions in this file.
	std::vector< Function* > m_function_defs;
};

#endif	/* TRANSLATIONUNIT_H */

