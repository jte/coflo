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

/**
 * Operators.
 * The C++ scope resolution operator has the highest precedence.
 */
cpp_binary_operator_scope_resolution
	: '::' $binary_op_left 1100
	;

binary_operator_member_access
	: ('.'|'->') $binary_op_left 1099
	;

unary_postfix_inc_dec_operator
	: '++' $unary_op_left 1099 { $$ = M_NEW_AST_UNOP(POST_INC, $n0); }
	| '--' $unary_op_left 1099 { $$ = M_NEW_AST_UNOP(POST_DEC, $n0); }
	;
	
sizeof
	: SIZEOF { $$ = M_NEW_AST_UNOP(SIZEOF, $n0); }
	;

/**
 * Note: precedence does not appear to propagate up more than one level. E.g., putting these
 * in separate productions and collecting them under this one ends up behaving as if all the
 * operators have equal precedence.
 */
basic_binary_operator
	: /*('.'|'->') $binary_op_left 1099
	|*/ '*' $binary_op_left 1096 { $$ = M_NEW_AST_BINOP(MUL, $n0); }
	| '/' $binary_op_left 1096 { $$ = M_NEW_AST_BINOP(DIV, $n0); }
	| '%' $binary_op_left 1096 { $$ = M_NEW_AST_BINOP(MOD, $n0); }
	| '+' $binary_op_left 1095 { $$ = M_NEW_AST_BINOP(ADD, $n0); }
	| '-' $binary_op_left 1095 { $$ = M_NEW_AST_BINOP(SUB, $n0); }
	| '<<' $binary_op_left 1094 { $$ = M_NEW_AST_BINOP(LSHIFT, $n0); }
	| '>>' $binary_op_left 1094 { $$ = M_NEW_AST_BINOP(RSHIFT, $n0); }
	| '<' $binary_op_left 1093 { $$ = M_NEW_AST_BINOP(LESSTHAN, $n0); }
	| '>' $binary_op_left 1093 { $$ = M_NEW_AST_BINOP(GREATERTHAN, $n0); }
	| '<=' $binary_op_left 1093 { $$ = M_NEW_AST_BINOP(LESSEQ, $n0); }
	| '>=' $binary_op_left 1093 { $$ = M_NEW_AST_BINOP(GREATEREQ, $n0); }
	| '==' $binary_op_left 1092 { $$ = M_NEW_AST_BINOP(EQUAL, $n0); }
	| '!=' $binary_op_left 1092 { $$ = M_NEW_AST_BINOP(NOT_EQUAL, $n0); }
	| '&' $binary_op_left 1091 { $$ = M_NEW_AST_BINOP(BITWISE_AND, $n0); }
	| '^' $binary_op_left 1090 { $$ = M_NEW_AST_BINOP(BITWISE_XOR, $n0); }
	| '|' $binary_op_left 1089 { $$ = M_NEW_AST_BINOP(BITWISE_OR, $n0); }
	| '&&' $binary_op_left 1088 { $$ = M_NEW_AST_BINOP(LOGICAL_AND, $n0); }
	| '||' $binary_op_left 1087 { $$ = M_NEW_AST_BINOP(LOGICAL_OR, $n0); }
	//| ('='|'+='|'-='|'*='|'/='|'%='|'<<='|'>>='|'&='|'^='|'|=') $binary_op_right 1085
	;

comma_operator
	: ',' $binary_op_left 1083 { $$ = M_NEW_AST_BINOP(COMMA, $n0); }
	;
	
unary_prefix_inc_dec_operator
	: '++' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(PRE_INC, $n0); }
	| '--' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(PRE_DEC, $n0); }
	;

unary_prefix_operator
	/* Unary prefix plus/minus, logical/bitwise NOT, dereference, address-of. */
	: '+' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(UNARY_PLUS, $n0); }
	| '-' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(UNARY_MINUS, $n0); }
	| '!' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(LOGICAL_NOT, $n0); }
	| '~' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(BITWISE_NOT, $n0); }
	| '*' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(DEREFERENCE, $n0); }
	| '&' $unary_op_right 1098 { $$ = M_NEW_AST_UNOP(ADDRESS_OF, $n0); }
	;

unary_postfix_function_call_operator
	: '(' argument_expression_list? ')' $unary_op_left 1099 
		{
			$$ = M_NEW_AST_UNOP(FUNCTION_CALL, $n0);
			/// @todo
		}
	;
	
unary_postfix_array_subscript_operator
	: '[' expression ']' $unary_op_left 1099
		{
			$$ = M_NEW_AST_UNOP(ARRAY_SUBSCRIPT, $n0);
			$$ += $1;
		}
	; 

bitfield_colon
	: ':' $binary_op_right 1085
	;
	
