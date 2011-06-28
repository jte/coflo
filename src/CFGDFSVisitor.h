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

#ifndef CFGDFSVISITOR_H
#define	CFGDFSVISITOR_H

#include "ImprovedDFSVisitorBase.h"
#include "ControlFlowGraph.h"

class CFGDFSVisitor : public ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>
{
public:
	CFGDFSVisitor(T_CFG &g);
	CFGDFSVisitor(const CFGDFSVisitor& orig);
	virtual ~CFGDFSVisitor();
	
	return_value_t initialize_vertex(T_CFG_VERTEX_DESC u);
	return_value_t start_vertex(T_CFG_VERTEX_DESC u);
	return_value_t discover_vertex(T_CFG_VERTEX_DESC u);
	return_value_t examine_edge(T_CFG_EDGE_DESC u);
	return_value_t tree_edge(T_CFG_EDGE_DESC u);
	return_value_t back_edge(T_CFG_EDGE_DESC u);
	return_value_t forward_or_cross_edge(T_CFG_EDGE_DESC u);
	return_value_t finish_vertex(T_CFG_VERTEX_DESC u);

private:

};

#endif	/* CFGDFSVISITOR_H */

