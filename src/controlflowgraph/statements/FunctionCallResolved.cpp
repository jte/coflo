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

#include "../../Location.h"
#include "../../Function.h"
#include "FunctionCallResolved.h"
#include "FunctionCallUnresolved.h"



FunctionCallResolved::FunctionCallResolved(Function *f, FunctionCallUnresolved *fcu) : FunctionCall(fcu->GetLocation(), fcu->m_params)
{
	m_target_function = f;
}

FunctionCallResolved::FunctionCallResolved(const FunctionCallResolved& orig) : FunctionCall(orig)
{
	m_target_function = orig.m_target_function;
}

FunctionCallResolved::~FunctionCallResolved()
{
	// We don't own m_function, so don't delete it.
}

std::string FunctionCallResolved::GetStatementTextDOT() const
{
	return m_target_function->GetIdentifier()+"()";
}

std::string FunctionCallResolved::GetIdentifier() const
{
	return m_target_function->GetIdentifier(); 
}
