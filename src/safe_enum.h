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
 * Type-safe enumeration class template.
 */
template <typename def>
class safe_enum : public def
{
	typedef typename def::value_type value_type;
	
public:
	safe_enum() {};
	safe_enum(value_type value) : m_value(value) {};
	// Note: No destructor since it's trivial.  We'll let the compiler determine the
	// best way to destroy us.
	
	/// @name Operators
	//@{
	bool operator == (const safe_enum & other) const { return m_value == other.m_value; }
	bool operator != (const safe_enum & other) const { return m_value != other.m_value; }
	//@}
	
	value_type as_enum() const { return m_value; };

private:
	/// The actual representation of the enum.
	value_type m_value;
};


/**
 * Macro for backwards- and forwards-compatible support of C++0x's "Strongly Typed Enums",
 * i.e. "enum class E { E1, E2, ...};"
 * 
 * This one uses the default underlying type of the enum, and so is not completely equivalent
 * to an enum class {}; in that regard.
 */
#define DECLARE_ENUM_CLASS(enum_class_name, ...) \
	struct enum_class_name##_declaration { enum value_type { __VA_ARGS__ }; }; \
	typedef safe_enum<enum_class_name##_declaration> enum_class_name;

#endif	/* SAFE_ENUM_H */

