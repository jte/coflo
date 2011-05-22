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

#ifndef CONTROLFLOWGRAPH_H
#define	CONTROLFLOWGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "Statement.h"
#include "CFGEdgeTypeBase.h"

class Function;

/// \name Control Flow Graph definitions.
//@{
	
/// Vertex properties for the CFG graph.
struct CFGVertexProperties
{
	/// The Statement at this vertex of the CFG.
	Statement *m_statement;
	
	/// The Function which contains this vertex.
	Function *m_containing_function;
};

/// Edge properties for the CFG graph.
struct CFGEdgeProperties
{
	/// The Edge Type.
	/// May be a FunctionCall, fallthrough, goto, etc.
	CFGEdgeTypeBase *m_edge_type;
};

/// Typedef for the CFG graph.
typedef boost::adjacency_list
		<boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		CFGVertexProperties,
		CFGEdgeProperties
		> T_CFG;

/// Typedef for the vertex_descriptors in the block graph.
typedef boost::graph_traits<T_CFG>::vertex_descriptor T_CFG_VERTEX_DESC;

/// Typedef for the edge_descriptors in the block graph.
typedef boost::graph_traits<T_CFG>::edge_descriptor T_CFG_EDGE_DESC;

//@}

#endif	/* CONTROLFLOWGRAPH_H */
