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

#ifndef SPARSEPROPERTYMAP_H_
#define SPARSEPROPERTYMAP_H_

#include <boost/tr1/unordered_map.hpp>

template < typename Key, typename Value, Value DefaultValue >
class SparsePropertyMap
{
	typedef std::tr1::unordered_map<Key, Value> T_UNDERLYING_MAP;

public:
	SparsePropertyMap() {};
	~SparsePropertyMap() {};

	void set(const Key& vdesc, Value val)
	{
		if(val != 0)
		{
			// Simply set the value.
			m_underlying_map[vdesc] = val;
		}
		else
		{
			// If we're setting that value to 0, we no longer need the key.
			// Let's erase it in the interest of conserving space.
			m_underlying_map.erase(vdesc);
		}
	};

	Value get(const Key& vdesc)
	{
		typename T_UNDERLYING_MAP::iterator it;

		it = m_underlying_map.find(vdesc);
		if (it == m_underlying_map.end())
		{
			// The key wasn't in the map, which means we haven't
			// encountered it before now.
			// Pretend it was in the map and add it with its original value.
			m_underlying_map[vdesc] = DefaultValue;

			return DefaultValue;
		}
		else
		{
			// Vertex was in the map.
			return it->second;
		}
	};

private:
	/// The underlying vertex descriptor to integer map.
	T_UNDERLYING_MAP m_underlying_map;
};


#endif /* SPARSEPROPERTYMAP_H_ */
