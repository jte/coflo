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

#include <cstdlib>
#include "CFGEdgeTypeFallthrough.h"

/// Definition of the single CFGEdgeTypeFallthrough instance.
CFGEdgeTypeFallthrough *CFGEdgeTypeFallthrough::m_the_singleton = NULL;

CFGEdgeTypeFallthrough* CFGEdgeTypeFallthrough::Factory()
{
	if(m_the_singleton == NULL)
	{
		m_the_singleton = new CFGEdgeTypeFallthrough();
	}
	
	return m_the_singleton;
}

CFGEdgeTypeFallthrough::CFGEdgeTypeFallthrough() : CFGEdgeTypeBase()
{
}

CFGEdgeTypeFallthrough::CFGEdgeTypeFallthrough(const CFGEdgeTypeFallthrough& orig)
{
}

CFGEdgeTypeFallthrough::~CFGEdgeTypeFallthrough()
{
}