primary_expression
	: LITERAL_INTEGER
		{
			$$ = M_NEW_AST_NODE(literal_integer, $n0);
		}
	| LITERAL_HEX
		{
			$$ = M_NEW_AST_NODE(literal_integer, $n0);
		}
	| LITERAL_OCTAL
		{
			$$ = M_NEW_AST_NODE(literal_integer, $n0);
		}
	| LITERAL_FLOATING_POINT
		{
			$$ = M_NEW_AST_NODE(literal_float, $n0);
		}
	| concatenated_literal_strings
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	| LITERAL_CHAR
		{
			$$ = M_NEW_AST_NODE(literal_character, $n0);
		}
	| identifier
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	/* Parenthesized expression. */
	| '(' expression ')' $left 1101
		{
			M_PROPAGATE_AST_NODE($$, $1); 
		}
	;

concatenated_literal_strings
	: LITERAL_STRING+
		{
			$$ = M_NEW_AST_NODE(literal_string, $n0);
		}
	;

postfix_expression
	: primary_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	/* Array subscripting. */
	| postfix_expression unary_postfix_array_subscript_operator $unary_left 1099
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, UNARY, $n1);
			$$ += $0;
		}
	/* Function call */
	| postfix_expression unary_postfix_function_call_operator $unary_left 1099
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, FUNCTION_CALL, $n1);
			$$ += $0;
		}
	| postfix_expression binary_operator_member_access identifier $unary_left 1099
		{
			$$ = $1;
			$$ += $0;
			$$ += $2;
		}
	| postfix_expression unary_postfix_inc_dec_operator $unary_left 1099
		{
			$$ = $1;
			$$ += $0;
		}
	/* Compound literals. */
	| '(' type_name ')' '{' initializer (',' initializer)* ','? '}' $unary_left 1099
		{
			//$$.m_str = new std::string("M1");
			$$ = $1;
			/// @todo
		}
	;

argument_expression_list
	: assignment_expression (',' assignment_expression)*
		{
			$$ = M_NEW_AST_NODE(nil, $n0);
			M_APPEND_AST_LIST($$, $0, $n1);
		}
	;

unary_expression
	: postfix_expression
		{
			M_PROPAGATE_AST_NODE($$, $0); 
		}
	| unary_prefix_inc_dec_operator unary_expression $unary_right 1098
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, UNARY, $n0);
			$$ += $1;
		}
	| unary_prefix_operator cast_expression $unary_right 1098
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, UNARY, $n0);
			$$ += $1;
		}
	| sizeof unary_expression $unary_op_right 1098
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, SIZEOF_EXPR, $n0);
			$$ += $1;
		}
	| sizeof '(' type_name ')' $unary_op_right 1098
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, SIZEOF_TYPE, $n0);
			$$ += $2;
		}
	;

cast_expression
	: unary_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	| '(' type_name ')' cast_expression $unary_op_right 1098
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, CAST, $n0);
			$$ += $1;
			$$ += $3;
		}
	; 

/**
 * Simple binary expressions.
 * E.g. "a + b * c >> d".
 * This rule covers the C precedence chain from logical-or expressions to multiplicative expressions,
 * with an "exit" to cast_expression.
 */
binary_expression
	: /*primary_expression*/ cast_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	| binary_expression basic_binary_operator binary_expression
		{
			$$ = $1;
			$$ += $0;
			$$ += $2;  
		}
	| extension_gcc_statements_within_expressions
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	;

conditional_expression
	: binary_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	| binary_expression '?' expression ':' conditional_expression $right 1086
		{
			std::cout << "Ternary Start" << std::endl;
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, TERNARY, $n1);
			$$ += $0;
			$$ += $2;
			$$ += $4;
			std::cout << "Ternary End" << std::endl;
		}
	;

assignment_expression
	: /*binary_expression*/ conditional_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	| unary_expression assignment_operator assignment_expression
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, ASSIGNMENT, $n1);
			$$ += $0;
			$$ += $2;
		}
	;
	
assignment_operator
	: '=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '+=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '-=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '*=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '/=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '%=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '<<=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '>>=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '&=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '^=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	| '|=' $binary_op_right 1085 { $$ = M_NEW_AST_ASSIGN(BASIC_ASSIGNMENT, $n0); }
	
	;

expression
	: assignment_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	| assignment_expression (comma_operator assignment_expression)*
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(expression, ASSIGNMENT, $n0);
			M_APPEND_AST_LIST($$, $0, $n1);
		}
	;
	
constant_expression
	: conditional_expression
		{
			M_PROPAGATE_AST_NODE($$, $0);
		}
	;

/**
 * Extensions
 */
 
extension_gcc_statements_within_expressions
	: GCC_EXTENSION? '(' compound_statement ')'
	;
