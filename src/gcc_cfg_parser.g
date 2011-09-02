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
#include <dparse.h>

/// Type of the object that gets passed through the parse tree.
struct GCCCFGFileGlobals
{
	int dummy_deleteme;
};

//#define D_ParseNode_Globals GCCCFGFileGlobals

using namespace std;

#define M_TO_STR(the_n) string(the_n.start_loc.s, the_n.end-the_n.start_loc.s)

// Extern of the parser in hamiltons_grammar.c.
extern D_ParserTables parser_tables_gram;

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout << "No file" << endl;
	}
	
	// Load the given file into memory.
    std::string buffer;
    ifstream in(argv[1]);

    while (in.good())     // loop while extraction from file is possible
    {
        char c;
        c = in.get();       // get character from file
        if (in.good())
        {
            buffer += c;
        }
    }

    in.close();           // close file

    //std::cout << "Read :" << buffer;

    // Create a new parser.
    D_Parser *parser = new_D_Parser(&parser_tables_gram, 0);
    D_ParseNode *tree = dparse(parser, (char*)buffer.c_str(), buffer.length());

    if (tree && !parser->syntax_errors)
    {
        cout << "success\n";
    }
    else
    {
       cout << "failure\n";
    }

    // Destroy the parser.
    free_D_Parser(parser);
    
    return 0;
}

}


/// Nonterminals.

gcc_cfg_file:
	{
		//$g = copy_globals($g);
		//$g->dummy_deleteme = &dummy_deleteme;
		std::cout << "gcc_cfg_file" << std::endl;
	}
	function_definition*
	;

function_definition
	: identifier '(' param_decls? ')' location? '{' declaration* statement* '}'
		{
			cout << "Found function: " << M_TO_STR($n0) << endl; 
		}
	;

location
	: '[' path ':' linenumber (':' column)? ']'
	;
	
param_decls
	: decl_spec* identifier (',' decl_spec* identifier)*
	;

declaration
	: decl_spec+ var_id ('[' integer ']')? ';'
	| 'void' identifier '=' '<<<' 'error' '>>>' ';'
	;
	
statement
	: location? statement_with_optional_location
	| scope
	| label ';'?
	;
	
statement_with_optional_location
	: assignment ';'
	| 'return' var_id ';'
	| 'return' ';'
	| if
	| function_call ';'
	| goto ';'
	| switch
	| comment
	;
	
assignment
	: lhs '=' rhs '>>' rhs
	| lhs '=' rhs '<<' rhs
	| lhs '=' var_id '|' var_id
	| lhs '=' rhs '&' rhs
	| lhs '=' location? rhs
	| lhs '=' '~' rhs
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
	;
	
scope
	: location? '{' declaration* statement* '}'
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
			cout << "Function Call: " << M_TO_STR($n0) << endl; 
		} 
	;

goto
	: 'goto' synthetic_label_id
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

comment
	: '// predicted unlikely by continue predictor.'
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
	| 'struct' identifier 
	| 'short'
	| 'signed'
	| 'unsigned'
	| 'char'
	| 'int'
	| 'long'
	| 'double'
	/* These are typedefs in bzip2.c */
	| 'IntNative'
	| 'Int32'
	| 'Char'
	| 'Bool'
	/******/
	| 'void'
	| '*'
	| identifier
	| '(' decl_spec+ ')'
	;
	
type_qualifier
	: 'const'
	| 'volatile'
	| 'restrict'
	;

lhs
	: location?
	('*'? var_id
	| var_id '->' var_id
	| var_id '[' var_id | integer_decimal ']'
	)
	;
	
rhs
	: location? (
		var_id
		| var_id '->' var_id
		| var_id '[' var_id | integer_decimal ']'
		| '&' var_id
		| '*' var_id
		| integer_decimal 'B'? /* Not sure what the 'B' represents.  Seems to always be '0B'. */
		| integer_hex
		| string_literal
		| '&' string_literal '[' integer ']'
		| '&' var_id '[' integer ']'
	)
	;

var_id
	: synthetic_id
			{
			string id($n0.start_loc.s, $n0.end-$n0.start_loc.s);
			cout << "Var ID: " << id << endl; 
			}
	| identifier
	;
	
synthetic_label_id
	: '<' synthetic_id '>'
	;

/// Terminals.

linenumber: "[0-9]+" $term -1;
integer: "[0-9]+";
integer_decimal: "(-|+)?[1-9][0-9]*" | '0';
integer_hex: "0x[0-9A-Fa-f]+";
column: "[0-9]+" $term -1;
synthetic_id: "[a-zA-Z_][a-zA-Z_0-9_]*\.[0-9]+";
identifier: "[a-zA-Z_][a-zA-Z0-9_]*";
path: "[.\\/a-zA-Z0-9_\-]+" $term -2;
string_literal: "\"([^\"\\]|\\[^])*\"";
