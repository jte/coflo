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
 * @todo Handle a wider range of version number formats, e.g. "1.2.3b".
 */
class VersionNumber : private boost::totally_ordered<VersionNumber>
{
public:
	VersionNumber();
	/**
	 * Construct-from-string.
	 *
	 * @param version_string The version number string to initialize this instance with.  Should be
	 *     of the format "N.N.N.N", where "N" is any number of decimal digits.
	 */
	explicit VersionNumber(const std::string &version_string);
	VersionNumber(const VersionNumber& orig);
	virtual ~VersionNumber();
	
	void Set(const std::string &version_string);
	
	/// @name Overloaded operators.
	/// Others are provided by Boost.Operators.
	//@{
	const VersionNumber operator=(const VersionNumber &other);
	bool operator==(const VersionNumber &other) const;
	bool operator<(const VersionNumber &other) const;
	//@}
	
	/// @name Conversion operators
	//@{

	/**
	 * Convert the VersionNumber to a std::string.
	 * 
     * @return the VersionNumber as a std::string.
     */
	operator std::string() const;
	//@}
	
	/**
	 * Check whether the version number is empty.  If it is, this could be due to it never having been
	 * assigned, or the attempt to assign to it was with a badly formatted string.
	 *
	 * @return true if empty().
	 */
	bool empty() const { return m_version_string.empty(); };
	
	/**
	 * Friend relation to the output stream inserter.
	 * Outputs the VersionNumber object @a ver as text to stream @a os.
	 *
	 * @param os The output stream to insert VersionNumber @a ver into.
	 * @param ver The VersionNumber object to insert into @a os.
	 * @return Reference to @a os.
	 */
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

