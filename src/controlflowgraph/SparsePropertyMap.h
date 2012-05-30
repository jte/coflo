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

#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/functional.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/concept_check.hpp>


/**
 * Functor type for use as a default for SparsePropertyMap's DefaultValueFunctor in the common case
 * where the default value is a constant equal to the EntryNoLongerNeededValue.
 */
template <typename KeyType, typename ValueType, const ValueType ConstDefaultValue>
struct default_is_const : public std::unary_function<KeyType, const ValueType>
{
	const ValueType operator()(const KeyType /*unused*/) const { return ConstDefaultValue; };
};

/**
 * Sparse property map class.
 *
 * This is a lazily-evaluated data structure, in that a @a key doesn't have a real entry in the underlying map until the first
 * call to either get() or put().  In the case of get(), a never-before-seen @a key will be initialized to
 * the value returned by @a DefaultValueFunctor.
 *
 * The @a EntryNoLongerNeededValue template parameter tells the map when to remove a @a key from the underlying map.  When
 * put() is called with this value, the key is removed from m_underlying_map.  If no DefaultValueFunctor is specified, the
 * default DefaultValueFunctor also uses this value as the default constant value which all keys are initialized to.
 *
 * @tparam KeyType
 * @tparam ValueType
 * @tparam EntryNoLongerNeededValue
 * @tparam DefaultValueFunctor
 */
template < typename KeyType, typename ValueType, ValueType EntryNoLongerNeededValue,
	typename DefaultValueFunctor = default_is_const<KeyType,ValueType,EntryNoLongerNeededValue> >
class SparsePropertyMap
{
	/// Require the DefaultValueFunctor to have a signature like the following:
	///   ValueType DefaultValueFunctor(KeyType);
	BOOST_CONCEPT_ASSERT(( boost::UnaryFunction<DefaultValueFunctor, ValueType, KeyType> ));

	/// Private convenience typedef for the underlying map type we'll use.
	/// @todo This should come in as a template param, probably with this as a default.
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
	SparsePropertyMap(const SparsePropertyMap& other) : m_underlying_map(other.m_underlying_map),
			m_default_value_functor(other.m_default_value_functor) {};
	~SparsePropertyMap() {};

	void put(const KeyType& vdesc, ValueType val)
	{
		if(val != EntryNoLongerNeededValue)
		{
			// Simply set the value.
			m_underlying_map[vdesc] = val;
		}
		else
		{
			// If we're setting that value to EntryNoLongerNeededValue, we no longer need the key.
			// Let's erase it in the interest of conserving space.
			m_underlying_map.erase(vdesc);
		}
	};

	const reference get(const KeyType& key) const
	{
		typename T_UNDERLYING_MAP::iterator it;

		it = m_underlying_map.find(key);
		if (it == m_underlying_map.end())
		{
			// The key wasn't in the map, which means we haven't
			// encountered it before now.
			// Pretend it was in the map and add it with its original value.
			const ValueType default_value = m_default_value_functor(key);
			m_underlying_map[key] = default_value;

			return default_value;
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

	/// A copy of the DefaultValueFunctor.
	DefaultValueFunctor m_default_value_functor;
};

/// @name Free functions to match the Boost PropertyMap concepts.
///@{

/**
 * The non-member get() function for the SparsePropertyMap class.
 *
 * @param map
 * @param key
 * @return
 */
template < typename KeyType, typename ValueType, ValueType EntryNoLongerNeededValue, typename DefaultValueFunctor >
inline const typename SparsePropertyMap<KeyType, ValueType, EntryNoLongerNeededValue, DefaultValueFunctor>::reference
get(const SparsePropertyMap<KeyType, ValueType, EntryNoLongerNeededValue, DefaultValueFunctor> &map, const KeyType &key)
{
	return map.get(key);
};

/**
 * The non-member put() function for the SparsePropertyMap class.
 *
 * @param map
 * @param key
 * @param value
 */
template < typename KeyType, typename ValueType, ValueType EntryNoLongerNeededValue, typename DefaultValueFunctor >
inline void
put(SparsePropertyMap<KeyType, ValueType, EntryNoLongerNeededValue, DefaultValueFunctor> &map, const KeyType &key, const ValueType &value)
{
	map.put(key, value);
};

///@}

/// @name SparsePropertyMap concept checks.
///@{

/// Make sure we're correctly modeling the property map concept.
struct IntIntZeroPropMapDefaultValueFunctor
{
	int operator()(const int ) const { return 0; };
};
typedef SparsePropertyMap<int, int, 0, IntIntZeroPropMapDefaultValueFunctor > IntIntZeroPropMap;
BOOST_CONCEPT_ASSERT(( boost::CopyConstructibleConcept<IntIntZeroPropMap> ));
BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyMapConcept<IntIntZeroPropMap, const int> ));
BOOST_CONCEPT_ASSERT(( boost::WritablePropertyMapConcept<IntIntZeroPropMap, const int> ));

///@}

#endif /* SPARSEPROPERTYMAP_H_ */
