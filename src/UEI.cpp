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

#include "UEI.h"

UEI::UEI()
{
	// TODO Auto-generated constructor stub

}

UEI::UEI(const UEI & other)
{
	m_as_string = other.m_as_string;
}

UEI::UEI(const std::string & s)
{
	m_as_string = s;
}

UEI::~UEI()
{
	// TODO Auto-generated destructor stub
}

std::string UEI::asString() const
{
	return m_as_string;
}



