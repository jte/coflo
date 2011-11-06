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
 
// Run this file through make_dparser to generate the parser.

/// C++ prefix code.
{

#include <iostream>
#include <fstream>
#include <string>
#include "gcc_gimple_parser.h"

#include "Location.h"
#include "controlflowgraph/statements/statements.h"

#define D_ParseNode_Globals gcc_gimple_parser_ParseNode_Globals
#define D_ParseNode_User gcc_gimple_parser_ParseNode_User

#include <dparse.h>

// Redefine this with C++ casts.
#define D_PN(_x, _o) (reinterpret_cast<D_ParseNode*>(static_cast<char*>(_x) + _o))

extern D_ParserTables parser_tables_gcc_cfg_parser;

void gcc_cfg_parser_FreeNodeFn(D_ParseNode *d);

#define M_TO_STR(the_n) std::string(the_n.start_loc.s, the_n.end-the_n.start_loc.s)
#define M_PROPAGATE_PTR(from, to, field_name) do { to.field_name = from.field_name; from.field_name = NULL; } while(0) 

D_Parser* new_gcc_gimple_Parser()
{
	D_Parser *parser = new_D_Parser(&parser_tables_gcc_cfg_parser, sizeof(D_ParseNode_User));
	parser->free_node_fn = gcc_cfg_parser_FreeNodeFn;
	parser->commit_actions_interval = 0;
	parser->error_recovery = 1;
	parser->save_parse_tree = 1;
	
	return parser;
}

D_ParseNode* gcc_gimple_dparse(D_Parser *parser, char* buffer, long length)
{
	D_ParseNode *tree = dparse(parser, buffer,length);
	return tree;
}

long gcc_gimple_parser_GetSyntaxErrorCount(D_Parser *parser)
{
	return parser->syntax_errors;
}

void free_gcc_gimple_ParseTreeBelow(D_Parser *parser, D_ParseNode *tree)
{
	free_D_ParseTreeBelow(parser, tree);
}

void free_gcc_gimple_Parser(D_Parser *parser)
{
	free_D_Parser(parser);
}

void gcc_cfg_parser_FreeNodeFn(D_ParseNode *d)
{
	//std::cout << "FREE NODE" << std::endl;
}


void StatementListPrint(StatementList *the_list)
{
	StatementList::iterator it;
	for(it = the_list->begin(); it != the_list->end(); ++it)
	{
		std::cout << "Ptr = " << (*it)->m_statement;
		if((*it)->m_statement != NULL)
		{
			std::cout << " : " << (*it)->m_statement->GetIdentifierCFG();
		}
		std::cout << std::endl;
	}
}

}

/// DParser declarations.

/// Nonterminals.

gcc_cfg_file
	: {
		//$g = copy_globals($g);
		//$g->dummy_deleteme = &dummy_deleteme;
		std::cout << "gcc_cfg_file" << std::endl;
	} file_contents+
	;

file_contents
	: function_definition
	| NL
	;

function_definition
	// C code.
	: identifier '(' ')' NL location '{' NL (declaration_list NL)? statement_list '}'
		{
			std::cout << "Found function definition: " << M_TO_STR($n0) << std::endl;
			std::cout << "  Location: " << *($4.m_location) << std::endl;
			std::cout << "  Num Statements: " << $8.m_statement_list->size() << std::endl;
			StatementListPrint($8.m_statement_list);
		}
	| identifier '(' param_decls? ')' NL location '{' NL (declaration_list NL)? statement_list '}'
		{
			std::cout << "Found function definition: " << M_TO_STR($n0) << std::endl;
			std::cout << "  Location: " << *($5.m_location) << std::endl;
			std::cout << "  Num Statements: " << $9.m_statement_list->size() << std::endl;
			StatementListPrint($9.m_statement_list);
		}
	// C++ code.
	| decl_spec+ identifier '(' param_decls? ')' '(' param_decls? ')' NL location '{' NL (declaration_list NL)? statement_list '}'
		{
			std::cout << "Found C++ style function definition: " << M_TO_STR($n1) << std::endl;
			StatementListPrint($14.m_statement_list);
		}
	;

location
	: '[' path ':' integer ':' integer ']'
		{ $$.m_location = new Location(M_TO_STR($n1), $3.m_int, $5.m_int); }
	| '[' path ':' integer ']'
		{ $$.m_location = new Location(M_TO_STR($n1), $3.m_int); }
	| /* No location. */
		{
			$$.m_location = new Location("UNKNOWN",0);
		}
	;
	
