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
	: '++' $unary_op_left 1099
	| '--' $unary_op_left 1099
	;

/**
 * Note: precedence does not appear to propagate up more than one level. E.g., putting these
 * in separate productions and collecting them under this one ends up behaving as if all the
 * operators have equal precedence.
 */
basic_binary_operator
	: /*('.'|'->') $binary_op_left 1099
	|*/ ('*'|'/'|'%') $binary_op_left 1096
	| ('+'|'-') $binary_op_left 1095
	| ('<<'|'>>') $binary_op_left 1094
	| ('<'|'>'|'<='|'>=') $binary_op_left 1093
	| ('==' | '!=') $binary_op_left 1092
	| '&' $binary_op_left 1091
	| '^' $binary_op_left 1090
	| '|' $binary_op_left 1089
	| '&&' $binary_op_left 1088
	| '||' $binary_op_left 1087
	//| ('='|'+='|'-='|'*='|'/='|'%='|'<<='|'>>='|'&='|'^='|'|=') $binary_op_right 1085
	;

comma_operator
	: ',' $binary_op_left 1083
	;
	
unary_prefix_inc_dec_operator
	: '++' $unary_op_right 1098
	| '--' $unary_op_right 1098 
	;

unary_prefix_operator
	/* Unary prefix plus/minus, logical/bitwise NOT, dereference, address-of. */
	: ('+'|'-'|'!'|'~'|'*'|'&') $unary_op_right 1098
	;

unary_postfix_function_call_operator
	: '(' argument_expression_list? ')' $unary_op_left 1099
	;
	
unary_postfix_array_subscript_operator
	: '[' expression ']' $unary_op_left 1099
	; 

prefix_ops
	: '(' type_name ')' $unary_op_right 1098
	| ('+'|'-'|'!'|'~'|'*'|'&') $unary_op_right 1098
	| '++' $unary_op_right 1098
	| '--' $unary_op_right 1098
	;
	
postfix_ops
	: '(' argument_expression_list? ')' $unary_op_left 1099
	| '[' expression ']' $unary_op_left 1099
	| '++' $unary_op_left 1099
	| '--' $unary_op_left 1099
	;

bitfield_colon
	: ':' $binary_op_right 1085
	;
	
