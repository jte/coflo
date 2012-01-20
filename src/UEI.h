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

#ifndef UEI_H
#define UEI_H

#include <string>

/**
 * Universal Element Identifier (UEI) class.
 *
 * This class unambiguously identifies a class, class member, free function, define, or any other member
 * which must be referred to.
 *
 */
class UEI
{
public:
	UEI();
	UEI(const UEI &other);
	UEI(const std::string &s);
	virtual ~UEI();

	std::string asString() const;

private:

	/// The UEI in std::string format.
	std::string m_as_string;
};

#endif /* UEI_H_ */
