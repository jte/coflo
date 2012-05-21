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

#ifndef SPARSEPROPERTYMAP_H_
#define SPARSEPROPERTYMAP_H_

#include <boost/property_map/property_map.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/concept_check.hpp>


template < typename KeyType, typename ValueType, ValueType DefaultValue >
class SparsePropertyMap
{
	/// Private convenience typedef for the underlying map type we'll use.
	typedef std::tr1::unordered_map<KeyType, ValueType> T_UNDERLYING_MAP;

public:
	/// @name Public typenames for the ReadablePropertyMap concept.
	///@{
    typedef KeyType key_type;
    typedef ValueType value_type;
    typedef value_type reference;
    typedef boost::read_write_property_map_tag category;
    ///@}

	SparsePropertyMap() {};
	/**
	 * ReadablePropertyMaps are a refinement of the CopyConstructable concept,
	 * although it appears the ReadablePropertyMap concept checking class doesn't actually check for this.
	 *
	 * @param other  The SparsePropertyMap to copy from.
	 */
	SparsePropertyMap(const SparsePropertyMap& other) : m_underlying_map(other.m_underlying_map) {};
	~SparsePropertyMap() {};

	void put(const KeyType& vdesc, ValueType val)
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

	const reference get(const KeyType& vdesc) const
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
	mutable T_UNDERLYING_MAP m_underlying_map;
};

/// @nameFree functions to match the Boost PropertyMap concepts.
///@{

template < typename KeyType, typename ValueType, ValueType DefaultValue >
inline const typename SparsePropertyMap<KeyType, ValueType, DefaultValue>::reference
get(const SparsePropertyMap<KeyType, ValueType, DefaultValue> &map, const KeyType &key)
{
	return map.get(key);
};

template < typename KeyType, typename ValueType, ValueType DefaultValue >
inline void
put(SparsePropertyMap<KeyType, ValueType, DefaultValue> &map, const KeyType &key, const ValueType &value)
{
	map.put(key, value);
};

///@}


/// Make sure we're correctly modeling the property map concept.
typedef SparsePropertyMap<int, int, 0> IntIntZeroPropMap;
BOOST_CONCEPT_ASSERT(( boost::CopyConstructibleConcept<IntIntZeroPropMap> ));
BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyMapConcept<IntIntZeroPropMap, const int> ));
BOOST_CONCEPT_ASSERT(( boost::WritablePropertyMapConcept<IntIntZeroPropMap, const int> ));



#endif /* SPARSEPROPERTYMAP_H_ */
