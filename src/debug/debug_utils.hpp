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
	~debug_ostream() {};

	/// Typedef for ostream manipulators, which take ostream references as the input parameter.
	typedef std::ostream& (*OSTREAM_MANIPULATOR)(std::ostream&);

	/**
	 * Primary inserter overload.  Handles everything except manipulators.
	 *
	 * @param value Reference to the value to insert into the output stream.
	 * @return Reference to this debug_ostream.
	 */
	template<typename T>
	debug_ostream& operator<<(const T &value)
	{
		if(m_enabled)
		{
			m_default_ostream << value;
		}
		return *this;
	};

	/**
	 * Inserter overload for handling manipulators.
	 *
	 * @param manipulator
	 * @return
	 */
	debug_ostream& operator<<(OSTREAM_MANIPULATOR manipulator)
	{
		if(m_enabled)
		{
			// Apply the manipulator to the stream.
			manipulator(m_default_ostream);
		}
		return *this;
	};

	void enable(bool enable = true) { m_enabled = enable; };
	void disable() { enable(false); };

private:
	std::ostream &m_default_ostream;
	bool m_enabled;
};

/// @name Our three standard debug output streams.
//@{
extern debug_ostream dout;
extern debug_ostream derr;
extern debug_ostream dlog;
//@}

#endif /* DEBUG_UTILS_HPP */
