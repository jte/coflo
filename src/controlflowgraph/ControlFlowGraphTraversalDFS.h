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

#ifndef CONTROLFLOWGRAPHTRAVERSALDFS_H_
#define CONTROLFLOWGRAPHTRAVERSALDFS_H_

#include "ControlFlowGraphTraversalBase.h"

class ControlFlowGraphVisitorBase;

/**
 * Depth-first search traversal of the CFG.
 */
class ControlFlowGraphTraversalDFS : public ControlFlowGraphTraversalBase
{
public:
	ControlFlowGraphTraversalDFS(ControlFlowGraph &control_flow_graph);
	virtual ~ControlFlowGraphTraversalDFS();

	virtual void Traverse(typename boost::graph_traits<T_CFG>::vertex_descriptor source,
			ControlFlowGraphVisitorBase *visitor);

protected:
	virtual bool SkipEdge(typename boost::graph_traits<T_CFG>::edge_descriptor e);
};

#endif /* CONTROLFLOWGRAPHTRAVERSALDFS_H_ */
