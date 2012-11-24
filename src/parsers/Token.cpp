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

#include "Token.h"

#include <string>

#include <dparse.h>

Token::Token(const std::string &text, const Location& location) : m_text(text), m_location(location)
{
}

Token::Token(const D_ParseNode &dparser_parse_node)
{
	// Extract the filename, line, and column from the dparser parse node.
	m_location = Location(dparser_parse_node.start_loc.pathname,
			dparser_parse_node.start_loc.line,
			dparser_parse_node.start_loc.col);
	m_text = std::string(dparser_parse_node.start_loc.s, dparser_parse_node.end - dparser_parse_node.start_loc.s);
}

Token::Token(const Token& other) : m_text(other.m_text), m_location(other.m_location)
{
}

Token::~Token()
{
}

std::ostream& Token::InsertionHelper(std::ostream& os) const
{
	/// @todo Just the text for now, but probably want this to be something different.
	os << m_text;
	return os;
}
