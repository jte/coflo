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

#ifndef FLOWCONTROLBASE_H
#define FLOWCONTROLBASE_H

#include "StatementBase.h"

/**
 * Base class for any statements which modify the control flow.
 */
class FlowControlBase: public StatementBase
{
public:
	explicit FlowControlBase(const Location &location);
	FlowControlBase(const FlowControlBase& orig);
	virtual ~FlowControlBase();
};

#endif /* FLOWCONTROLBASE_H */
