/**
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of coflo.
 *
 * coflo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * coflo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * coflo.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 #include "Function.h"
 
 
Function::Function(const std::string &function_id)
{
	m_function_id = function_id;
}
 
Function::~Function()
{
}
 
void Function::AddBlock(Block *block)
{
	m_block_list.push_back(block);
}