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

#ifndef SAFE_ENUM_H
#define	SAFE_ENUM_H

/**
 * @macro DECLARE_ENUM_CLASS Type-safe enumeration class macro.
 *
 * Macro for backwards- and forwards-compatible support of C++0x's "Strongly Typed Enums",
 * i.e. "enum class E { E1, E2, ...};"
 * 
 * This one uses the default underlying type of the enum, and so is not completely equivalent
 * to an enum class {}; in that regard.
 */
#define DECLARE_ENUM_CLASS(enum_class_name, ...) \
class enum_class_name \
{\
public:\
	/** The underlying value type, which is still an enum. */ \
	enum value_type { __VA_ARGS__ };\
\
	enum_class_name() {};\
	enum_class_name(value_type value) : m_value(value) {};\
\
	/** @name Comparison operators */ \
	/**@{*/\
	bool operator == (const enum_class_name & other) const { return m_value == other.m_value; }\
	bool operator != (const enum_class_name & other) const { return m_value != other.m_value; }\
	bool operator == (const value_type & other) const { return m_value == other; }\
	bool operator != (const value_type & other) const { return m_value != other; }\
	/**@}*/\
\
	/** An accessor to return the underlying integral type.  Unfortunately there isn't \
	  * any real way around this if we want to support switch statements. */ \
	value_type as_enum() const { return m_value; };\
\
private:\
	/** The stored value type. */ \
	value_type m_value;\
};

#endif	/* SAFE_ENUM_H */

