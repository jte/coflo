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
#include <fstream>
#include <map>
#include <typeinfo>
#include <cstdlib>

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include "Block.h"

#include "Function.h"
#include "SuccessorTypes.h"
#include "Statement.h"

typedef std::map< long, Block * > T_LINK_MAP;
typedef T_LINK_MAP::iterator T_LINK_MAP_ITERATOR;
typedef T_LINK_MAP::value_type T_LINK_MAP_VALUE;
 
Function::Function(const std::string &function_id)
{
	m_function_id = function_id;
	m_entry_block = NULL;
	m_exit_block = NULL;
	
	// Add the entry and exit blocks.
	m_entry_block = new Block(this, 0, 0);
	m_exit_block = new Block(this, 1, 0);
	m_block_list.push_back(m_entry_block);
	m_block_list.push_back(m_exit_block);
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
	
	// Add the entry pseudoblock.
	m_entry_block->AddSuccessors("2 (fallthru)");
	
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
				(*s)->SetSuccessorBlockPtr(m_exit_block);
			}
			else
			{
				successor_list.push_back(*s);
			}
		}
	}

	// Block 2 appears to always be the entry block, set it.
	/// \todo We really should check for "PRED: ENTRY" to make sure of this.
	T_LINK_MAP_ITERATOR lmit = linkmap.find(2);
	if(lmit == linkmap.end())
	{
		std::cerr << "ERROR: Can't find Block 2." << std::endl;
	}
	// = lmit->second;

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
					// Label the edge with whatever the Successor object has
					// for a label.
					if((*s)->HasEdgeLabel())
					{
						m_block_graph[edge].m_edge_text = (*s)->GetEdgeLabel();
					}
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

/// Functor for writing GraphViz dot-compatible info for the function's entire CFG.
struct graph_property_writer
{
	void operator()(std::ostream& out) const
	{
		out << "graph [clusterrank=local]" << std::endl;
		out << "node [shape=rectangle]" << std::endl;
		out << "edge [style=solid]" << std::endl;
	}
};

/// Class template of a vertex property writer, for use with write_graphviz().
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
			out << g[v].m_block->GetBlockLabel();
			
			Block::T_STATEMENT_LIST_ITERATOR sit;
			for(sit = g[v].m_block->begin(); sit != g[v].m_block->end(); sit++)
			{
				out << "\\n" << (*sit)->GetStatementTextDOT();
			}
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

/// Class template of a vertex property writer, for use with write_graphviz().
template < typename Graph >
class cfg_vertex_property_writer
{
public:
	cfg_vertex_property_writer(Graph _g) : g(_g) {}
	template <typename Vertex>
	void operator()(std::ostream& out, const Vertex& v) 
	{
		if(g[v].m_statement != NULL)
		{
			out << g[v].m_statement->GetStatementTextDOT();
		}
		else
		{
			out << "[label=\"NULL STMNT\"]";
		}
	}
private:
	Graph& g;
};

/// Class template of an edge property writer, for use with write_graphviz().
template < typename Graph >
class edge_property_writer
{
public:
	edge_property_writer(Graph _g) : g(_g) {}
	template <typename Edge>
	void operator()(std::ostream& out, const Edge& e) 
	{
		// Only label the edge if we have some text to label it with.
		if(g[e].m_edge_text.empty() == false)
		{
			out << "[label=\"";
			out << g[e].m_edge_text;
			out << "\"]";
		}
	}
private:
	Graph& g;
};

template < typename Edge, typename Graph >
class dfs_back_edge_collector_visitor : public boost::default_dfs_visitor
{
public:
	dfs_back_edge_collector_visitor(std::vector< Edge > *back_edge_list) : boost::default_dfs_visitor()
	{ 
		// Save the pointer to the back edge list to put the results in.
		m_back_edge_list = back_edge_list;
	};
	
	void back_edge(Edge e, const Graph & g) const
	{
		std::cout << "Marking BackEdge" << std::endl;
		
		// Record this edge as a back edge.
		m_back_edge_list->push_back(e);
	}
	
private:
	/// Pointer to the edge list we'll add any back edges to.
	std::vector< Edge > *m_back_edge_list;
};


void Function::RemoveBackEdges()
{
	// List of back edges that we'll delete.
	std::vector< EdgeID > m_back_edge_list;
	
	// The instance of the DFS visitor which we'll pass to the DFS.
	dfs_back_edge_collector_visitor<EdgeID, T_BLOCK_GRAPH> vis(&m_back_edge_list);
	
	// Find all the back edges.
	boost::depth_first_search(m_block_graph, boost::visitor(vis));
	
	// Remove the back edges.
	BOOST_FOREACH(EdgeID e, m_back_edge_list)
	{
		std::cerr << "Removing back edge." << std::endl;
		boost::remove_edge(e, m_block_graph);
	}
}

