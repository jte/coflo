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

#include <functional>
#include <boost/graph/depth_first_search.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "ControlFlowGraph.h"

#include "RuleDFSBase.h"

class OldCFGDFSVisitor : public boost::default_dfs_visitor
{
public:
	typedef boost::on_discover_vertex event_filter;
	OldCFGDFSVisitor(const T_CFG &cfg, RuleDFSBase *dfs_base)
		: m_cfg(cfg)
	{
		m_dfs_base = dfs_base;
	};
	OldCFGDFSVisitor(const OldCFGDFSVisitor &other) : boost::default_dfs_visitor(other), m_cfg(other.m_cfg)
	{
		m_dfs_base = other.m_dfs_base;
	};
	virtual ~OldCFGDFSVisitor() {};
	
	//void discover_vertex(T_CFG_VERTEX_DESC v, const T_CFG &cfg)	
	void operator()(T_CFG_VERTEX_DESC v, const T_CFG &cfg) 
	{
		if(v == m_dfs_base->m_sink)
		{
			std::cout << "Found it." << std::endl;
			m_dfs_base->WalkPredecessorList(v);
		}
	};
	
private:
	const T_CFG &m_cfg;
	
	RuleDFSBase *m_dfs_base;
};


RuleDFSBase::RuleDFSBase(const T_CFG &cfg) : m_cfg(cfg)
{
	m_found_sink = false;
}

RuleDFSBase::RuleDFSBase(const RuleDFSBase& orig) : m_cfg(orig.m_cfg)
{
}

RuleDFSBase::~RuleDFSBase() 
{
}

class TerminatorRedirector
{
public:
	TerminatorRedirector(RuleDFSBase *dfs_base)
	{
		m_dfs_base = dfs_base; 
	};
	TerminatorRedirector(const TerminatorRedirector &orig)
	{
		m_dfs_base = orig.m_dfs_base;
	};
	~TerminatorRedirector() {};

	bool operator()(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
	{
		return m_dfs_base->TerminatorFunction(v); 
	};

private:
	RuleDFSBase *m_dfs_base;
};

bool RuleDFSBase::TerminatorFunction(T_CFG_VERTEX_DESC v)
{
	if(m_found_sink)
	{
		// We found the vertex we were looking for, so always terminate any new
		// searches down any paths.
		return true;
	}
	
	return false;
}

bool RuleDFSBase::RunRule()
{
	// Create the visitor object.
	OldCFGDFSVisitor v(m_cfg, this);
	
	// Define a function object to terminate the search down a particular branch.
	// We need to do this functor stuff because for some reason
	// BGL doesn't expose this functionality to visitors.
	TerminatorRedirector term_func(this);
	
	// Allocate the predecessor map.
	m_p.resize(boost::num_vertices(m_cfg));
	
	// Point each vertex to itself.
	for(size_t i = 0; i<boost::num_vertices(m_cfg); i++)
	{
		m_p[i] = i;
	}
	
	m_color_vec.resize(boost::num_vertices(m_cfg));
	
	boost::depth_first_visit(m_cfg, m_source,
		boost::make_dfs_visitor(
			std::make_pair(
				v, boost::record_predecessors(&m_p[0], boost::on_tree_edge())
			)
		),
		boost::make_iterator_property_map(m_color_vec.begin(), boost::get(boost::vertex_index, m_cfg)),
		term_func
	);

	return true;
}

void RuleDFSBase::WalkPredecessorList(T_CFG_VERTEX_DESC v)
{
	// Walk the predecessor list up to the start vertex.
	T_CFG_VERTEX_DESC parent = -1;
	
	// Keep walking as long as the vertex's predecessor isn't itself.
	// When it is, we know we reached the top of the list.
	while(parent != v)
	{
		// Do whatever action the derived class wants to do at this vertex.
		WalkPredecessorListAction(v);
		
		parent = v;
		v = m_p[v];
	}
}
