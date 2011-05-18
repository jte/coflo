/**
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

#ifndef BASICBLOCKGRAPH_H
#define	BASICBLOCKGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

class Block;

/// Vertex properties for the Block graph.
struct BasicBlockGraphVertexProperties
{
	Block *m_block;
};

/// Edge properties for the Block graph.
struct BasicBlockGraphEdgeProperties
{
	std::string m_edge_text;
};

/// Typedef for the block graph.
typedef boost::adjacency_list
		<boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		BasicBlockGraphVertexProperties,
		BasicBlockGraphEdgeProperties
		> T_BLOCK_GRAPH;

/// Typedef for the vertex_descriptors in the block graph.
typedef T_BLOCK_GRAPH::vertex_descriptor T_BLOCK_GRAPH_VERTEX_DESC;

/// Typedef for the edge_descriptors in the block graph.
typedef T_BLOCK_GRAPH::edge_descriptor T_BLOCK_GRAPH_EDGE_DESC;

#endif	/* BASICBLOCKGRAPH_H */

