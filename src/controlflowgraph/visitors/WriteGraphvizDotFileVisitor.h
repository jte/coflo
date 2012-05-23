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

class WriteGraphvizDotFileVisitor: public ControlFlowGraphVisitorBase
{
public:
	WriteGraphvizDotFileVisitor(ControlFlowGraph &g, std::ostream& out);
	virtual ~WriteGraphvizDotFileVisitor();

	virtual edge_return_value_t examine_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t tree_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t back_edge(ControlFlowGraph::edge_descriptor u);
	virtual edge_return_value_t forward_or_cross_edge(ControlFlowGraph::edge_descriptor u);
	virtual vertex_return_value_t discover_vertex(ControlFlowGraph::vertex_descriptor u);

private:

	void StreamOutEdgeAttributes(ControlFlowGraph::edge_descriptor u);

	/// Reference to the ostream we'll send our info to.
	std::ostream& m_out_stream;
};

#endif /* WRITEGRAPHVIZDOTFILEVISITOR_H_ */
