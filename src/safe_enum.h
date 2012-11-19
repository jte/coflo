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

#ifndef SAFE_ENUM_H
#define	SAFE_ENUM_H

#include <iosfwd>
#include <vector>
#include <string>

class SafeEnumBaseClass
{
public:
	SafeEnumBaseClass(const std::string &enum_names);
	virtual ~SafeEnumBaseClass();

	std::string asString(int value) const;

	virtual std::string asString() const = 0;

	std::string GetEnumeratorsAsString() const;

private:

	/**
	 * Convert a string of enumerator declarations to a vector of strings with the value of the enumerators' identifiers.
	 *
	 * @todo The SafeEnumBaseClass currently doesn't handle enumerators with assigned values.
	 *
	 * @param enum_names
	 */
	void EnumeratorStringToVectorOfStrings(const std::string &enum_names);

	/**
	 * A vector of strings which will hold the enumerator names.
	 */
	std::vector<std::string> m_enumerator_names;

};

/**
 * Insertion operator for classes derived from SafeEnumBaseClass.
 * @param os
 * @param n
 * @return
 */
inline std::ostream &operator<<(std::ostream &os, const SafeEnumBaseClass& n)
{
	os << n.asString();
	return os;
};

/**
 * DECLARE_ENUM_CLASS Type-safe enumeration class macro.
 *
 * Macro for backwards- and forwards-compatible support of C++0x's "Strongly Typed Enums",
 * i.e. "enum class E { E1, E2, ...};"
 * 
 * This one uses the default underlying type of the enum, and so is not completely equivalent
 * to an enum class {}; in that regard.
 */
#define DECLARE_ENUM_CLASS(enum_class_name, ...) \
class enum_class_name : public SafeEnumBaseClass \
{\
public:\
	/** The underlying value type, which is still an enum. */ \
	enum value_type { enum_class_name##UNINITIALIZED, __VA_ARGS__ };\
\
	enum_class_name() : SafeEnumBaseClass("UNINITIALIZED, " #__VA_ARGS__), m_value(enum_class_name##UNINITIALIZED) {};\
	enum_class_name(value_type value) : SafeEnumBaseClass("UNINITIALIZED, " #__VA_ARGS__), m_value(value) {};\
\
	/** @name Comparison operators */ \
	/**@{*/\
	bool operator == (const enum_class_name & other) const { return m_value == other.m_value; }\
	bool operator != (const enum_class_name & other) const { return m_value != other.m_value; }\
	bool operator == (const value_type & other) const { return m_value == other; }\
	bool operator != (const value_type & other) const { return m_value != other; }\
	/**@}*/\
\
	/** An accessor to return the underlying integral type.  Unfortunately there isn't */ \
	/** any real way around this if we want to support switch statements. */ \
	value_type as_enum() const { return m_value; };\
	/** Returns the current value of this safe enum as a string. */ \
	std::string asString() const { return SafeEnumBaseClass::asString(static_cast<int>(m_value)); };\
\
private:\
	/** The stored value type. */ \
	value_type m_value;\
};

#endif	/* SAFE_ENUM_H */

