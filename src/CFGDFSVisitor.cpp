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

#include "CFGDFSVisitor.h"

CFGDFSVisitor::CFGDFSVisitor(T_CFG &g) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(g)
{
}

CFGDFSVisitor::CFGDFSVisitor(const CFGDFSVisitor& orig) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(orig)
{
}

CFGDFSVisitor::~CFGDFSVisitor()
{
}

CFGDFSVisitor::return_value_t CFGDFSVisitor::initialize_vertex(T_CFG_VERTEX_DESC u)
{
	return ok; 
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::start_vertex(T_CFG_VERTEX_DESC u)
{
	return ok;
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::discover_vertex(T_CFG_VERTEX_DESC u)
{
	return ok;
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::examine_edge(T_CFG_EDGE_DESC u)
{
	return ok;
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::tree_edge(T_CFG_EDGE_DESC u)
{
	return ok;
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::back_edge(T_CFG_EDGE_DESC u)
{
	return ok;
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::forward_or_cross_edge(T_CFG_EDGE_DESC u)
{
	return ok;
};

CFGDFSVisitor::return_value_t CFGDFSVisitor::finish_vertex(T_CFG_VERTEX_DESC u)
{
	return ok;
};
