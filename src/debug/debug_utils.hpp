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

#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <iosfwd>

class debug_ostream
{
public:
	debug_ostream(std::ostream &default_ostream);
	~debug_ostream();

	operator std::ostream& ();

	void enable(bool enable) { m_enabled = enable; };

private:
	std::ostream &m_default_ostream;
	bool m_enabled;
};

extern debug_ostream dout;
extern debug_ostream derr;
extern debug_ostream dlog;
/*
std::ostream& dout();

void enable_dout(bool enable);
*/

#endif /* DEBUG_UTILS_HPP */
