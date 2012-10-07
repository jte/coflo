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

#include "FileTemplate.h"

#include <boost/foreach.hpp>

#include "../libexttools/toollib.h"

void FileTemplate::simple_replace_t::Apply(std::string &text)
{
	::regex_replace(text, (*m_regex_to_match).str(), *m_replacement);
}

void FileTemplate::append_after_t::Apply(std::string &text)
{
	::regex_append_after(text, (*m_regex_to_match).str(), *m_replacement);
}


FileTemplate::FileTemplate()
{
	// TODO Auto-generated constructor stub

}

FileTemplate::~FileTemplate()
{
	// TODO Auto-generated destructor stub
}

FileTemplate& FileTemplate::regex_replace(const std::string& regex_to_match,
		const std::string& replacement)
{
	// Append a simple replacement functor to the modifier list.
	simple_replace_t *r = new simple_replace_t(regex_to_match, replacement);
	m_filter_list.push_back(r);

	return *this;
}

FileTemplate& FileTemplate::regex_append_after(const std::string& regex_to_match,
		const std::string& replacement)
{
	// Append a simple replacement functor to the modifier list.
	append_after_t *r = new append_after_t(regex_to_match, replacement);
	m_filter_list.push_back(r);

	return *this;
}

std::ostream& FileTemplate::InsertionHelper(std::ostream& os) const
{
	// Copy the string.
	std::string s;

	// Apply the filters in sequence.
	BOOST_FOREACH(modifier_base_t *m, m_filter_list)
	{
		m->Apply(s);
	}

	os << s;

	return os;
}
