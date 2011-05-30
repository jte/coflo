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

#ifndef PROGRAM_H
#define	PROGRAM_H

#include <vector>
#include <string>

class TranslationUnit;
class Function;

/**
 * Encapsulates the concept of an entire program, consisting of one or more
 * translation units.
 */
class Program
{
public:
	Program();
	Program(const Program& orig);
	virtual ~Program();
	
    void SetTheCtags(std::string the_ctags);
    void SetTheDot(std::string the_dot);
    void SetTheGcc(std::string the_gcc);
    void SetTheFilter(std::string the_filter);

	void AddSourceFiles(const std::vector< std::string > &file_paths);
	
	bool Parse(const std::vector< std::string > &defines,
		const std::vector< std::string > &include_paths,
		bool debug_parse = 0);
	
	void Print(const std::string &the_dot, const std::string &output_path);
	
private:

	/// The TranslationUnits which make up this Program.
	std::vector< TranslationUnit* > m_translation_units;
	
	/// The pre-preprocessor filter program.
	std::string m_the_filter;
	
	/// The gcc to use to get the actual CFG.
	std::string m_the_gcc;
	
	/// The dot program from the GraphViz program to use for generating
	/// the graph drawings.
	std::string m_the_dot;
	
	/// The ctags program to use to extract further information on the source.
	std::string m_the_ctags;
};

#endif	/* PROGRAM_H */

