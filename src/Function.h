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

#ifndef FUNCTION_H
#define FUNCTION_H
 
#include <string>
#include <vector>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "controlflowgraph/ControlFlowGraph.h"

class TranslationUnit;
class FunctionCall;
class ToolDot;
typedef std::vector< FunctionCallUnresolved* > T_UNRESOLVED_FUNCTION_CALL_MAP;


class Function
{
public:
	Function(TranslationUnit *parent_tu, const std::string &m_function_id);
	~Function();
	
	/**
	 * Link the unresolved function calls in this Function to the Functions
	 * in the passed \a function_map.
	 * 
	 * @param function_map The identifier->Function map to use to find the Functions to
	 * link to.
	 * @param[out] unresolved_function_calls List of function calls we weren't able to resolve.
     */
	void Link(const std::map< std::string, Function* > &function_map,
			T_UNRESOLVED_FUNCTION_CALL_MAP *unresolved_function_calls);
	
	/**
	 * Add the control flow graph of this Function to \a cfg.
	 * 
     * @param cfg The ControlFlowGraph to add this function's control-flow graph to.
     * @return true on success, false on failure.
     */
	bool CreateControlFlowGraph(ControlFlowGraph &cfg);
	
	bool CreateControlFlowGraph(ControlFlowGraph &cfg, const std::vector< StatementBase* > &statement_list);

	/**
	 * Return this Function's identifier.
	 *
	 * @return Identifier of this Function.
	 */
	std::string GetIdentifier() const { return m_function_id; };
	
	std::string GetDefinitionFilePath() const;

	/**
	 * Prints the control flow graph of this function to stdout.
	 *
	 * @param cfg_verbose  If true, output all statements and nodes CoFlo finds in the control flow graph.
	 *   If false, limit output to function calls and flow control constructs only.
	 * @param cfg_vertex_ids If true, print the numeric IDs of the control flow graph vertices.
	 *   This can help when comparing graphical and textual representations.
	 */
	void PrintControlFlowGraph(bool cfg_verbose, bool cfg_vertex_ids);
	
	/**
	 * Print the control flow graph of this function to a dot file in @a output_dir, and
	 * run the dot too to generate the resulting png file.
	 *
	 * @param the_dot
	 * @param output_dir The output directory where the dot and png files should be placed.
	 */
	void PrintDotCFG(ToolDot *the_dot, const boost::filesystem::path& output_dir);
	
	/// @name Function Properties
	/// These are various properties of the function represented by this class instance.
	/// They may have been determined by CoFlow analytically, stated by the user through
	/// annotations or via other means, etc.
	//@{
	
	/**
	 * Determine if this Function is ever called.
	 * 
     * @return true If this Function is called.
     */
	bool IsCalled() const;
	
	/// @todo Probably should have the following:
	/// IsPure()		(Has no side effects, but may read global memory)
	/// IsConst()		(IsPure() but with the further restriction that it can't access global memory).
	/// IsDeprecated()	(Shouldn't be used anymore.)
	/// IsEntryPoint()	(is an entry point of the program, e.g. main(), an ISR, etc.
	/// IsExitPoint()	(is an exit point.  Not sure of the semantics of this.)
	/// GetComplexity()	(Big-O notation stuff, probably want time, space).
	/// GetStackUsage()	(How much stack the function uses, whether it's bounded.)
	
	//@}
	
	
	/**
	 * Get the T_CFG_VERTEX_DESC corresponding to the Entry vertex of this Function.
	 *
	 * @return T_CFG_VERTEX_DESC corresponding to the Entry vertex of this Function
	 */
	T_CFG_VERTEX_DESC GetEntryVertexDescriptor() const { return m_entry_vertex_desc; };

	/**
	 * Get the T_CFG_VERTEX_DESC corresponding to the Exit vertex of this Function.
	 *
	 * @return T_CFG_VERTEX_DESC corresponding to the Exit vertex of this Function.
	 */
	T_CFG_VERTEX_DESC GetExitVertexDescriptor() const { return m_exit_vertex_desc; };
	
	/// @name Debugging helpers
	//@{
	void DumpCFG();
	//@}

private:
	
	bool CheckForNoInEdges(ControlFlowGraph & cfg,
			std::vector< T_CFG_VERTEX_DESC > &list_of_statements_with_no_in_edge_yet,
			std::vector< T_CFG_VERTEX_DESC > *output);

	/// The translation unit containing this function.
	TranslationUnit *m_parent_tu;

	/// Function identifier.
	std::string m_function_id;

	/// The first statement in the body of this function.
	T_CFG_VERTEX_DESC m_entry_vertex_desc;
	T_CFG_EDGE_DESC m_entry_vertex_self_edge;
	
	/// The last statement in the body of this function.
	T_CFG_VERTEX_DESC m_exit_vertex_desc;
	T_CFG_EDGE_DESC m_exit_vertex_self_edge;
	
	ControlFlowGraph *m_the_cfg;
	T_CFG *m_cfg;
};
 
#endif // FUNCTION_H
