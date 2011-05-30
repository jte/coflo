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

#include <iostream>
#include <boost/graph/depth_first_search.hpp>

#include "ControlFlowGraph.h"
#include "Function.h"
#include "RuleReachability.h"
#include "Entry.h"

RuleReachability::RuleReachability(const Function *source, const Function *sink)
{
	m_source = source;
	m_sink = sink;
}

RuleReachability::RuleReachability(const RuleReachability& orig)
{
	m_source = orig.m_source;
	m_sink = orig.m_sink;
}

RuleReachability::~RuleReachability()
{
}

class reachability_visitor : public boost::default_dfs_visitor
{
public:
	typedef boost::on_discover_vertex event_filter;
	reachability_visitor(const T_CFG &cfg, T_CFG_VERTEX_DESC sink, RuleReachability *reachability)
		: m_cfg(cfg), m_sink(sink)
	{
		m_reachability = reachability;
	};
	reachability_visitor(const reachability_visitor &other) : boost::default_dfs_visitor(other), m_cfg(other.m_cfg), m_sink(other.m_sink)
	{
		m_reachability = other.m_reachability;
	};
	virtual ~reachability_visitor() {};
	
	void operator()(T_CFG_VERTEX_DESC v, const T_CFG &cfg) 
	{
		if(v == m_sink)
		{
			std::cout << "Found it." << std::endl;
			m_reachability->PrintCallChain(cfg, v);
		}
	};
	//void discover_vertex(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg) {};
	
private:
	const T_CFG &m_cfg;
	
	T_CFG_VERTEX_DESC m_sink;
	
	RuleReachability *m_reachability;
};

bool RuleReachability::RunRule(const T_CFG &cfg)
{
	T_CFG_VERTEX_DESC starting_vertex_desc;
	reachability_visitor v(cfg, m_sink->GetEntryVertexDescriptor(), this);
	// Array to store predecessor (parent) of each visited vertex.
	std::vector<T_CFG_VERTEX_DESC> p(boost::num_vertices(cfg));
	m_p = &p;
	
	// Point each vertex to itself.
	for(long i = 0; i<boost::num_vertices(cfg); i++)
	{
		p[i] = i;
	}
	
	// The color map to use for the search.
	std::vector<boost::default_color_type> color_vec(boost::num_vertices(cfg));

	starting_vertex_desc = m_source->GetEntryVertexDescriptor();
	
	boost::depth_first_visit(cfg, starting_vertex_desc,
		boost::make_dfs_visitor(
			std::make_pair(
				v, boost::record_predecessors(&p[0], boost::on_tree_edge())
			)
		),
		boost::make_iterator_property_map(color_vec.begin(), boost::get(boost::vertex_index, cfg))
	);

	return true;
}

void RuleReachability::PrintCallChain(const T_CFG &cfg, T_CFG_VERTEX_DESC v) const
{
	T_CFG_VERTEX_DESC parent = -1;
	
	while(parent != v)
	{
		if(NULL != dynamic_cast<Entry*>(cfg[v].m_statement))
		{
			std::cout << "Function Entry: " << cfg[v].m_containing_function->GetIdentifier() << std::endl;
		}
		parent = v;
		v = (*m_p)[v];
	}
}