primary_expression
	: LITERAL_INTEGER
		{
			$$.m_str = new M_TO_STR($n0);
		}
	| LITERAL_HEX
		{
			$$.m_str = new M_TO_STR($n0);
		}
	| LITERAL_OCTAL
		{
			$$.m_str = new M_TO_STR($n0);
		}
	| LITERAL_FLOATING_POINT
		{
			$$.m_str = new M_TO_STR($n0);
		}
	| concatenated_literal_strings
		{
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	| LITERAL_CHAR
		{
			$$.m_str = new M_TO_STR($n0);
		}
	| identifier
		{
			$$.m_str = new M_TO_STR($n0);
		}
	/* Parenthesized expression. */
	| '(' expression ')' $left 1101
		{
			M_PROPAGATE_PTR($1,$$,m_str);
			*($$.m_str) = "P(" + *($$.m_str) + ")"; 
		}
	;

concatenated_literal_strings
	: LITERAL_STRING+
		{
			$$.m_str = new M_TO_STR($n0);
		}
	;

postfix_expression
	: primary_expression
		{
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	/* Array subscripting. */
	| postfix_expression unary_postfix_array_subscript_operator $unary_left 1099
		{
			$$.m_str = new std::string;
			*($$.m_str) = *($0.m_str) + "[" + M_TO_STR($n1) + "]";
		}
	/* Function call */
	| postfix_expression unary_postfix_function_call_operator $unary_left 1099
		{
			$$.m_str = new std::string("FC");
		}
	| postfix_expression binary_operator_member_access identifier $unary_left 1099
		{
			$$.m_str = new std::string;
			*($$.m_str) = "(" + *($0.m_str) + M_TO_STR($n1) + M_TO_STR($n2) + ")"; 
			
		}
	| postfix_expression unary_postfix_inc_dec_operator $unary_left 1099
		{
			$$.m_str = new std::string("UPF");
		}
	/* Compound literals. */
	| '(' type_name ')' '{' initializer (',' initializer)* ','? '}' $unary_left 1099
		{
			$$.m_str = new std::string("M1");
		}
	;

argument_expression_list
	: assignment_expression (',' assignment_expression)*
	;

unary_expression
	: postfix_expression
		{
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	| unary_prefix_inc_dec_operator unary_expression $unary_right 1098
		{
			$$.m_str = new std::string;
			*($$.m_str) = "(" + M_TO_STR($n0) + *($1.m_str) + ")";
		}
	| unary_prefix_operator cast_expression $unary_right 1098
		{
			$$.m_str = new std::string("M1");
		}
	| SIZEOF unary_expression $unary_op_right 1098
		{
			$$.m_str = new std::string("SIZEOF ");
			*($$.m_str) += *($1.m_str);
		}
	| SIZEOF '(' type_name ')' $unary_op_right 1098
		{
			$$.m_str = new std::string("SIZEOF ");
			*($$.m_str) += "TN1"; //*($2.m_str);
		}
	;

cast_expression
	: unary_expression
		{
			if($0.m_str != NULL)
			{
				M_PROPAGATE_PTR($0,$$,m_str);
			}
			else
			{
				$$.m_str = new std::string("M1");
			}
		}
	| '(' type_name ')' cast_expression $unary_op_right 1098
		{
			$$.m_str = new std::string("CAST");
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
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	| binary_expression basic_binary_operator binary_expression
		{
			$$.m_str = new std::string;
			*($$.m_str) = "(" + *($0.m_str) + ") " + M_TO_STR($n1) + " (" + *($2.m_str) + ")";
			std::cout << "BINARY EXPRESSION: (" << *($$.m_str) << ")" << std::endl;  
		}
	/* Experimental *************************/
//	| binary_expression '?' expression ':' binary_expression $right 1086
//		{
//			$$.m_str = new std::string;
//			*($$.m_str) = "(" + *($0.m_str) + " ? " + *($2.m_str) + " : " + *($4.m_str) + ")";
//			std::cout << "TERNARY EXPRESSION: (" << *($$.m_str) << ")" << std::endl;
//		}
	/*| binary_expression unary_postfix_inc_dec_operator
		{
			$$.m_str = new std::string;
			*($$.m_str) = "(" + *($0.m_str) + M_TO_STR($n1) + ")";
			std::cout << "UNARY POSTFIX EXPRESSION: " << *($$.m_str) << std::endl;
		}*/
//	| unary_expression assignment_operator assignment_expression
//		{
//			std::cout << "ASSIGNMENT: " << M_TO_STR($n0) << ", op=\"" << M_TO_STR($n1) << "\", " << M_TO_STR($n2) << std::endl;
//			M_PROPAGATE_PTR($0,$$,m_str);
//		}
//	| binary_expression postfix_ops
//		{
//			$$.m_str = new std::string("M1");
//		}
//	| prefix_ops binary_expression
//		{
//			$$.m_str = new std::string("M1");
//		}
//	| SIZEOF binary_expression
//		{
//			$$.m_str = new std::string("SIZEOF ");
//			*($$.m_str) += *($1.m_str);
//		}
//	| SIZEOF '(' type_name ')'
//		{
//			$$.m_str = new std::string("SIZEOF ");
//			*($$.m_str) += "TN1"; //*($2.m_str);
//		}
//	| '(' type_name ')' '{' initializer (',' initializer)* ','? '}' $left 1099
//		{
//			$$.m_str = new std::string("M1");
//		}
	| extension_gcc_statements_within_expressions
		{
			$$.m_str = new std::string("M1");
		}
	;

conditional_expression
	: binary_expression
		{
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	| binary_expression '?' expression ':' conditional_expression $right 1086
//		[
//			std::cout << "TERNARY: " << M_TO_STR($n0) << ", " << M_TO_STR($n2) << "," << M_TO_STR($n4) << std::endl;
//		]
		{
			$$.m_str = new std::string;
			*($$.m_str) = "(" + *($0.m_str) + "), (" + M_TO_STR($n2) + "), (" + *($4.m_str) + ")";
			std::cout << "TERNARY EXPRESSION: (" << *($$.m_str) << ")" << std::endl;
		}
	;

assignment_expression
	: /*binary_expression*/ conditional_expression
		{
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	| unary_expression assignment_operator assignment_expression
		{
			std::cout << "ASSIGNMENT: " << M_TO_STR($n0) << ", op=\"" << M_TO_STR($n1) << "\", " << M_TO_STR($n2) << std::endl;
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	;
	
assignment_operator
	: ('='|'+='|'-='|'*='|'/='|'%='|'<<='|'>>='|'&='|'^='|'|=') $binary_op_right 1085
	;

expression
	: assignment_expression (comma_operator assignment_expression)*
		{
			M_PROPAGATE_PTR($0,$$,m_str);
			//$$.m_str = new std::string("EX");
		}
	;
	
constant_expression
	: conditional_expression
		{
			M_PROPAGATE_PTR($0,$$,m_str);
		}
	;

/**
 * Extensions
 */
 
extension_gcc_statements_within_expressions
	: GCC_EXTENSION? '(' compound_statement ')'
	;
