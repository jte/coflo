/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include <boost/foreach.hpp>

#include "ParseHelpers.h"

#include "../ControlFlowGraph.h"
#include "statements.h"
#include "../edges/edge_types.h"

class LabelMap : public std::map< std::string, T_CFG_VERTEX_DESC>
{

};

FlowControlBase* GotoUnlinked::ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map)
{
	// Look up our target.
	LabelMap::iterator it;
	it = label_map.find(m_link_target_name);

	if(it == label_map.end())
	{
		// Couldn't find it.
		std::cerr << "ERROR: Can't find goto target label \"" << m_link_target_name << "\"" << std::endl;
		return NULL;
	}
	else
	{
		// Found it.  Add an edge.
		cfg.AddEdge(this_vertex, (*it).second, new CFGEdgeTypeFallthrough());

		return new Goto(this->GetLocation());
	}
}

FlowControlBase* ReturnUnlinked::ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map)
{
	// Look up our target, which is always "EXIT" for a return statement.
	LabelMap::iterator it;
	it = label_map.find("EXIT");

	if(it == label_map.end())
	{
		// Couldn't find it.
		std::cerr << "ERROR: Can't find return target label \"" << "EXIT" << "\"" << std::endl;
		return NULL;
	}
	else
	{
		// Found it.  Add an edge.
		cfg.AddEdge(this_vertex, (*it).second, new CFGEdgeTypeFallthrough());

		/// @todo Create a real Return class, return that instead.
		return new Goto(this->GetLocation());
	}
}

FlowControlBase* IfUnlinked::ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map)
{
	// Look up our targets.
	LabelMap::iterator it_true, it_false;
	it_true = label_map.find(m_true->GetTarget());
	it_false = label_map.find(m_false->GetTarget());

	if(it_true == label_map.end())
	{
		// Couldn't find it.
		std::cerr << "ERROR: Can't find if-true target label \"" << m_true->GetTarget() << "\"" << std::endl;
		return NULL;
	}
	else if(it_false == label_map.end())
	{
		// Couldn't find it.
		std::cerr << "ERROR: Can't find if-false target label \"" << m_false->GetTarget() << "\"" << std::endl;
		return NULL;
	}
	else
	{
		// Found both targets.  Add edges.
		cfg.AddEdge(this_vertex, (*it_true).second, new CFGEdgeTypeIfTrue());
		cfg.AddEdge(this_vertex, (*it_false).second, new CFGEdgeTypeIfFalse());

		return new If(this->GetLocation(), m_condition);
	}
}

FlowControlBase* CaseUnlinked::ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map)
{
	/// @todo We should eliminate this class, since it never appears on its own.
	std::cerr << "ERROR: CaseUnlinked::ResolveLinks called, should never happen." << std::endl;
	return NULL;
}

FlowControlBase* SwitchUnlinked::ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map)
{
	bool resolved_any_links = false;

	/** @todo Delete
	std::cout << "INFO: Num switch cases = " << m_case_list.size() << std::endl;
	BOOST_FOREACH(CaseUnlinked *cu, m_case_list)
	{
		std::cout << "INFO: Switch case = " << cu << std::endl;
	}
	*/
	BOOST_FOREACH(CaseUnlinked *cu, m_case_list)
	{
		// Look up our target.
		LabelMap::iterator it;
		it = label_map.find(cu->GetTarget());

		//std::cout << "INFO: Switch case target = " << cu->GetTarget() << std::endl;

		if(it == label_map.end())
		{
			// Couldn't find it.
			std::cerr << "ERROR: Can't find switch case target label \"" << cu->GetTarget() << "\"" << std::endl;
		}
		else
		{
			// Found it.  Add an edge.
			cfg.AddEdge(this_vertex, (*it).second, new CFGEdgeTypeFallthrough());

			resolved_any_links = true;
		}
	}

	if(resolved_any_links)
	{
		return new Switch(this->GetLocation());
	}
	else
	{
		return NULL;
	}
}

