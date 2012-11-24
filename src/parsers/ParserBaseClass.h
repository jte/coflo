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

/** @ file */

#ifndef PARSERBASECLASS_H
#define PARSERBASECLASS_H

#include <string>

struct D_ParseNode;
struct D_Parser;
struct D_ParserTables;
struct D_Scope;


class ParserBaseClass
{
public:
	ParserBaseClass(const std::string &filename, D_ParserTables *t);
	virtual ~ParserBaseClass();

	/**
	 * Sets the verbosity level of the parser.  For debugging and diagnostic purposes.
	 *
	 * @param val The verbosity level to set.  Valid values are:
	 * 	- 0 (the default): disable diagnostic output
	 * 	- 1              : enable diagnostic output during parse.
	 * 	- 2              : enable even more diagnostic output during parse.
	 */
	void SetVerboseLevel(int val);
	void SetDebugLevel(int val);

	/**
	 * Load the file, parse it, and return a pointer to the root parse node.
	 *
	 * @return Pointer to the root parse node.
	 */
	D_ParseNode* Parse();

	long GetSyntaxErrorCount() const;

	void PrintSubtree(const D_ParseNode *start_node);
	void PrintScope(D_Scope *scope);

	void* GetUserInfoAsVoidPtr(D_ParseNode *tree);
	void* GetGlobalInfoAsVoidPtr();

private:

	virtual size_t GetUserDataSize() = 0;
	virtual void* InitGlobalData() { return NULL; };

	int m_verbose_level;
	int m_debug_level;

	/// The filename of the file being parsed.
	std::string m_filename;

	D_ParserTables *m_parser_tables;

	/// The DParser instance.
	D_Parser *m_parser;

	std::string m_buffer;

	D_ParseNode *m_root_parsenode;
};

#endif // PARSERBASECLASS_H
