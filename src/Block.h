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
 
#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>

class Function;
class Successor;

class Block
{
public:
	Block(Function * parent_function, long block_number, long block_starting_line_in_src);
	~Block();
	
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
	
private:

    /// Pointer to the function which contains this block.
    Function *m_parent_function;
    
    /// The block number.
    long m_block_number;

    /// The source line it starts on.
    long block_starting_line_in_src;

    /// List of the function calls in this block.
    std::vector< std::string > m_function_calls;

    /// List of successors to this block.
    std::vector< Successor * > m_successor_list;
	
};

#endif // BLOCK_H