void Function::Print()
{
	long indent_level = 0;
	
	// Print the function info.
	std::cout << "Function Definition: " << m_function_id << std::endl;

	boost::write_graphviz(std::cout, m_block_graph,
						 vertex_property_writer<T_BLOCK_GRAPH>(m_block_graph),
						 edge_property_writer<T_BLOCK_GRAPH>(m_block_graph),
						 graph_property_writer());
	
	// Remove all back edges before we do the topological sort, since it can't
	// handle cycles.
	RemoveBackEdges();
	
	typedef std::vector< VertexID > T_SORTED_BLOCK_CONTAINER;
	T_SORTED_BLOCK_CONTAINER topologically_sorted_blocks;
	// vector for storing the indent level for each block.
	std::vector< long > block_indent_level;
	
	// Do a topological sort of the block graph, putting the results into topologically_sorted_blocks.
	boost::topological_sort(m_block_graph, std::back_inserter(topologically_sorted_blocks));
	
	// Start out at an indent level of 0 for every block.
	block_indent_level.assign(topologically_sorted_blocks.size(), 0);
	
	// Figure out the indent level.
	long this_parent = 0;
	BOOST_REVERSE_FOREACH(VertexID vid, topologically_sorted_blocks)
	{
		if(boost::out_degree(vid, m_block_graph)>1)
		{
			// This is some sort of decision point, indent the children.
			/// \todo Make this more efficient.
			long this_child = 0;
			BOOST_REVERSE_FOREACH(VertexID child, topologically_sorted_blocks)
			{
				EdgeID dummy;
				bool is_child;
				tie(dummy, is_child) = boost::edge(vid, child, m_block_graph);
				if(is_child)
				{
					// This is a child, add up the indent level.
					block_indent_level[this_child] = block_indent_level[this_parent] + 1;
				}
				this_child++;
			}
		}
		this_parent++;
	}
	
	// Print the function identifier.
	std::cout << m_function_id << "()" << std::endl;
	
	// Print the function's blocks.
	long indent_index = 0;
	for(T_SORTED_BLOCK_CONTAINER::reverse_iterator rit = topologically_sorted_blocks.rbegin();
		rit != topologically_sorted_blocks.rend();
		rit++)
	{
		if(m_block_graph[*rit].m_block != NULL)
		{
			// Print the block.
			m_block_graph[*rit].m_block->PrintBlock(block_indent_level[indent_index]);
			indent_index++;
		}
		else
		{
			std::cout << "INFO: m_block_graph[" << *rit << "].m_block == NULL" << std::endl;
		}
	}
}

void Function::PrintDotCFG(const boost::filesystem::path& output_dir)
{
	std::string dot_filename;
	
	dot_filename = output_dir.string()+m_function_id+".dot";
	
	std::cerr << "Creating " << dot_filename << std::endl;
	
	std::ofstream outfile(dot_filename.c_str());
	
	boost::write_graphviz(outfile, m_cfg,
						 cfg_vertex_property_writer<T_CFG>(m_cfg),
						 boost::default_writer(), //edge_property_writer<T_CFG>(m_cfg),
						 graph_property_writer());
	
	outfile.close();
	
	std::cerr << "Compiling " << dot_filename << std::endl;
	::system(("dot -O -Tpng "+dot_filename).c_str());
}

bool Function::CreateControlFlowGraph()
{
	// We create the Control Flow Graph in two stages:
	// - First we go through each basic block and add all Statements to m_cfg,
	//   adding an edge linking each to its predecessor as we go.  We exclude in-edges
	//   and out-edges for the first and last Statement in each block, resp.
	// - Second, we link the last Statement of each block to its Successors.
	
	std::map< T_BLOCK_GRAPH::vertex_descriptor, CFGVertexID > first_statement_of_block;
	std::vector< CFGVertexID > last_statement_of_block;
	bool ok;
				
	// Do the first step.
	//BOOST_FOREACH(Block *bp, m_block_list)
	T_BLOCK_GRAPH::vertex_iterator vit, vend;
	for(boost::tie(vit,vend) = boost::vertices(m_block_graph); vit != vend; vit++)
	{
		Block::T_STATEMENT_LIST_ITERATOR sit;
		CFGVertexID last_vid;
		bool is_first = true;
		
		// Iterate over all Statements in this Block.
		for(sit = m_block_graph[*vit].m_block->begin(); sit != m_block_graph[*vit].m_block->end(); sit++)
		{
			// Add this Statement to the Control Flow Graph.
			CFGVertexID vid;
			vid = boost::add_vertex(m_cfg);
			m_cfg[vid].m_statement = *sit;
			
			if(!is_first)
			{
				// Add an edge to its predecessor.
				CFGEdgeID eid;

				boost::tie(eid, ok) = boost::add_edge(last_vid, vid, m_cfg);
			}
			else
			{
				// This is the first statement from this block.  Save it for stage 2.
				first_statement_of_block[*vit] = vid;
				is_first = false;
			}
			
			// It's OK to save the vertex descriptor for next time.  Per the Boost
			// docs, neither add_vertex() nor add_edge() invalidate vertex or edge
			// descriptors.
			last_vid = vid;
		}
		
		// Save the vertex descriptor of the last statement of this block for the next stage.
		last_statement_of_block.push_back(last_vid);
	}
	
	// Do the second step.
	std::vector< CFGVertexID >::iterator last_statement_it;
	std::map< T_BLOCK_GRAPH::vertex_descriptor, CFGVertexID >::iterator first_statement_it;
	last_statement_it = last_statement_of_block.begin();
	for(boost::tie(vit,vend) = boost::vertices(m_block_graph); vit != vend; vit++)
	{
		T_BLOCK_GRAPH::out_edge_iterator eit, eend;
		for(boost::tie(eit, eend) = boost::out_edges(*vit, m_block_graph); eit != eend; eit++)
		{
			// Add an edge from the last statement of Block vit to the first statement
			// of the Block pointed to by eit.
			CFGVertexID target_vertex_descr = boost::target(*eit, m_block_graph);
			std::cout << "Target: " << target_vertex_descr << std::endl;
			first_statement_it = first_statement_of_block.find(target_vertex_descr);
			
			if(first_statement_it == first_statement_of_block.end())
			{
				std::cout << "ERROR: No first block statement found." << std::endl;
			}
			else
			{
				// Add the edge.
				CFGEdgeID new_edge_desc;
				boost::tie(new_edge_desc, ok) = boost::add_edge(*last_statement_it, first_statement_it->second, m_cfg);
			}
		}
		last_statement_it++;
	}
}
