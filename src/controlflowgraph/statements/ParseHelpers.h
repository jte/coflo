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

#ifndef PARSEHELPERS_H
#define PARSEHELPERS_H

#include <string>
#include <map>

#include "../ControlFlowGraph.h"
#include "FlowControlBase.h"


class LabelMap;


#define M_DEFINE_FLOW_CONTROL_VIRTUALS(name) \
	virtual std::string GetStatementTextDOT() const { return #name "_UNLINKED"; }; \
	virtual std::string GetIdentifierCFG() const { return #name "_UNLINKED"; };

#define M_DECLARE_FLOW_CONTROL_VIRTUALS(name) \
		virtual FlowControlBase* ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map);

class FlowControlUnlinked : public FlowControlBase
{
public:
	FlowControlUnlinked() : FlowControlBase(Location()) {};
	FlowControlUnlinked(const Location &loc) : FlowControlBase(loc)	{ };
	virtual ~FlowControlUnlinked() {};

	/**
	 *
	 * @param cfg
	 * @param this_vertex
	 * @param label_map
	 * @return Pointer to a FlowControlBase-derived object which the caller is to use to replace this object with in the
	 * 		vertex's CFGVertexProperties.  NULL if all links could not be resolved.
	 */
	virtual FlowControlBase* ResolveLinks(ControlFlowGraph &cfg, T_CFG_VERTEX_DESC this_vertex, LabelMap &label_map) = 0;

};

class GotoUnlinked : public FlowControlUnlinked
{
public:
	GotoUnlinked() : FlowControlUnlinked(Location())
	{
	}
	GotoUnlinked(const Location &loc, const std::string &link_target_name) : FlowControlUnlinked(loc)
	{
		m_link_target_name = link_target_name;
	}
	virtual ~GotoUnlinked() {};

	std::string GetTarget() { return m_link_target_name; };

	M_DEFINE_FLOW_CONTROL_VIRTUALS(GOTO)

	M_DECLARE_FLOW_CONTROL_VIRTUALS(Goto)

private:

	std::string m_link_target_name;
};

class ReturnUnlinked : public FlowControlUnlinked
{
public:
	ReturnUnlinked() : FlowControlUnlinked(Location())
	{

	}
	ReturnUnlinked(const Location &loc, const std::string &return_var_name) : FlowControlUnlinked(loc)
	{
		m_return_var_name = return_var_name;
	}
	virtual ~ReturnUnlinked() {};

	M_DEFINE_FLOW_CONTROL_VIRTUALS(RETURN)

	M_DECLARE_FLOW_CONTROL_VIRTUALS(Return)

private:

	std::string m_return_var_name;
};

class IfUnlinked : public FlowControlUnlinked
{
public:
	IfUnlinked() : FlowControlUnlinked() {};
	IfUnlinked(const Location &loc, const std::string &condition,
			GotoUnlinked *goto_true, GotoUnlinked *goto_false) : FlowControlUnlinked(loc)
	{
		m_condition = condition;
		m_true = goto_true;
		m_false = goto_false;
	}
	virtual ~IfUnlinked() {};

	M_DEFINE_FLOW_CONTROL_VIRTUALS(IF)

	M_DECLARE_FLOW_CONTROL_VIRTUALS(If)

private:
	std::string m_condition;
	GotoUnlinked *m_true;
	GotoUnlinked *m_false;
};

class CaseUnlinked : public FlowControlUnlinked
{
public:
	CaseUnlinked() : FlowControlUnlinked(Location()) {};
	CaseUnlinked(const Location &loc, /* condition,*/ const std::string &link_target_name) : FlowControlUnlinked(loc)
	{
		m_link_target_name = link_target_name;
	}
	virtual ~CaseUnlinked() {};

	std::string GetTarget() { return m_link_target_name; };

	M_DEFINE_FLOW_CONTROL_VIRTUALS(CASE);

	M_DECLARE_FLOW_CONTROL_VIRTUALS(Case)

private:
	std::string m_link_target_name;
};

class SwitchUnlinked : public FlowControlUnlinked
{
public:
	SwitchUnlinked() : FlowControlUnlinked(Location()) {};
	SwitchUnlinked(const Location &loc) : FlowControlUnlinked(loc) {};
	virtual ~SwitchUnlinked() {};

	void InsertCase(CaseUnlinked *the_case)
	{
		m_case_list.push_back(the_case);
	};

	M_DEFINE_FLOW_CONTROL_VIRTUALS(SWITCH)

	M_DECLARE_FLOW_CONTROL_VIRTUALS(Switch)

private:

	std::vector<CaseUnlinked *> m_case_list;
};


#endif /* PARSEHELPERS_H_ */
