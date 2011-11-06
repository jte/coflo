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

#ifndef PARSEHELPERS_H
#define PARSEHELPERS_H

#include "FlowControlBase.h"

class GotoUnlinked : public FlowControlBase
{
public:
	GotoUnlinked() : FlowControlBase(Location())
	{

	}
	GotoUnlinked(const Location &loc, const std::string &link_target_name) : FlowControlBase(loc)
	{
		m_link_target_name = link_target_name;
	}
	virtual ~GotoUnlinked() {};

	virtual std::string GetStatementTextDOT() const { return "GOTO_UNLINKED"; };

	virtual std::string GetIdentifierCFG() const { return "GOTO_UNLINKED"; };

private:

	std::string m_link_target_name;
};


#define M_DEFINE_FLOW_CONTROL_VIRTUALS(name) \
	virtual std::string GetStatementTextDOT() const { return #name "_UNLINKED"; }; \
	virtual std::string GetIdentifierCFG() const { return #name "_UNLINKED"; };

class FlowControlUnlinked : public FlowControlBase
{
public:
	FlowControlUnlinked() : FlowControlBase(Location()) {};
	FlowControlUnlinked(const Location &loc, GotoUnlinked *goto_true, GotoUnlinked *goto_false) : FlowControlBase(loc)
	{
		m_true = goto_true;
		m_false = goto_false;
	}
	virtual ~FlowControlUnlinked() {};

	M_DEFINE_FLOW_CONTROL_VIRTUALS(IF)

private:

	GotoUnlinked *m_true;
	GotoUnlinked *m_false;
};

class CaseUnlinked : public FlowControlBase
{
public:
	CaseUnlinked() : FlowControlBase(Location()) {};
	CaseUnlinked(const Location &loc, /* condition,*/ const std::string &link_target_name) : FlowControlBase(loc)
	{
		m_link_target_name = link_target_name;
	}
	virtual ~CaseUnlinked() {};

	M_DEFINE_FLOW_CONTROL_VIRTUALS(CASE);

private:
	std::string m_link_target_name;
};

class SwitchUnlinked : public FlowControlBase
{
public:
	SwitchUnlinked() : FlowControlBase(Location()) {};
	SwitchUnlinked(const Location &loc) : FlowControlBase(loc) {};
	virtual ~SwitchUnlinked() {};

	void InsertCase(CaseUnlinked *the_case)
	{
		m_case_list.push_back(the_case);
	};

	M_DEFINE_FLOW_CONTROL_VIRTUALS(SWITCH)

private:

	std::vector<CaseUnlinked *> m_case_list;
};


#endif /* PARSEHELPERS_H_ */
