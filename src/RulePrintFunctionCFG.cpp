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

#include "RulePrintFunctionCFG.h"
#include "Function.h"
#include "Entry.h"

RulePrintFunctionCFG::RulePrintFunctionCFG(const T_CFG &cfg, Function *f) : RuleDFSBase(cfg)
{
	SetSourceVertex(f->GetEntryVertexDescriptor());
	SetSinkVertex(f->GetExitVertexDescriptor());
}

RulePrintFunctionCFG::RulePrintFunctionCFG(const RulePrintFunctionCFG& orig) : RuleDFSBase(orig)
{
}

RulePrintFunctionCFG::~RulePrintFunctionCFG()
{
}

void RulePrintFunctionCFG::WalkPredecessorListAction(T_CFG_VERTEX_DESC v)
{
	// For this DFS, we only want to print out Entry predecessors.
	//if(NULL != dynamic_cast<Entry*>(m_cfg[v].m_statement))
	{
		std::cout << "Function Entry: " << m_cfg[v].m_containing_function->GetIdentifier() << std::endl;
		std::cout << " statement = " << m_cfg[v].m_statement->GetStatementTextDOT() << std::endl;
	}
}
