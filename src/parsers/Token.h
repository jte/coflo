/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include "Location.h"

struct D_ParseNode;

/**
 * Class for representing parser tokens.
 * A Token encapsulates two pieces of information:
 * - A text string from the input text.
 * - The Location of that string within the text.
 */
class Token
{
public:
	/**
	 * Create a Token object from a std::string object and an optional Location object.
	 * If no Location object is provided, the Token is assigned a default "UNKNOWN" location.
	 *
	 * @param text
	 * @param location
	 */
	Token(const std::string &text, const Location& location = Location());
	Token(const D_ParseNode &dparser_parse_node);
	Token(const Token &other);
	~Token();

	std::string getText() const { return m_text; };

	Location getLocation() const { return m_location; };

	Token& operator=(Token other)
	{
		// We'll use the copy-and-swap idiom to give this assignment operator a strong-exception-safety guarantee.
		// That means that this operation might throw, but if it does, we can guarantee there's been no change
		// to the state of the assignee (i.e. this).

		// Note that by taking "other" as a non-const value instead of the perhaps more customary "const T &", we've
		// implicitly done the first step, the creation of a temporary copy - the "other" parameter itself - via the
		// class' copy constructor.  If the creation of "other" throws, we haven't changed anything here.

		// Swap the contents of this with the contents of other.
		// The swap function is defined to be non-throwing.
		swap(other);

		// This also doesn't throw.
		return *this;
	};

	/**
	 * Non-throwing-swap.
	 *
	 * @param other
	 */
	void swap(Token &other)
	{
		// All the C++ std library classes swap()'s are guaranteed to not throw.
		m_text.swap(other.m_text);
		m_location.swap(other.m_location);
	}

	std::ostream& InsertionHelper(std::ostream &os) const;


private:

	/// The text of this Token.
	std::string m_text;

	/// The Location in the source text of this Token.
	Location m_location;
};

/**
 * Free function in the same namespace as Token to fully implement the non-throwing-swap idiom.
 *
 * @param t1
 * @param t2
 */
inline void swap (Token & t1, Token & t2)
{
  t1.swap(t2);
}

/**
 * Explicit specialization of std::swap() to fully implement the non-throwing-swap idiom for Token.
 *
 * @param t1
 * @param t2
 */
namespace std
{
  template <>
  inline void swap (Token & t1, Token & t2)
  {
    t1.swap(t2);
  }
}

/**
 * Non-member stream insertion operator.
 *
 * @param os
 * @param n
 * @return
 */
inline std::ostream &operator<<(std::ostream &os, const Token& n)
{
	return n.InsertionHelper(os);
};

#endif /* TOKEN_H_ */
