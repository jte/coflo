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

#include <boost/regex.hpp>

#include "Successor.h"

Successor::Successor(long block_number)
{
	m_block_number = block_number;
	m_block_ptr = NULL;
}

Successor::Successor(const std::string &block_number_as_string)
{
	m_block_number = atoi(block_number_as_string.c_str());
	m_block_ptr = NULL;
}

Successor::Successor(const Successor& orig)
{
	m_block_number = orig.m_block_number;
	m_block_ptr = NULL;
}

Successor::~ Successor()
{
}

