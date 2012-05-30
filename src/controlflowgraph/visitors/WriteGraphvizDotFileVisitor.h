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

#ifndef WRITEGRAPHVIZDOTFILEVISITOR_H_
#define WRITEGRAPHVIZDOTFILEVISITOR_H_

#include <iosfwd>

#include "ControlFlowGraphVisitorBase.h"

/**
 * Visitor which, when sent on a depth-first traversal of the ControlFlowGraph of a Function, will print out a
 * Graphviz-dot-compatible file representing that Function's graph.
 */
class WriteGraphvizDotFileVisitor: public ControlFlowGraphVisitorBase
{
public:
	WriteGraphvizDotFileVisitor(ControlFlowGraph &g, std::ostream& out);
	virtual ~WriteGraphvizDotFileVisitor();

	virtual vertex_return_value_t start_vertex(ControlFlowGraph::vertex_descriptor u);
	virtual vertex_return_value_t discover_vertex(ControlFlowGraph::vertex_descriptor u);
	virtual edge_return_value_t examine_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t tree_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t back_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t forward_or_cross_edge(ControlFlowGraph::edge_descriptor u);

private:

	/**
	 * Helper function for streaming an edge's attributes to m_out_stream.
	 * During a depth-first search, we must output tree_edge(), back_edge(), and forward_or_cross_edge()
	 * attributes, and this helper cuts down on copy/paste code.
	 *
	 * @param u  Descriptor of the Edge whose attributes are to be inserted into m_out_stream.
	 */
	void StreamOutEdgeAttributes(ControlFlowGraph::edge_descriptor u);

	/// Reference to the ostream we'll send our info to.
	std::ostream& m_out_stream;
};

#endif /* WRITEGRAPHVIZDOTFILEVISITOR_H_ */
