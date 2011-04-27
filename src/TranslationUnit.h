/**
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

#ifndef TRANSLATIONUNIT_H
#define	TRANSLATIONUNIT_H

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

class Function;

class TranslationUnit
{
public:
    TranslationUnit();
    TranslationUnit(const TranslationUnit& orig);
    virtual ~TranslationUnit();

	bool ParseFile(const boost::filesystem::path &filename, bool debug_parse = false);

	bool LinkBasicBlocks();

	void Print();

private:

	/// The filename.
	boost::filesystem::path m_filename;

	/// List of function definitions in this file.
	std::vector< Function * > m_function_defs;

};

#endif	/* TRANSLATIONUNIT_H */