param_decls
	: decl_spec+ identifier? (',' decl_spec+ identifier?)*
	;

declaration_list
	: (declaration NL)+
	;

declaration
	: var_declaration
	| local_function_declaration
	;
	
var_declaration
	: decl_spec+ var_id declarator_suffix* ('=' constant)? ';'
		{ /*std::cout << "VAR_DECL: " << M_TO_STR($n1) << std::endl;*/ }
	;
	
declarator_suffix
	: '[' constant? ']'
	;
	
local_function_declaration
	: decl_spec+ identifier '(void)' ';'
		{ /*std::cout << "LOCAL_FUNC_DECL: " << M_TO_STR($n1) << std::endl;*/ }
	;

statement_list
	: statement_list statement NL
		{
			/*std::cout << "STATEMENT: \"" << M_TO_STR($n1) << "\"" << std::endl;*/
			$0.m_statement_list->push_back(new StatementListEntry($1.m_statement));
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| statement NL
		{ 
			/*std::cout << "STATEMENT: \"" << M_TO_STR($n0) << "\"" << std::endl;*/
			$$.m_statement_list = new StatementList;
			$$.m_statement_list->push_back(new StatementListEntry($0.m_statement));
		}
	| /* Nothing */
		{
			/*std::cout << "NO STATEMENT" << std::endl;*/
		}
	;
	
statement
	: statement_one_line ';'
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| location comment
		{ std::cout << "Ignoring comment" << std::endl; $$.m_statement = NULL; }
	| statement_possibly_split_across_lines
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| scope
	| label ';'?
		{ /*std::cout << "STATEMENT LABEL: " << *($0.m_str) << std::endl;*/ }
	;
	
post_one_line_statement_text
	: "\[[^\]]+\]"
	;
	
statement_one_line
	: assignment
	| return_statement
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| function_call
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| goto_statement
		{ M_PROPAGATE_PTR($0, $$, m_statement);	}
	;
	
// A goto that's not part of another statement such as "if" or "switch", but is a stand-alone statement.
/*goto_statement
	: location goto
		{ $$.m_statement = new Goto(*($1.m_location)); }
	;
	*/
return_statement
	: location 'return' var_id
	| location 'return'
	;
	
statement_possibly_split_across_lines
	: if
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| switch
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	/*| comment*/
	;

assignment
	: lhs '=' rhs '>>' rhs
	| lhs '=' rhs '<<' rhs
	| lhs '=' rhs '|' rhs
	| lhs '=' rhs '&' rhs
	| lhs '=' location? rhs
	| lhs '=' '~' rhs
	| lhs '=' '-' rhs
	| lhs '=' rhs '+' rhs
	| lhs '=' rhs '-' rhs
	| lhs '=' rhs '*' rhs
	| lhs '=' rhs '/' rhs
	| lhs '=' rhs '%' rhs
	| lhs '=' '(' decl_spec+ ')' rhs
	| lhs '=' function_call
	| lhs '=' 'MIN_EXPR' '<' fc_param_list '>'
	| lhs '=' condition
	;
	
if
	// The style used in 4.5.3.
	: location 'if' '(' condition ')' goto_statement ';' 'else' goto_statement ';'
		{
			$$.m_statement_list = new StatementList();
			StatementListEntry *sb = new StatementListEntry(new FlowControlUnlinked(*($0.m_location),
					dynamic_cast<GotoUnlinked*>($5.m_statement),
					dynamic_cast<GotoUnlinked*>($8.m_statement))
				);
			/// @todo Get condition in here.
			$$.m_statement_list->push_back(sb);
		}
	;
	
scope
	: location '{' NL declaration_list? NL statement_list '}'
	;
	
condition
	: lhs '>' rhs
	| lhs '<' rhs
	| lhs '==' rhs
	| lhs '>=' rhs
	| lhs '<=' rhs
	| rhs '!=' rhs
	;
	
function_call
	: location identifier '(' fc_param_list? ')'
		{
			/*std::cout << "Function Call: " << M_TO_STR($n1) << std::endl;
			std::cout << "   Param list: " << M_TO_STR($n3) << std::endl;*/
			$$.m_statement = new FunctionCallUnresolved(M_TO_STR($n1), *($0.m_location), M_TO_STR($n3));
		} 
	| identifier '(' fc_param_list? ')'
	;

goto_statement
	: location 'goto' synthetic_label_id
		{
			/*$$.m_str = $2.m_str;*/
			$$.m_statement = new GotoUnlinked(*($0.m_location), *($2.m_str));
		}
	| location 'goto' identifier
		{
			/*$$.m_str = $2.m_str;*/
			$$.m_statement = new GotoUnlinked(*($0.m_location), *($2.m_str));
		}
	;
	
label
	: synthetic_label_id ':'
		{ $$.m_str = new M_TO_STR($n0); }
	| location identifier ':'
		{ $$.m_str = new M_TO_STR($n1); }
	;

switch
	: location 'switch' '(' rhs ')' switch_case_list
		{
			$$.m_statement = new SwitchUnlinked(*($0.m_location));
			$0.m_location = NULL;
			
			StatementList::iterator it;
			for(it=$5.m_statement_list->begin(); it != $5.m_statement_list->end(); ++it)
			{
				dynamic_cast<SwitchUnlinked*>($$.m_statement)->InsertCase(dynamic_cast<CaseUnlinked*>((*it)->m_statement));
			}
		}
	;
	
switch_case_list
	// Style used in 4.5.3.
	: '<' switch_case_list_453  '>'
		{ M_PROPAGATE_PTR($1, $$, m_statement_list); }
	// Style used in ??? (4.5.2?)
	| '{' case* '}'
		{ std::cerr << "SWITCH CASE STYLE UNIMPLEMENTED" << std::endl; }
	;

switch_case_list_453
	: switch_case_list_453 ',' case_453
		{
			$0.m_statement_list->push_back(new StatementListEntry($1.m_statement));
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| case_453
		{
			$$.m_statement_list = new StatementList;
			StatementListEntry *sb = new StatementListEntry($0.m_statement);
			$$.m_statement_list->push_back(sb);
		}
	;

/************************/

case
	: location 'case' rhs ':' goto_statement ';'
	| location 'default:' goto_statement ';'
	;
	
case_453
	: location 'case' rhs ':' synthetic_label_id
		{
			$$.m_statement = new CaseUnlinked(*($0.m_location), *($4.m_str));
		}
	| location 'default:' synthetic_label_id
		{
			$$.m_statement = new CaseUnlinked(*($0.m_location), *($2.m_str));
		}
	;

fc_param_list
	: rhs (',' rhs)*
	;

decl_spec
	: type_qualifier
	| storage_class_specifier
	| 'struct' identifier 
	| 'short'
	| 'signed'
	| 'unsigned'
	| 'char'
	| 'int'
	| 'long'
	| 'double'
	| 'void'
	| '*'
	| identifier
	| '[' constant ']'
	| "\<[A-Za-z]+[0-9]+\>"	/* Not sure what this is, saw it as "<Te8>". */
	| '(' decl_spec+ ')'
	| '(' decl_spec+ (',' decl_spec+)+ ')'
	;
	
type_qualifier
	: 'const'
	| 'volatile'
	| 'restrict'
	;

storage_class_specifier
	: 'static'
	| 'extern'
	;

lhs
	: location? unary_expression
	;
	
rhs
	: location unary_expression
	;
	
unary_expression
	: location? postfix_expression
	| unary_operator location? unary_expression
	;

unary_operator : '&' | '*' ;

postfix_expression
	: primary_expression
	('[' postfix_expression ']' 
	| '.' identifier
	| '->' identifier
	)* ;

primary_expression 
  : var_id
  | constant
  | string_literal+
  ;

constant
	: integer_decimal 'B'? /* Not sure what the 'B' represents.  Seems to always be '0B'. */
	| integer_hex
	| literal_floating_point
	| '<<< error >>>'
	;

var_id
	: identifier_ssa
	| identifier
	;
	
synthetic_label_id
	: '<' identifier_ssa '>'
		{ $$.m_str = new M_TO_STR($n1); *($$.m_str) = "<" + *($$.m_str) + ">"; }
	;

/// Terminals.

NL: '\n';
whitespace: "[ \t]+";
integer : "[0-9]+" { $$.m_int = atoi(M_TO_STR($n0).c_str()); } ;
integer_decimal: "[\-\+]?[1-9][0-9]*" | '0';
integer_hex: "0x[0-9A-Fa-f]+";
literal_floating_point: "[0-9]+.[0-9]+(e[\+\-][0-9]+)?";
string_literal: "\"([^\"\\]|\\[^])*\"";
path: "[a-zA-Z0-9_\.\-\\\/]+" $term -1;
identifier_ssa: "[a-zA-Z_][a-zA-Z0-9_]*.[0-9]+" $term -2;
identifier
	: "[a-zA-Z_][a-zA-Z0-9_]*" $term -3
		{ $$.m_str = new M_TO_STR($n0); }
	;
comment: "\/\/[^\n]+";
