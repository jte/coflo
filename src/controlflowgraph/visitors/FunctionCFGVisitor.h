/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#ifndef FUNCTIONCFGVISITOR_H_
#define FUNCTIONCFGVISITOR_H_

#include "ControlFlowGraphVisitorBase.h"

#include "../CallStackFrameBase.h"

/**
 * Visitor which, when passed to topological_visit_kahn, prints out the control flow graph.
 */
class FunctionCFGVisitor: public ControlFlowGraphVisitorBase
{
public:
	FunctionCFGVisitor(ControlFlowGraph &g,
			ControlFlowGraph::vertex_descriptor last_statement,
			bool cfg_verbose,
			bool cfg_vertex_ids) :
			ControlFlowGraphVisitorBase(g)
	{
		m_last_statement = last_statement;
		m_cfg_verbose = cfg_verbose;
		m_cfg_vertex_ids = cfg_vertex_ids;
		m_last_discovered_vertex_is_recursive = false;
		m_indent_level = 0;
	};
	FunctionCFGVisitor(const FunctionCFGVisitor &original) :
			ControlFlowGraphVisitorBase(original)
	{
	};
	virtual ~FunctionCFGVisitor();

	vertex_return_value_t start_vertex(ControlFlowGraph::edge_descriptor u);

	vertex_return_value_t discover_vertex(ControlFlowGraph::vertex_descriptor u, ControlFlowGraph::edge_descriptor e);

	edge_return_value_t examine_edge(ControlFlowGraph::edge_descriptor ed);

	void vertex_visit_complete(ControlFlowGraph::vertex_descriptor u, long num_vertices_pushed, ControlFlowGraph::edge_descriptor e);

private:

	/// Vertex corresponding to the last statement of the function.
	/// We'll terminate the search when we find this.
	ControlFlowGraph::vertex_descriptor m_last_statement;

	/// Flag indicating if we should only print function calls and flow control constructs.
	bool m_cfg_verbose;

	/// Flag indicating if we should print the vertex ID.
	bool m_cfg_vertex_ids;

	/// The current indentation level of the output control flow graph.
	/// This is affected by both intra-function branch-producing instructions (if()'s and switch()'s) and
	/// by inter-Function operations (function calls).
	long m_indent_level;

	bool m_last_discovered_vertex_is_recursive;
};

#endif /* FUNCTIONCFGVISITOR_H_ */
