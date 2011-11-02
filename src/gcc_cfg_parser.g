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

#define D_ParseNode_User gcc_gimple_parser_ParseNode_User

#include <dparse.h>

extern D_ParserTables parser_tables_gcc_cfg_parser;

void gcc_cfg_parser_FreeNodeFn(D_ParseNode *d);

#define M_TO_STR(the_n) std::string(the_n.start_loc.s, the_n.end-the_n.start_loc.s)

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

void gcc_cfg_parser_FreeNodeFn(struct D_ParseNode *d)
{
	//std::cout << "FREE NODE" << std::endl;
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
	: identifier '(' ')' NL location '{' NL (declaration_list NL)? statement_list? '}'
		{
			std::cout << "Found function definition: " << M_TO_STR($n0) << std::endl;
			std::cout << "  Location: " << *($4.m_location) << std::endl; 
		}
	| identifier '(' param_decls? ')' NL location '{' NL (declaration_list NL)? statement_list? '}'
		{std::cout << "Found function definition: " << M_TO_STR($n0) << std::endl;} 
	;

location
	: '[' path ':' integer ':' integer ']'
		{ $$.m_location = new Location(M_TO_STR($n1), $3.m_int, $5.m_int) ; std::cout << *($$.m_location) << std::endl; }
	| '[' path ':' integer ']'
		{ $$.m_location = new Location(M_TO_STR($n1), $3.m_int) ; std::cout << *($$.m_location) << std::endl; }
	;
	
param_decls
	: decl_spec* identifier (',' decl_spec* identifier)*
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
		{ std::cout << "VAR_DECL: " << M_TO_STR($n1) << std::endl; }
	;
	
declarator_suffix
	: '[' constant? ']'
	;
	
local_function_declaration
	: decl_spec+ identifier '(void)' ';'
		{ std::cout << "LOCAL_FUNC_DECL: " << M_TO_STR($n1) << std::endl; }
	;

statement_list
	: (statement { std::cout << $n.symbol << std::endl; } NL)+
	;
	
statement
	: location? statement_one_line ';'
	| location comment
		{ std::cout << "Ignoring comment" << std::endl; }
	| location? statement_possibly_split_across_lines
	| scope
	| label ';'?
	;
	
post_one_line_statement_text
	: "\[[^\]]+\]"
	;
	
statement_one_line
	: assignment
	| 'return' var_id
	| 'return'
	| function_call
	| goto
	;
	
statement_possibly_split_across_lines
	: if
	| switch
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
	| lhs '=' location? function_call
	| lhs '=' 'MIN_EXPR' '<' fc_param_list '>'
	| lhs '=' condition
	;
	
if
	: 'if' '(' location? condition ')' '{' statement* '}' ('else' '{' statement* '}')?
	// The style used in 4.5.3.
	| 'if' '(' condition ')' goto ';' 'else' goto ';'
		{ std::cout << "DECISION NODE: IF\n"
			<< "  Condition: " << M_TO_STR($n2) << "\n"
			<< std::endl;
		}
	;
	
scope
	: location? '{' NL declaration_list? NL statement_list? '}'
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
	: identifier '(' fc_param_list? ')'
		{
			std::cout << "Function Call: " << M_TO_STR($n0) << std::endl;
			std::cout << "   Param list: " << M_TO_STR($n2) << std::endl; 
		} 
	;

goto
	: 'goto' (synthetic_label_id|identifier)
	;
	
label
	: synthetic_label_id ':'
	| location? identifier ':'
	;

switch
	: 'switch' '(' rhs ')' '{' case* '}'
	// Style used in 4.5.3.
	| 'switch' '(' rhs ')' '<' case_453 (',' case_453)* '>'
	;

/************************/

case
	: location? 'case' rhs ':' goto ';'
	| location? 'default' ':' goto ';'
	;
	
case_453
	: location? 'case' rhs ':' synthetic_label_id
	| location? 'default' ':' synthetic_label_id
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
	: location? unary_expression
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
identifier: "[a-zA-Z_][a-zA-Z0-9_]*" $term -3;
comment: "\/\/[^\n]+";
