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

#include "ControlFlowGraphVisitorBase.h"

#if 0
ControlFlowGraphVisitorBase::ControlFlowGraphVisitorBase(ControlFlowGraph & cfg) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(cfg.GetT_CFG()), m_cfg(cfg)
{
}
#endif

ControlFlowGraphVisitorBase::ControlFlowGraphVisitorBase(T_CFG &g) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(g)
{
}

ControlFlowGraphVisitorBase::ControlFlowGraphVisitorBase(const ControlFlowGraphVisitorBase& orig) : ImprovedDFSVisitorBase<T_CFG_VERTEX_DESC, T_CFG_EDGE_DESC, T_CFG>(orig)
{
}

ControlFlowGraphVisitorBase::~ControlFlowGraphVisitorBase()
{
}

vertex_return_value_t ControlFlowGraphVisitorBase::initialize_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok; 
};

vertex_return_value_t ControlFlowGraphVisitorBase::start_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok;
};

vertex_return_value_t ControlFlowGraphVisitorBase::discover_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::examine_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::tree_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::back_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

edge_return_value_t ControlFlowGraphVisitorBase::forward_or_cross_edge(T_CFG_EDGE_DESC u)
{
	return edge_return_value_t::ok;
};

vertex_return_value_t ControlFlowGraphVisitorBase::finish_vertex(T_CFG_VERTEX_DESC u)
{
	return vertex_return_value_t::ok;
};
