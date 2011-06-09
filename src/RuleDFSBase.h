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

#ifndef RULEDFSBASE_H
#define	RULEDFSBASE_H

#include "RuleBase.h"

class RuleDFSBase : RuleBase
{
public:
	RuleDFSBase(const T_CFG &cfg);
	RuleDFSBase(const RuleDFSBase& orig);
	virtual ~RuleDFSBase();
	
	void SetSourceVertex(T_CFG_VERTEX_DESC source);
	
	void SetSinkVertex(T_CFG_VERTEX_DESC sink);
	
	bool RunRule();
	
	virtual bool TerminatorFunction(T_CFG_VERTEX_DESC v);
	
	virtual void WalkPredecessorList(T_CFG_VERTEX_DESC v);
	
	virtual void WalkPredecessorListAction(T_CFG_VERTEX_DESC v) = 0;

protected:
	
	friend class CFGDFSVisitor;
	
	T_CFG_VERTEX_DESC m_source;
	T_CFG_VERTEX_DESC m_sink;
	
	/// Array in which to store the predecessor of each visited vertex.
	std::vector<T_CFG_VERTEX_DESC> m_p;
	
	/// The color map to use for the search.
	std::vector<boost::default_color_type> m_color_vec;
	
private:

	/// The control flow graph we'll run our depth-first search on.
	const T_CFG &m_cfg;	
	
	/// Flag which we'll set when we find m_sink.
	bool m_found_sink;
};

#endif	/* RULEDFSBASE_H */

