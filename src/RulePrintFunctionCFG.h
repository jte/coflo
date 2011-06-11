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

#ifndef RULEPRINTFUNCTIONCFG_H
#define	RULEPRINTFUNCTIONCFG_H

#include "RuleDFSBase.h"

class Function;

class RulePrintFunctionCFG : public RuleDFSBase
{

public:
	RulePrintFunctionCFG(const T_CFG &cfg, Function *f);
	RulePrintFunctionCFG(const RulePrintFunctionCFG& orig);
	virtual ~RulePrintFunctionCFG();
	
protected:
	virtual void WalkPredecessorListAction(T_CFG_VERTEX_DESC v);
	
private:

};

#endif	/* RULEPRINTFUNCTIONCFG_H */

