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

#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>

class Function;
class Successor;

class Block
{
public:
	typedef std::vector< std::string > T_FUNCTION_CALL_LIST;
	typedef T_FUNCTION_CALL_LIST::iterator T_FUNCTION_CALL_LIST_ITERATOR;
	typedef std::vector< Successor * > T_BLOCK_SUCCESSOR_LIST;
	typedef T_BLOCK_SUCCESSOR_LIST::iterator T_BLOCK_SUCCESSOR_ITERATOR;

	Block(Function * parent_function, long block_number, long block_starting_line_in_src);
	~Block();

	long GetBlockNumber() const { return m_block_number; };

	void AddFunctionCall(const std::string &function_name);

	/**
	 * Extract the successor block information from the passed string.  The
	 * string is expected to be the trailing text of a "#SUCC: <<<this here>>>"
	 * line in the input file.
	 *
	 * @param successors_string Note that we deliberately are passing this by value,
	 * not by reference, because the function will use it as a temporary.
	 */
	void AddSuccessors(std::string successors_string);

	Successor* GetSuccessorAtIndex(std::vector< Successor * >::size_type index) const;

	void PrintBlock(long indent_level);

	/// \name Iterators.
	T_FUNCTION_CALL_LIST_ITERATOR begin() { return m_function_calls.begin(); };
	T_FUNCTION_CALL_LIST_ITERATOR end() { return m_function_calls.end(); };

	T_BLOCK_SUCCESSOR_ITERATOR successor_begin() { return m_successor_list.begin(); };
	T_BLOCK_SUCCESSOR_ITERATOR successor_end() { return m_successor_list.end(); };

private:

	/// Pointer to the function which contains this block.
	Function *m_parent_function;

	/// The block number.
	long m_block_number;

	/// The source line it starts on.
	long block_starting_line_in_src;

	/// List of the function calls in this block.
	T_FUNCTION_CALL_LIST m_function_calls;

	/// List of successors to this block.
	T_BLOCK_SUCCESSOR_LIST m_successor_list;

};

#endif // BLOCK_H
