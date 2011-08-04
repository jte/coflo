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

#include "debug_utils.hpp"

#include <iostream>

static bool f_dout_enabled = true;
static std::ostream null_stream(0);

debug_ostream dout(std::cout);
debug_ostream derr(std::cerr);
debug_ostream dlog(std::clog);


debug_ostream::debug_ostream(std::ostream &default_ostream) : m_default_ostream(default_ostream)
{
}

debug_ostream::operator std::ostream& ()
{
	if(m_enabled)
	{
		return m_default_ostream;
	}
	else
	{
		return null_stream;
	}
}
/*
std::ostream& dout()
{
	if(f_dout_enabled)
	{
		return std::cout;
	}
	else
	{
		return null_stream;
	}
}

void enable_dout(bool enable)
{
	f_dout_enabled = enable;
}
*/
