/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#ifndef GCC_GIMPLE_PARSER_H
#define GCC_GIMPLE_PARSER_H

#include <vector>
#include <string>

/// Forward declaration of the parse node struct.
struct D_ParseNode;
struct D_Parser;

// Forward declarations for the User data.
class Location;
class StatementBase;
class TranslationUnit;


typedef std::vector< StatementBase*> StatementList;
typedef std::vector< std::string > StringList;

struct FunctionInfo
{
	Location *m_location;
	std::string *m_identifier;
	StatementList *m_statement_list;
};

typedef std::vector< FunctionInfo* > FunctionInfoList;

struct gcc_gimple_parser_ParseNode_Globals
{
	TranslationUnit *m_translation_unit;
	FunctionInfoList *m_function_info_list;
};
/// Type of the object that gets passed through the parse tree.
struct gcc_gimple_parser_ParseNode_User
{
	long m_int;
	std::string *m_str;
	Location *m_location;
	StatementBase *m_statement;
	StatementList *m_statement_list;
	StringList *m_string_list;
	FunctionInfo *m_function_info;
	FunctionInfoList *m_function_info_list;
};

D_Parser* new_gcc_gimple_Parser();
D_ParseNode* gcc_gimple_dparse(D_Parser *parser, char* buffer, long length);
long gcc_gimple_parser_GetSyntaxErrorCount(D_Parser *parser);
gcc_gimple_parser_ParseNode_User* gcc_gimple_parser_GetUserInfo(D_ParseNode *tree);
gcc_gimple_parser_ParseNode_Globals* gcc_gimple_parser_GetGlobalInfo(D_ParseNode *tree);
void free_gcc_gimple_ParseTreeBelow(D_Parser *parser, D_ParseNode *tree);
void free_gcc_gimple_Parser(D_Parser *parser);

#endif /* GCC_GIMPLE_PARSER_H*/
