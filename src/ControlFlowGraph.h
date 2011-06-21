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

#include "statements/Statement.h"
#include "controlflowgraph/CFGEdgeTypeBase.h"

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

/// Typedef for the vertex_descriptors in the control flow graph.
typedef boost::graph_traits<T_CFG>::vertex_descriptor T_CFG_VERTEX_DESC;

/// Typedef for the edge_descriptors in the control flow graph.
typedef boost::graph_traits<T_CFG>::edge_descriptor T_CFG_EDGE_DESC;

/// Typedef for vertex iterators for the CFG.
typedef boost::graph_traits< T_CFG >::vertex_iterator T_CFG_VERTEX_ITERATOR;

/// Typedef for vertex iterators for the CFG.
typedef boost::graph_traits< T_CFG >::out_edge_iterator T_CFG_OUT_EDGE_ITERATOR;

template < typename CFGEdgeType >
boost::tuple<T_CFG_EDGE_DESC, bool> GetFirstOutEdgeOfType(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg)
{
	boost::graph_traits< T_CFG >::out_edge_iterator eit, eend;
	boost::tuple<T_CFG_EDGE_DESC, bool> retval;
	
	boost::tie(eit, eend) = boost::out_edges(vdesc, cfg);
	for(; eit != eend; eit++)
	{
		if(NULL != dynamic_cast<CFGEdgeType*>(cfg[*eit].m_edge_type))
		{
			// Found it.
			retval = boost::make_tuple(*eit, true);
			return retval;
		}
	}

	// Couldn't find one.
	retval = boost::make_tuple(*eit, false);
	return retval;
}

//@}

#endif	/* CONTROLFLOWGRAPH_H */
