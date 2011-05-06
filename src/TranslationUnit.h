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

#ifndef TRANSLATIONUNIT_H
#define	TRANSLATIONUNIT_H

#include <string>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

class Function;

typedef std::map< std::string, Function* > T_ID_TO_FUNCTION_PTR_MAP;

class TranslationUnit
{
public:
    TranslationUnit();
    TranslationUnit(const TranslationUnit& orig);
    virtual ~TranslationUnit();

	/**
	 * Parse the given source file, extracting the basic blocks.
	 */
	bool ParseFile(const boost::filesystem::path &filename,
		T_ID_TO_FUNCTION_PTR_MAP *function_map,
		const std::string &the_filter,
		const std::string &the_gcc,
		const std::string &the_ctags,
		bool debug_parse = false);

	bool LinkBasicBlocks();
	
	void Link(const std::map< std::string, Function* > &function_map);
	
	bool CreateControlFlowGraphs();

	void Print(const boost::filesystem::path &output_dir);

private:
	
	/**
	 * Compile the file with GCC to get the control flow decomposition we need.
	 * 
     * @param file_path
     */
	void CompileSourceFile(const std::string& file_path, const std::string &the_filter, const std::string &the_gcc);

	/// The source filename.
	boost::filesystem::path m_source_filename;

	/// List of function definitions in this file.
	std::vector< Function * > m_function_defs;

};

#endif	/* TRANSLATIONUNIT_H */

