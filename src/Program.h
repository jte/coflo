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
#include <map>

#include "controlflowgraph/ControlFlowGraph.h"

class TranslationUnit;
class Function;
class ToolCompiler;
class ToolDot;

/// Map of identifiers to pointers to the Function objects the correspond to.
typedef std::map< std::string, Function* > T_ID_TO_FUNCTION_PTR_MAP;

/// Map of function call identifiers to FunctionCallUnresolved instances.
typedef std::multimap< std::string, FunctionCallUnresolved*> T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP;

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
	
    void SetTheDot(ToolDot *the_dot);
    void SetTheGcc(ToolCompiler *the_compiler);
    void SetTheFilter(const std::string &the_filter);

	void AddSourceFiles(const std::vector< std::string > &file_paths);
	
	bool Parse(const std::vector< std::string > &defines,
		const std::vector< std::string > &include_paths,
		T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls,
		bool debug_parse = 0);
	
	void PrintUnresolvedFunctionCalls(T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls);

	/**
	 * Return a pointer to the Function object corresponding to the given identifier.
	 *
	 * @param function_id
	 * @return
	 */
	Function *LookupFunction(const std::string &function_id);
	
	/**
	 * Creates an HTML page containing graphical control flow graphs of all functions in the program.
	 *
	 * @param output_path
	 */
	void Print(const std::string &output_path);
	
	/**
	 * Prints the Control Flow Graph of the specified function to cout.
	 *
	 * @param function_identifier Identifier of the function whose CFG is to be printed.
	 * @param only_function_calls If true, only include function calls and control-flow-altering
	 *        constructs (if/switch) in the printed graph.
	 * @param cfg_vertex_ids If true, also print numeric vertex ID.
	 * @return
	 */
	bool PrintFunctionCFG(const std::string &function_identifier, bool only_function_calls, bool cfg_vertex_ids);
	
	/**
	 * Get a reference to the underlying T_CFG.
	 *
	 * @deprecated Should eventually not be necessary due to the encapsulation provided by ControlFlowGraph.
	 *
	 * @return A reference to the underlying T_CFG.
	 */
	//T_CFG& GetControlFlowGraph() { return m_cfg.GetT_CFG(); };
	
	ControlFlowGraph* GetControlFlowGraphPtr() { return &m_cfg; };

private:

	/// The TranslationUnits which make up this Program.
	std::vector< TranslationUnit* > m_translation_units;
	
	/// The pre-preprocessor filter program.
	std::string m_the_filter;
	
	/// The gcc to use to get the actual CFG.
	ToolCompiler *m_compiler;
	
	/// The dot program from the GraphViz program to use for generating
	/// the graph drawings.
	ToolDot *m_the_dot;
	
	/// The Control Flow Graph for the Program.
	ControlFlowGraph m_cfg;
	
	/// The identifier string to Function* map.
	T_ID_TO_FUNCTION_PTR_MAP m_function_map;
};

#endif	/* PROGRAM_H */

