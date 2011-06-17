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

#ifndef CFGEDGETYPERETURN_H
#define	CFGEDGETYPERETURN_H

#include "CFGEdgeTypeBase.h"

class FunctionCall;

class CFGEdgeTypeReturn : public CFGEdgeTypeBase
{
public:
	CFGEdgeTypeReturn(FunctionCall *function_call);
	CFGEdgeTypeReturn(const CFGEdgeTypeReturn& orig);
	virtual ~CFGEdgeTypeReturn();

//private:
	
	/// The FunctionCall instance which resulted in this edge.
	/// We need this to determine the point to return to.
	FunctionCall *m_function_call;

};

#endif	/* CFGEDGETYPERETURN_H */

