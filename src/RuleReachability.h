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

#ifndef RULEREACHABILITY_H
#define	RULEREACHABILITY_H

#include "RuleDFSBase.h"

class Function;

class RuleReachability : public RuleDFSBase
{
public:
	RuleReachability(const T_CFG &cfg, const Function *source, const Function *sink);
	RuleReachability(const RuleReachability& orig);
	virtual ~RuleReachability();
	
	virtual bool RunRule();
	
	void PrintCallChain(const T_CFG &cfg, T_CFG_VERTEX_DESC v) const;
	
	virtual bool TerminatorFunction(T_CFG_VERTEX_DESC v);
	
protected:
	
	/// \todo I think we probably don't need this.
	virtual void WalkPredecessorListAction(T_CFG_VERTEX_DESC v) {};
	
private:
	
	/// Flag which we'll set when we find m_sink to stop the search.
	bool m_found_sink;

	/// The function which must not call m_sink.
	const Function *m_source;
	
	/// The function which must not be called from m_sink.
	const Function *m_sink;
	
	std::vector<T_CFG_VERTEX_DESC> *m_p;

};

#endif	/* RULEREACHABILITY_H */

