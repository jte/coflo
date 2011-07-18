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

#include <string>
#include <vector>

#ifndef VERSIONNUMBER_H
#define	VERSIONNUMBER_H

#include <boost/operators.hpp>

/**
 * Class for encapsulating version numbers.
 * 
 * @param version_string The version number string to initialize this instance with.
 */
class VersionNumber : boost::totally_ordered<VersionNumber>
{
public:
	VersionNumber(const std::string &version_string);
	VersionNumber(const VersionNumber& orig);
	virtual ~VersionNumber();
	
	/// @name Overloaded operators.
	/// Others are provided by Boost.Operators.
	//@{
	bool operator==(const VersionNumber &other) const;
	bool operator<(const VersionNumber &other) const;
	//@}
private:

	/// The version number in its original string form.
	std::string m_version_string;
	
	// The version number parsed into its constituent parts.
	std::vector<int> m_version_digits;
};

#endif	/* VERSIONNUMBER_H */

