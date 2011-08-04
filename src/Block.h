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

#ifndef BLOCK_H
#define BLOCK_H

#include <iosfwd>
#include <vector>
#include <string>

class Function;
class Successor;
class StatementBase;

/**
 * Class encapsulating a basic block.
 */
class Block
{
public:
	typedef std::vector< StatementBase* > T_STATEMENT_LIST;
	typedef T_STATEMENT_LIST::iterator T_STATEMENT_LIST_ITERATOR;
	
	typedef std::vector< Successor * > T_BLOCK_SUCCESSOR_LIST;
	typedef T_BLOCK_SUCCESSOR_LIST::iterator T_BLOCK_SUCCESSOR_ITERATOR;

	Block(Function * parent_function, long block_number, long block_starting_line_in_src);
	~Block();
	
	/**
	 * Parse a basic block from @a input_stream, assumed to be a .cfg file from gcc.
	 *
	 * @param input_stream
	 * @return
	 */
	bool Parse(std::istream &input_stream);

	bool IsENTRY() const { return m_block_number == 0; };
	
	bool IsEXIT() const { return m_block_number == 1; };
	
	long GetBlockNumber() const { return m_block_number; };
	
	long GetBlockStartingLineNo() const { return m_block_starting_line_in_src; };
	
	/**
	 * Returns text appropriate for labeling the block in a GraphViz dot graph.
	 * 
     * @return The appropriate block label text.
     */
	std::string GetBlockLabel() const { return m_block_label; };

	/**
	 * Add the next statement to the block.
	 * 
     * @param statement Pointer to the Statement object to add.
     */
	void AddStatement(StatementBase *statement);
	
	T_STATEMENT_LIST::size_type NumberOfStatements() const { return m_statement_list.size(); };

	/**
	 * Extract the successor block information from the passed string.  The
	 * string is expected to be the trailing text of a "#SUCC: <<<this here>>>"
	 * line in the input file.
	 *
	 * @param successors_string Note that we deliberately are passing this by value,
	 * not by reference, because the function will use it as a temporary.
	 */
	void AddSuccessors(std::string successors_string);

	void PrintBlock(long indent_level);

	/// @name Iterators.
	//@{
	
	/// Returns the begin() iterator for statements in this block.
	T_STATEMENT_LIST_ITERATOR begin() { return m_statement_list.begin(); };
	
	/// Returns the end() iterator for statements in this block.
	T_STATEMENT_LIST_ITERATOR end() { return m_statement_list.end(); };

	/// Returns the begin iterator for iterating through successors.
	T_BLOCK_SUCCESSOR_ITERATOR successor_begin() { return m_successor_list.begin(); };
	
	/// Returns the begin iterator for iterating through successors.
	T_BLOCK_SUCCESSOR_ITERATOR successor_end() { return m_successor_list.end(); };
	
	//@}
	
private:

	/// Pointer to the function which contains this block.
	Function *m_parent_function;

	/// The block number.
	long m_block_number;
	
	std::string m_block_label;

	/// The source line it starts on.
	long m_block_starting_line_in_src;

	/// List of the statements in this block.
	T_STATEMENT_LIST m_statement_list;

	/// List of successors to this block.
	T_BLOCK_SUCCESSOR_LIST m_successor_list;

};

#endif // BLOCK_H
