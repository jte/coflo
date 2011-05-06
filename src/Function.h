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

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "ControlFlowGraph.h"

class Block;

class Function
{
public:
	Function(const std::string &m_function_id);
	~Function();
	
	void AddBlock(Block *block);

	void LinkBlocks();

	void LinkIntoGraph();
	
	/**
	 * Link the unresolved function calls in this Function to the Functions
	 * in the passed \a function_map.
     */
	void Link(const std::map< std::string, Function* > &function_map);
	
	bool CreateControlFlowGraph();

	std::string GetIdentifier() const { return m_function_id; };
	
	void Print();
	
	void PrintDotCFG(const boost::filesystem::path& output_dir);
	
private:
	
	/// Remove the back edges from the graph.
	void RemoveBackEdges();

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
	
	/// Typedef for the vertex_descriptors in the block graph.
	typedef T_BLOCK_GRAPH::vertex_descriptor VertexID;
	
	/// Typedef for the edge_descriptors in the block graph.
	typedef T_BLOCK_GRAPH::edge_descriptor EdgeID;
	
	/// The block graph.
	T_BLOCK_GRAPH m_block_graph;
	
	/// The block graph with all back edges removed.
	T_BLOCK_GRAPH m_block_graph_no_back_edges;

	/// Function identifier.
	std::string m_function_id;

	/// Entry block.
	/// Only valid after LinkBlocks() has been called.
	Block *m_entry_block;
	
	/// Exit block.
	/// Only valid after LinkBlocks() has been called.
	Block *m_exit_block;
	
	/// Block list.
	std::vector < Block * > m_block_list;
	
	/// The first statement in the body of this function.
	CFGVertexID m_first_statement;
	
	/// The last statement in the body of this function.
	CFGVertexID m_last_statement;

	/// The Control Flow Graph for this Function.
	T_CFG m_cfg;
	
};
 
#endif // FUNCTION_H
