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
 
#include <iostream>
#include <map>
#include <typeinfo>

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include "Block.h"

#include "Function.h"
#include "SuccessorTypes.h"

typedef std::map< long, Block * > T_LINK_MAP;
typedef T_LINK_MAP::iterator T_LINK_MAP_ITERATOR;
typedef T_LINK_MAP::value_type T_LINK_MAP_VALUE;
 
Function::Function(const std::string &function_id)
{
	m_function_id = function_id;
	m_entry_block = NULL;
}
 
Function::~Function()
{
}
 
void Function::AddBlock(Block *block)
{
	m_block_list.push_back(block);
}

void Function::LinkBlocks()
{
	// A map of the blocks constituting this function.
	T_LINK_MAP linkmap;
	std::vector< Successor* > successor_list;

	// Go through all the blocks and add them to the link map.
	BOOST_FOREACH(Block *bp, m_block_list)
	{
		// Add this block to the number->Block* map.
		linkmap[bp->GetBlockNumber()] = bp;

		// Collect all the blocks' Successors into a list.
		Block::T_BLOCK_SUCCESSOR_ITERATOR s;
		for(s = bp->successor_begin(); s != bp->successor_end(); s++)
		{
			if(NULL != dynamic_cast<SuccessorExit*>(*s))
			{
				std::cout << "INFO: Found EXIT successor." << std::endl;
				(*s)->SetSuccessorBlockPtr(NULL);
			}
			else
			{
				successor_list.push_back(*s);
			}
		}
	}

	// Block 2 appears to always be the entry block, set it.
	/// \todo Check for "PRED: ENTRY" to make sure of this.
	T_LINK_MAP_ITERATOR lmit = linkmap.find(2);
	if(lmit == linkmap.end())
	{
		std::cerr << "ERROR: Can't find Block 2." << std::endl;
	}
	m_entry_block = lmit->second;

	// Now go through all the Successors and link them to the Blocks they refer to.
	std::vector< Successor* >::iterator it2;
	for(it2 = successor_list.begin(); it2 != successor_list.end(); it2++)
	{
		long block_no;
		Block *block_ptr;
		T_LINK_MAP_ITERATOR linkmap_it;

		block_no = (*it2)->GetSuccessorBlockNumber();

		// Look up the block in the map.
		linkmap_it = linkmap.find(block_no);

		if(linkmap_it == linkmap.end())
		{
			std::cerr << "ERROR: Can't find Block " << block_no << " in linkmap." << std::endl;
		}
		else if(linkmap_it->second == NULL)
		{
			std::cerr << "ERROR: Found a Successor with no Block pointer." << std::endl;
		}
		else
		{
			// Found the referenced block, assign a pointer directly to it.
			(*it2)->SetSuccessorBlockPtr(linkmap_it->second);
		}
	}

	LinkIntoGraph();
}

void Function::LinkIntoGraph()
{
	// Block*->VertexID map.
	typedef std::map< Block*, VertexID > T_BLOCK_MAP;
	T_BLOCK_MAP block_map;	
	
	// Add the function blocks into the m_block_graph.	
	BOOST_FOREACH(Block *bp, m_block_list)
	{
		// this_block_id is the new Vertex.
		VertexID this_block_id = boost::add_vertex(m_block_graph);
		m_block_graph[this_block_id].m_block = bp;
		
		// Add the block ID to a temporary map for use in linking below.
		block_map[bp] = this_block_id;
	}

	// Iterate over each block again, this time adding the edges between blocks.
	BOOST_FOREACH(Block *bp, m_block_list)
	{
		VertexID this_block = block_map[bp];
		VertexID next_block;

		// Go through all the successors.
		Block::T_BLOCK_SUCCESSOR_ITERATOR s;
		for(s = bp->successor_begin(); s != bp->successor_end(); s++)
		{
			T_BLOCK_MAP::iterator it;
			it = block_map.find((*s)->GetSuccessorBlockPtr());
			if(it != block_map.end())
			{
				// Found the next block.
				next_block = it->second;
				EdgeID edge;
				bool ok;
				boost::tie(edge, ok) = boost::add_edge(this_block, next_block, m_block_graph);
				// Check if there's no error, then set up the edge properties.
				if (ok)
				{
					m_block_graph[edge].m_edge_text = "TODO";
				}
			}
			else
			{
				// Couldn't find the next block, it's probably EXIT.
				std::cerr << "ERROR: Couldn't find next_block in block_map" << std::endl;
			}
		}
	}
}

struct graph_property_writer
{
	void operator()(std::ostream& out) const
	{
		out << "graph [clusterrank=local]" << std::endl;
		out << "node [shape=rectangle]" << std::endl;
		out << "edge [style=solid]" << std::endl;
	}
};

	// Class template of a vertex property writer
template < typename Graph >
class vertex_property_writer
{
public:
	vertex_property_writer(Graph _g) : g(_g) {}
	template <typename Vertex>
	void operator()(std::ostream& out, const Vertex& v) 
	{
		out << "[label=\"";
		if(g[v].m_block != NULL)
		{
			out << g[v].m_block->GetBlockNumber();
		}
		else
		{
			out << "UNKNOWN";
		}
		out << "\"]";
	}
private:
	Graph& g;
};
	
void Function::Print()
{
	long indent_level = 0;
	
	// Print the function info.
	std::cout << "Function Definition: " << m_function_id << std::endl;

	boost::write_graphviz(std::cout, m_block_graph,
						 vertex_property_writer<T_BLOCK_GRAPH>(m_block_graph),
						 boost::default_writer(),
						 graph_property_writer());

	typedef std::vector< VertexID > T_SORTED_BLOCK_CONTAINER;
	T_SORTED_BLOCK_CONTAINER topologically_sorted_blocks;
	boost::topological_sort(m_block_graph, std::back_inserter(topologically_sorted_blocks));
	
	// Print the function's blocks.
	for(T_SORTED_BLOCK_CONTAINER::reverse_iterator rit = topologically_sorted_blocks.rbegin();
		rit != topologically_sorted_blocks.rend();
		rit++)
	{
		if(m_block_graph[*rit].m_block != NULL)
		{
			std::cout << m_block_graph[*rit].m_block->GetBlockNumber() << std::endl;

			// If we have more than one in-edge, that means we're collapsing down
			// from a higher level of branching, so decrese the indent.
			if(boost::in_degree(*rit, m_block_graph) > 1)
			{
				indent_level--;
			}
			
			// Print the block.
			m_block_graph[*rit].m_block->PrintBlock(1+indent_level);
			
			// If we have more than one out-edge, that means this was a decision
			// block of some sort, with multiple alternative outgoing control flow paths.
			// Increase the indent level.
			if(boost::out_degree(*rit, m_block_graph) > 1)
			{
				indent_level++;
			}
		}
		else
		{
			std::cout << "INFO: m_block_graph[" << *rit << "].m_block == NULL" << std::endl;
		}
	}
}
