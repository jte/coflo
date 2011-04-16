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
 
#ifndef FUNCTION_H
#define FUNCTION_H
 
#include <string>
#include <vector>

class Block;
 
class Function
{
public:
	Function(const std::string &m_function_id);
	~Function();
	
	void AddBlock(Block *block);

	void LinkBlocks();

	void LinkIntoGraph();

	void Print();
	
private:

	/// Vertex properties for the Block graph.
	struct Vertex
	{
		Block *m_block;
	};
	
	/// Edge properties for the Block graph.
	struct Edge
	{
		std::string m_edge_text;
	};

	/// Typedef for the block graph.
	typedef boost::adjacency_list
			<boost::vecS,
			boost::vecS,
			boost::bidirectionalS,
			Vertex,
			Edge
			> T_BLOCK_GRAPH;

	/// The block graph.
	T_BLOCK_GRAPH m_block_graph;
	
	typedef T_BLOCK_GRAPH::vertex_descriptor VertexID;
	typedef T_BLOCK_GRAPH::edge_descriptor EdgeID;

	/// Function identifier.
	std::string m_function_id;

	/// Entry block.
	/// Only valid after LinkBlocks() has been called.
	Block *m_entry_block;
	
	/// Block list.
	std::vector < Block * > m_block_list;	
};
 
#endif // FUNCTION_H
