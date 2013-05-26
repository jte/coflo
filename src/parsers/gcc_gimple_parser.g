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
 
// Run this file through make_dparser to generate the parser.

/// C++ prefix code.
{

#include <iostream>
#include <fstream>
#include <string>
#include "gcc_gimple_parser.h"

#include "Location.h"
#include "controlflowgraph/statements/statements.h"
#include "controlflowgraph/statements/ParseHelpers.h"

#define D_ParseNode_Globals gcc_gimple_parser_ParseNode_Globals
#define D_ParseNode_User gcc_gimple_parser_ParseNode_User

#include <dparse.h>

// Redefine P_PN with C++ casts to avoid warnings.
#undef D_PN
#define D_PN(_x, _o) (reinterpret_cast<D_ParseNode*>(static_cast<char*>(_x) + _o))

extern D_ParserTables parser_tables_gcc_gimple_parser;

void gcc_gimple_parser_FreeNodeFn(D_ParseNode *d);

#define M_TO_STR(the_n) std::string(the_n.start_loc.s, the_n.end-the_n.start_loc.s)
#define M_PROPAGATE_PTR(from, to, field_name) do { to.field_name = from.field_name; from.field_name = NULL; } while(0) 

// The globals.
static gcc_gimple_parser_ParseNode_Globals TheGlobals;

D_Parser* new_gcc_gimple_Parser()
{
	D_Parser *parser = new_D_Parser(&parser_tables_gcc_gimple_parser, sizeof(D_ParseNode_User));
	parser->free_node_fn = gcc_gimple_parser_FreeNodeFn;
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

gcc_gimple_parser_ParseNode_User* gcc_gimple_parser_GetUserInfo(D_ParseNode *tree)
{
	return &(tree->user);
}

gcc_gimple_parser_ParseNode_Globals* gcc_gimple_parser_GetGlobalInfo(D_ParseNode *tree)
{
	return &TheGlobals;
}

void free_gcc_gimple_ParseTreeBelow(D_Parser *parser, D_ParseNode *tree)
{
	free_D_ParseTreeBelow(parser, tree);
}

void free_gcc_gimple_Parser(D_Parser *parser)
{
	free_D_Parser(parser);
}

void gcc_gimple_parser_FreeNodeFn(D_ParseNode *d)
{
	//std::cout << "FREE NODE" << std::endl;
}


void StatementListPrint(StatementList *the_list)
{
	StatementList::iterator it;
	for(it = the_list->begin(); it != the_list->end(); ++it)
	{
		dlog_parse_gimple << "Ptr = " << (*it);
		if((*it) != NULL)
		{
			dlog_parse_gimple << " : " << (*it)->GetIdentifierCFG();
		}
		dlog_parse_gimple << std::endl;
	}
}

}

/// DParser declarations.

/// Nonterminals.

gcc_gimple_file
	: {
		//$g = copy_globals($g);
		//$g->dummy_deleteme = &dummy_deleteme;
		//std::cout << "gcc_gimple_file" << std::endl;
	} function_definition_list
		{
			//std::cout << "DONE" << std::endl;
			TheGlobals.m_function_info_list = $1.m_function_info_list;
			M_PROPAGATE_PTR($1, $$, m_function_info_list);
		}
	;

function_definition_list
	: function_definition_list function_definition NL+
		{
			$0.m_function_info_list->push_back($1.m_function_info);
			M_PROPAGATE_PTR($0, $$, m_function_info_list);
		}
	| function_definition NL+
		{
			$$.m_function_info_list = new FunctionInfoList;
			$$.m_function_info_list->push_back($0.m_function_info);
		}
	;

function_definition
	// C code.
	: identifier '(' param_decls_list ')' NL location '{' NL (declaration_list NL)? statement_list '}'
		{
			dlog_parse_gimple << "Found function definition: " << *($0.m_str) << std::endl;
			dlog_parse_gimple << "  Location: " << *($5.m_location) << std::endl;
			dlog_parse_gimple << "  Num Statements: " << $9.m_statement_list->size() << std::endl;
			StatementListPrint($9.m_statement_list);
			$$.m_function_info = new FunctionInfo;
			$$.m_function_info->m_location = $5.m_location;
			$$.m_function_info->m_identifier = $0.m_str;
			$$.m_function_info->m_statement_list = $9.m_statement_list;
		}
	// C++ code.
	| decl_spec+ identifier '(' param_decls_list ')' '(' param_decls_list ')' NL location '{' NL (declaration_list NL)? statement_list '}'
		{
			dlog_parse_gimple << "Found C++ style function definition: " << *($1.m_str) << std::endl;
			StatementListPrint($13.m_statement_list);
			$$.m_function_info = new FunctionInfo;
			$$.m_function_info->m_location = $9.m_location;
			$$.m_function_info->m_identifier = $1.m_str;
			$$.m_function_info->m_statement_list = $13.m_statement_list;
		}
	;

location
	: real_location
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| /* Nothing */
		{ $$.m_location = new Location(); }
	;

real_location
	: '[' path ':' integer ':' integer ']'
		{ $$.m_location = new Location(M_TO_STR($n1), $3.m_int, $5.m_int); }
	| '[' path ':' integer ']'
		{ $$.m_location = new Location(M_TO_STR($n1), $3.m_int); }
	;
	
param_decls_list
	: param_decls_list ',' param_decls
		{
			$0.m_statement_list->push_back(NULL);
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| param_decls
		{ 
			$$.m_statement_list = new StatementList;
			$$.m_statement_list->push_back(NULL);
		}
	| /* Nothing */
		{
			$$.m_statement_list = new StatementList;
		}
	;
	
param_decls
	: decl_spec+ identifier?
	;

declaration_list
	: (declaration NL)+
	;

declaration
	: var_declaration
	| local_function_declaration
	;
	
var_declaration
	: decl_spec+ var_id declarator_suffix* ('=' (constant | string_literal))? ';'
		{ /*std::cout << "VAR_DECL: " << M_TO_STR($n1) << std::endl;*/ }
	;
	
declarator_suffix
	: '[' constant? ']'
	;
	
local_function_declaration
	: decl_spec+ identifier '(' param_decls_list ')' ';'
		{ /*std::cout << "LOCAL_FUNC_DECL: " << M_TO_STR($n1) << std::endl;*/ }
	;

statement_list
	: statement_list statement NL
		{
			$0.m_statement_list->push_back($1.m_statement);
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| statement NL
		{ 
			$$.m_statement_list = new StatementList;
			$$.m_statement_list->push_back($0.m_statement);
		}
	| statement_list scope NL
		{
			std::copy($1.m_statement_list->begin(), $1.m_statement_list->end(), std::back_inserter(*($0.m_statement_list)));
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| scope NL
		{
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| /* Nothing */
		{
			$$.m_statement_list = new StatementList;
		}
	;
	
statement
	: statement_one_line ';' post_line_text
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	// gcc 4.5.3 will emit "comment statements", in particular:
	// <location> '// predicted unlikely by continue predictor.'
	| location comment
		{
			std::cout << "Ignoring comment" << std::endl;
			$$.m_statement = new NoOp(Location());
		}
	| statement_possibly_split_across_lines
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| label_statement ';'?
		{
			M_PROPAGATE_PTR($0, $$, m_statement);
		}
	;
	
// Sometimes there is superfluous text after the end of a line.
// Absorb that here.
post_line_text
	: "\[[^\]]+\]"
	| /* Nothing. */
	;
	
statement_one_line
	: assignment_statement
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| return_statement
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| function_call
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| goto_statement
		{ M_PROPAGATE_PTR($0, $$, m_statement);	}
	;
	
return_statement
	: location 'return' var_id
		{
			$$.m_statement = new ReturnUnlinked(*($0.m_location), *($2.m_str));
		}
	| location 'return'
		{
			$$.m_statement = new ReturnUnlinked(*($0.m_location), "");
		}
	;
	
statement_possibly_split_across_lines
	: if
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	| switch
		{ M_PROPAGATE_PTR($0, $$, m_statement); }
	;

bitwise_binary_operator
	: '>>'
	| '<<'
	// gcc 4.5.3 will output this.
	| 'r<<'
	| '|'
	| '&'
	| '^'
	;
	
logical_binary_operator
	: '&&'
	| '||'
	;
	
arithmetic_binary_operator
	: '+'
	| '-'
	| '*'
	| '/'
	| '%'
	;

assignment_statement
	: real_location assignment_statement_internals
		{
			$1.m_statement->SetLocation(*($0.m_location));
			M_PROPAGATE_PTR($1,$$,m_statement);
		}
	;

assignment_statement_internals
	: lhs '=' rhs bitwise_binary_operator rhs
		{ $$.m_statement = new Placeholder(Location()); }
	| nested_lhs '=' cast_expression
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' '~' rhs
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' '-' rhs
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' rhs arithmetic_binary_operator rhs
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' '(' decl_spec+ ')' rhs
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' function_call
		{ M_PROPAGATE_PTR($2, $$, m_statement); }
	| lhs '=' ('MIN_EXPR' | 'MAX_EXPR') '<' argument_expression_list '>'
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' real_location 'BIT_FIELD_REF' '<' rhs ',' constant ',' constant '>'
		{ $$.m_statement = new Placeholder(Location()); }
	| lhs '=' condition
		{ $$.m_statement = new Placeholder(Location()); }
	| nested_lhs '=' rhs
		{ $$.m_statement = new Placeholder(Location()); }
	;
	
if
	// The style used in 4.5.3.
	: location 'if' '(' condition ')' goto_statement ';' 'else' goto_statement ';'
		{
			$$.m_str = new M_TO_STR($n3);
			$$.m_statement = new IfUnlinked(*($0.m_location),
					*($$.m_str),
					dynamic_cast<GotoUnlinked*>($5.m_statement),
					dynamic_cast<GotoUnlinked*>($8.m_statement));
			delete $$.m_str;
		}
	;
	
scope
	: location '{' NL declaration_list? NL statement_list '}'
		{ M_PROPAGATE_PTR($5, $$, m_statement_list); }
	;

// The GIMPLE output only appears to have var_id's or constants on the left side of the comparison operator.	
condition
	: var_or_constant comparison_operator rhs
	;
	
cast_expression
	: '(' type_name ')' rhs
	// Some gcc 4.5.3 builds handle string literals differently than others.  They result in statements which look like this:
	//
	// "i686-w64-mingw32-gcc-4.5.3 (GCC) 4.5.3":
	//   <location> <var_id> = (const char * restrict) &"the string literal"[0];
	//
	// var_id is then subsequently passed to a printf() call.  Other builds will dispense with the intermediate var_id and
	// just pass the literal to the function call directly:
	// 
	// "i686-pc-cygwin-gcc-4.5.3 (GCC) 4.5.3" : 
	//      <location> printf (<location> &<location> "the string literal"[0]);
	//
	// The below production is intended to handle the former.
	| '(' type_name ')' '&' string_literal '[' constant ']'
	;
	
type_name
	: decl_spec+
	;
	
var_or_constant
	: var_id
	| constant
	;
	
comparison_operator
	: '>'
	| '<'
	| '=='
	| '>='
	| '<='
	| '!='
	;
	
function_call
	: location identifier '(' argument_expression_list ')'
		{
			$$.m_statement = new FunctionCallUnresolved(M_TO_STR($n1), *($0.m_location), M_TO_STR($n3));
		}
	| location identifier_ssa '(' argument_expression_list ')'
		{
			// This should pretty much always be a function call through a compiler-generated function
			// pointer.
			$$.m_statement = new FunctionCallUnresolved(M_TO_STR($n1), *($0.m_location), M_TO_STR($n3));
		}
	;

goto_statement
	: location 'goto' synthetic_label_id
		{
			$$.m_statement = new GotoUnlinked(*($0.m_location), *($2.m_str));
		}
	| location 'goto' identifier
		{
			$$.m_statement = new GotoUnlinked(*($0.m_location), *($2.m_str));
		}
	;
	
label_statement
	: location synthetic_label_id ':'
		{
			$$.m_statement = new Label(*($0.m_location), *($1.m_str));
		}
	| location identifier ':'
		{
			$$.m_statement = new Label(*($0.m_location), *($1.m_str));
		}
	;

switch
	: location 'switch' '(' var_or_constant ')' switch_case_list
		{
			$$.m_statement = new SwitchUnlinked(*($0.m_location));
			$0.m_location = NULL;
			
			StatementList::iterator it;
			for(it=$5.m_statement_list->begin(); it != $5.m_statement_list->end(); ++it)
			{
				dynamic_cast<SwitchUnlinked*>($$.m_statement)->InsertCase(dynamic_cast<CaseUnlinked*>(*it));
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
			$0.m_statement_list->push_back($2.m_statement);
			M_PROPAGATE_PTR($0, $$, m_statement_list);
		}
	| case_453
		{
			$$.m_statement_list = new StatementList;
			$$.m_statement_list->push_back($0.m_statement);
		}
	;

case
	: location 'case' rhs ':' goto_statement ';'
	| location 'default:' goto_statement ';'
	;
	
// Case style used by gcc 4.5.3, 4.6.1.
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

argument_expression_list
	: argument_expression_list ',' rhs
	| rhs
	| /* Nothing */
	;

decl_spec
	: type_qualifier
	| storage_class_specifier
	| function_specifier  /* C++ */
	| 'struct' identifier 
	| 'short'
	| 'signed'
	| 'unsigned'
	| 'char'
	| 'int'
	| 'long'
	| 'float'
	| 'double'
	| 'void'
	| '*'
	| identifier
	| '[' constant ']'
	| "\<[A-Za-z]+[A-Za-z0-9]+\>"	/* Not sure what this is, see it as "<T{hex_number}>" in function pointer decls. */
	| '(' decl_spec+ (',' decl_spec+)* ')'
	// gcc 4.5.3 emits this one in some C code.  First encountered it when running against make 3.82 sources.
	| '<unnamed type>'
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
	
function_specifier
	: 'inline'
	| 'virtual'
	| 'explicit'
	;

lhs
	: nested_lhs
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	;
	
rhs
	: nested_rhs
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	;
	
nested_lhs
	: location nested_lhs '.' var_id
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| location nested_lhs '->' identifier
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| location nested_lhs '[' var_or_constant ']'
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| location '*' var_id
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| var_id
		{ $$.m_location = new Location(); }
	;
	
nested_rhs
	: location nested_rhs '.' var_id
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| location nested_rhs '->' identifier
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| location nested_rhs '[' var_or_constant ']'
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| location '*' nested_rhs
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| real_location '&' nested_rhs
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| var_id
		{ $$.m_location = new Location(); }
	| constant
		{ $$.m_location = new Location(); }
	| location string_literal
		{ M_PROPAGATE_PTR($0, $$, m_location); }
	| real_location var_id
		{
			// With gcc 4.5.3, this seems to always be a callback pointer being passed to a function.
			{ M_PROPAGATE_PTR($0, $$, m_location); }
		}
	;

constant
	: integer_decimal 'B'? /* Not sure what the 'B' represents.  Saw '0B', '35B', etc. */
	| integer_hex
	| literal_floating_point
	| '<<< error >>>'
	;

var_id
	: identifier_ssa
		{ M_PROPAGATE_PTR($0, $$, m_str); }
	| identifier
		{ M_PROPAGATE_PTR($0, $$, m_str); }
	// gcc 4.5.3 sometimes outputs '<retval>' in returns which return a value.
	| '<retval>'
		{ $$.m_str = new std::string("<retval>"); }
	;
	
synthetic_label_id
	: '<' identifier_ssa '>'
		{
			$$.m_str = new std::string;
			*($$.m_str) = "<" + *($1.m_str) + ">";
			$1.m_str = NULL;
		}
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
identifier_ssa
	: "[a-zA-Z_][a-zA-Z0-9_]*.[0-9]+" $term -2
		{ $$.m_str = new M_TO_STR($n0); }
	;
identifier
	: "[a-zA-Z_][a-zA-Z0-9_]*" $term -3
		{ $$.m_str = new M_TO_STR($n0); }
	;
comment: "\/\/[^\n]+";
