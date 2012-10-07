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

#ifndef FILETEMPLATE_H_
#define FILETEMPLATE_H_

#include <string>
#include <vector>
#include <iosfwd>

#include <boost/regex.hpp>

/*
 *
 */
class FileTemplate
{
public:
	/// @name Member types.
	///@{
	typedef boost::regex regex_t;
	class modifier_base_t
	{
	public:
		modifier_base_t(const std::string& regex_to_match, const std::string& replacement)
		{
			/// @todo Make this exception-safe.
			m_regex_to_match = new boost::regex(regex_to_match);
			m_replacement = new std::string(replacement);
		};
		~modifier_base_t() { delete m_replacement; delete m_regex_to_match; };

		virtual void Apply(std::string &text) = 0;

		regex_t *m_regex_to_match;
		std::string *m_replacement;
	};
	class simple_replace_t : public modifier_base_t
	{
		public: simple_replace_t(const std::string& regex_to_match, const std::string& replacement) : modifier_base_t(regex_to_match, replacement) {};
		virtual void Apply(std::string &text);
	};
	class append_after_t : public modifier_base_t
	{
		public: append_after_t(const std::string& regex_to_match, const std::string& replacement) : modifier_base_t(regex_to_match, replacement) {};
		virtual void Apply(std::string &text);
	};
	///@}

public:
	FileTemplate();

	/**
	 * Construct a file template from a string.
	 *
	 * @param s The string to initialize the FileTemplate with.
	 */
	FileTemplate(const std::string& s) : m_current_file_contents(s) { };
	~FileTemplate();

	FileTemplate& regex_replace(const std::string &regex_to_match, const std::string &replacement);
	FileTemplate& regex_append_after(const std::string &regex_to_match, const std::string &replacement);

	std::ostream& InsertionHelper(std::ostream& os) const;

private:
	/// A string containing the contents of the file.
	std::string m_current_file_contents;

	/// List of attached filters.
	std::vector<modifier_base_t*> m_filter_list;
};

std::ostream& operator<<(std::ostream& os, const FileTemplate &ft)
{
	return ft.InsertionHelper(os);
}


#endif /* FILETEMPLATE_H_ */
