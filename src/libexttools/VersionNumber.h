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
	VersionNumber();
	VersionNumber(const std::string &version_string);
	VersionNumber(const VersionNumber& orig);
	virtual ~VersionNumber();
	
	void Set(const std::string &version_string);
	
	/// @name Overloaded operators.
	/// Others are provided by Boost.Operators.
	//@{
	VersionNumber& operator=(const VersionNumber &other);
	bool operator==(const VersionNumber &other) const;
	bool operator<(const VersionNumber &other) const;
	//@}
	
	/// @name Conversion operators
	//@{
	operator std::string() const;
	//@}
	
	bool empty() const { return m_version_string.empty(); };
	
	friend std::ostream& operator<<(std::ostream& os, const VersionNumber& ver);
	
private:

	/**
	 * Do a deep (vs. the default shallow) copy of the object.
	 * Does the work for copy constructors and assignment operators.
	 * 
     * @param other The other VersionNumber to copy into this one.
     */
	void DeepCopy(const VersionNumber &other);
	
	/// The version number in its original string form.
	std::string m_version_string;
	
	/// The version number parsed into its constituent parts.
	/// This is primarily for comparison purposes.
	std::vector<int> m_version_digits;
};

#endif	/* VERSIONNUMBER_H */

