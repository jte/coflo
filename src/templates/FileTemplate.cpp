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

#include <fstream>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>


void FileTemplate::simple_replace_t::Apply(std::string &text)
{
	text = boost::regex_replace(text, *m_regex_to_match, *m_replacement);
}

void FileTemplate::insert_before::Apply(std::string &text)
{
	text = boost::regex_replace(text, *m_regex_to_match, *m_replacement);
}


FileTemplate::FileTemplate()
{
	// TODO Auto-generated constructor stub

}

FileTemplate::~FileTemplate()
{
	/// @todo Memeory leak, delete all members of m_filter_list.
}

FileTemplate& FileTemplate::regex_replace(const std::string& regex_to_match,
		const std::string& replacement)
{
	// Append a simple replacement functor to the modifier list.
	simple_replace_t *r = new simple_replace_t(regex_to_match, replacement);
	m_filter_list.push_back(r);

	return *this;
}

FileTemplate& FileTemplate::regex_insert_before(const std::string& regex_to_match,
		const std::string& replacement)
{
	// Append a simple replacement functor to the modifier list.
	insert_before *r = new insert_before(regex_to_match, replacement);
	m_filter_list.push_back(r);

	return *this;
}

std::string FileTemplate::str()
{
	// Apply the modifiers.
	Apply();

	// Return the resulting string.
	return m_current_file_contents;
}

void FileTemplate::SaveAs(const std::string& filename)
{
	std::ofstream output_file(filename.c_str());
	output_file << str();
}

std::ostream& FileTemplate::InsertionHelper(std::ostream& os)
{
	// Apply the modifiers.
	Apply();

	// Stream out the resulting string.
	os << m_current_file_contents;

	return os;
}

void FileTemplate::Apply()
{
	// Apply all attached filters in sequence.
	BOOST_FOREACH(modifier_base_t *m, m_filter_list)
	{
		m->Apply(m_current_file_contents);
	}

	// We're done with the filters, so remove them.
	BOOST_FOREACH(modifier_base_t *m, m_filter_list)
	{
		delete m;
	}

	m_filter_list.clear();
}
