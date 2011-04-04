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
 
#ifndef FUNCTION_H
#define FUNCTION_H
 
#include <string>
#include <vector>

class Block;
 
class Function
{
public:
	Function(const std::string &m_function_id);
	~Function();
	
	void AddBlock(Block *block);
	
private:
	
	/// Function identifier.
	std::string m_function_id;
	
	/// Block list.
	std::vector < Block * > m_block_list;
	
};
 
#endif // FUNCTION_H