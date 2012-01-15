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

/** @file */

#ifndef FUNCTION_H
#define FUNCTION_H
 
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "controlflowgraph/ControlFlowGraph.h"

class TranslationUnit;
class FunctionCall;
class ToolDot;

/// Map of function call identifiers to FunctionCallUnresolved instances.
typedef std::multimap< std::string, FunctionCallUnresolved*> T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP;

/**
 * Class representing a single function in the source.
 */
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
			T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls);
	
	/**
	 * Add the control flow graph of this Function to \a cfg.
	 * 
     * @param cfg The ControlFlowGraph to add this function's control-flow graph to.
     * @return true on success, false on failure.
     */
	//bool CreateControlFlowGraph(ControlFlowGraph &cfg);
	
	bool CreateControlFlowGraph(ControlFlowGraph &cfg, const std::vector< StatementBase* > &statement_list);

	/**
	 * Return this Function's identifier.
	 *
	 * @return Identifier of this Function.
	 */
	std::string GetIdentifier() const { return m_function_id; };
	
	std::string GetDefinitionFilePath() const;

	/// @name Control Flow Graph Rendering Functions
	//@{

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
	 * Writes the control flow graph of this function as a Graphviz dot file to @a output_filename.
	 *
	 * @param cfg_verbose
	 * @param cfg_vertex_ids
	 * @param output_filename
	 */
	void PrintControlFlowGraphDot(bool cfg_verbose, bool cfg_vertex_ids, const std::string &output_filename);

	/**
	 * Print the control flow graph of this function to a dot file in @a output_dir, and
	 * run the dot too to generate the resulting png file.
	 *
	 * @param the_dot
	 * @param output_filename The filename of the generated png file.
	 */
	void PrintControlFlowGraphBitmap(ToolDot *the_dot, const boost::filesystem::path& output_filename);
	
	//@}

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

	T_CFG_EDGE_DESC GetEntrySelfEdgeDescriptor() const { return m_entry_vertex_self_edge; };

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
	
	/**
	 * Structure for capturing info regarding basic block leaders.
	 * Used in the creation of a Function's CFG.
	 */
	struct BasicBlockLeaderInfo
	{
		BasicBlockLeaderInfo(T_CFG_VERTEX_DESC leader, T_CFG_VERTEX_DESC immediate_predecessor)
		{
			m_leader = leader;
			m_immediate_predecessor = immediate_predecessor;
		};

		/// Vertex descriptor of the basic block leader.
		T_CFG_VERTEX_DESC m_leader;

		/// The vertex which ended the immediately-preceding basic block.
		T_CFG_VERTEX_DESC m_immediate_predecessor;
	};

	/**
	 * Add any necessary Impossible edges to the CFG.
	 *
	 * These can be necessary when the source contains infinite loops or dead code.
	 *
	 * @param cfg
	 * @param leader_info_list
	 */
	void AddImpossibleEdges(ControlFlowGraph & cfg, std::vector< BasicBlockLeaderInfo > &leader_info_list);

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

	/// @name Static properties of this function.
	/// These are properties of the function determined at analysis-time which are invariant, such as
	/// whether it is known to terminate, its complexity, etc.
	//@{
	//@}
};
 
#endif // FUNCTION_H
