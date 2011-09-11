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
 
#ifndef FUNCTION_H
#define FUNCTION_H
 
#include <string>
#include <vector>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "BasicBlockGraph.h"
#include "controlflowgraph/ControlFlowGraph.h"

class TranslationUnit;
class Block;
class FunctionCall;
class ToolDot;
typedef std::vector< FunctionCall* > T_UNRESOLVED_FUNCTION_CALL_MAP;

class Function
{
public:
	Function(TranslationUnit *parent_tu, const std::string &m_function_id);
	~Function();
	
	void AddBlock(Block *block);

	void LinkBlocks();

	void LinkIntoGraph();
	
	/**
	 * Link the unresolved function calls in this Function to the Functions
	 * in the passed \a function_map.
	 * 
	 * @function_map The identifier->Function map to use to find the Functions to
	 * link to.
	 * @unresolved_function_calls List of function calls we weren't able to resolve.
     */
	void Link(const std::map< std::string, Function* > &function_map,
			T_UNRESOLVED_FUNCTION_CALL_MAP *unresolved_function_calls);
	
	/**
	 * Add the control flow graph of this Function to \a cfg.
	 * 
     * @param cfg The T_CFG to add this function's control-flow graph to.
     * @return true on success, false on failure.
     */
	bool CreateControlFlowGraph(T_CFG & cfg);
	
	std::string GetIdentifier() const { return m_function_id; };
	
	std::string GetDefinitionFilePath() const;

	/**
	 * Prints the control flow graph of this function to cout.
	 *
	 * @param only_function_calls  If true, only print function calls and flow control constructs.
	 */
	void PrintControlFlowGraph(bool only_function_calls);
	
	void PrintDotCFG(ToolDot *the_dot, const boost::filesystem::path& output_dir);
	
	/// @name Function Properties
	/// These are various properties of the function represented by this class instance.
	/// They may have been determined by CoFlow analytically, stated by the user through
	/// annotations or via other means, etc.
	//@{
	
	/**
	 * Determine if this Function is ever called.
	 * 
     * @return true if this Function is called.
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
	
//protected:
	
	T_CFG_VERTEX_DESC GetEntryVertexDescriptor() const { return m_first_statement; };
	T_CFG_VERTEX_DESC GetExitVertexDescriptor() const { return m_last_statement; };
	
private:
	
	void FixupBackEdges();

	/// The translation unit containing this function.
	TranslationUnit *m_parent_tu;

	/// The block graph.
	T_BLOCK_GRAPH m_block_graph;
	
	/// The block graph with all back edges removed.
	T_BLOCK_GRAPH m_block_graph_no_back_edges;

	/// Function identifier.
	std::string m_function_id;

	/// Entry block.
	/// Only valid after LinkBlocks() has been called.
	Block *m_entry_block;
	
	/// Exit block.
	/// Only valid after LinkBlocks() has been called.
	Block *m_exit_block;
	
	/// Block list.
	std::vector < Block * > m_block_list;
	
	/// The first statement in the body of this function.
	T_CFG_VERTEX_DESC m_first_statement;
	T_CFG_EDGE_DESC m_first_statement_self_edge;
	
	/// The last statement in the body of this function.
	T_CFG_VERTEX_DESC m_last_statement;
	T_CFG_EDGE_DESC m_last_statement_self_edge;
	
	T_CFG *m_cfg;
};
 
#endif // FUNCTION_H
