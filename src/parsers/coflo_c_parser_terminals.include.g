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
  * @file Terminals for the C parser.
  */
 
/**
 * All keywords of the C language proper.
 * Used for rejecting ID (identifier) parses that could otherwise pick them up, making parsing more difficult
 * or impossible.
 */
keywords_c
	: CASE
	| CONTINUE
	| DO
	| IF
	| ELSE
	| SWITCH
	| BREAK
	| DEFAULT
	| WHILE
	| FOR
	| GOTO
	| KEYWORD_RETURN
	| SIZEOF
	| CONST
	| RESTRICT
	| VOLATILE
	| STRUCT
	| UNION
	| ENUM
	| keywords_c_builtin_types
	| keywords_c_storage_class_specifiers
	| keywords_c_function_specifiers
	| keywords_c_extensions_gcc
	;
	
keywords_c_builtin_types
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	;
	
keywords_c_storage_class_specifiers
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;
	
keywords_c_function_specifiers
	: INLINE
	| INLINE_ANSI_C89
	;
	
keywords_c_extensions_gcc
	: GCC_ATTRIBUTE
	| GCC_EXTENSION
	| ASM
	;
	
/**
 * Lexer definitions.
 */
 
/// Identifier/typename disambiguation.

identifier
	: ID
		[
			/* Look up the string in the typename symbol table and reject it if its has been declared as a typedef. */
			D_Sym *s = find_D_Sym(${scope}, $n0.start_loc.s, $n0.end);
			if (s && s->user.m_is_typename)
			{
				${reject};
			}
		]
		{
			$$ = M_NEW_AST_NODE(identifier, $n0);
		}
	;
	
typedef_name
	: ID
		[
			/* Look up the string in the typename symbol table and reject the parse if it has not been declared. */
			D_Sym *s = find_D_Sym(${scope}, $n0.start_loc.s, $n0.end);
			if (!s || !s->user.m_is_typename)
			{
				${reject};
			}
		]
		{
			$$ = M_NEW_AST_NODE(typedef_name, $n0);
		}
	;

/// Definitions for preprocessor constructs.

PP_INCLUDE: "#[ ]*include" $term 0;
PP_IF: "#[ ]*if" $term 0;
PP_ELSE: "#[ ]*else" $term 0;
PP_IFDEF: "#[ ]*ifdef" $term 0;
PP_IFNDEF: "#[ ]*ifndef" $term 0;
PP_ENDIF: "#[ ]*endif" $term 0;
PP_DEFINE: "#[ ]*define" $term 0;
PP_DEFINED: 'defined' $term 0;

PP_FILESPEC: "[^>]*" $term 0;

/// Lexer definitions for the language proper.

/**
 * Keywords.
 */
GCC_ATTRIBUTE: '__attribute__';
GCC_EXTENSION: '__extension__';
GCC_TYPEOF: ('typeof'|'__typeof__');
PRAGMA_UNDERSCORE: '_Pragma';
PRAGMA_HASH: "#[ ]*pragma";
ASM: '__asm__';

IF: 'if';
ELSE: 'else';

SWITCH: 'switch';
CASE: 'case';
BREAK: 'break';
DEFAULT: 'default';

// Iteration statement keywords.
DO: 'do';
WHILE: 'while';
FOR: 'for';

// Jump statement keywords.
GOTO: 'goto';
CONTINUE: 'continue';
KEYWORD_RETURN: 'return';

SIZEOF: 'sizeof' $unary_op_right 1098;

// Type qualifier keywords
CONST: 'const';
RESTRICT: 'restrict';
VOLATILE: 'volatile';

// Storage class specifier keywords.
TYPEDEF: 'typedef';
EXTERN: 'extern';
STATIC: 'static';
AUTO: 'auto';
REGISTER: 'register';

// Built-in types keywords
VOID: 'void';
CHAR: 'char';
SHORT: 'short';
INT: 'int';
LONG: 'long';
FLOAT: 'float';
DOUBLE: 'double';
SIGNED: 'signed';
UNSIGNED: 'unsigned';

// struct/union/enum
STRUCT: 'struct';
UNION: 'union';
ENUM: 'enum';

// function specifiers
// Available in C99, C11 
INLINE: 'inline';
// GCC alternative for ANSI C89 code.
// See <http://gcc.gnu.org/onlinedocs/gcc/Alternate-Keywords.html>
INLINE_ANSI_C89: "__inline(__)?";

/**
 * Identifiers have priority lower than keywords.
 */
ID
	: /*keywords_c
		[
			//std::cout << "Rejecting ID match to keyword: " << M_TO_STR($n0) << "\n";
			${reject};
		]
	|*/ "[a-zA-Z_][a-zA-Z_0-9]*" $term -3
    ;
    
LITERAL_INTEGER
	: "[\-+]?[1-9][0-9]*[uUlL]*|0" $term -2
    ;
    
LITERAL_HEX
	: "0[xX][0-9a-fA-F]+[uUlL]*" $term -2
	;
	
LITERAL_OCTAL
	: "0[0-9]+" $term -2
	;

LITERAL_FLOATING_POINT
	: "[\-+]?[0-9]+\.[0-9]+(e[\-+][0-9]+)?" $term -1
	;

LITERAL_STRING : "L?\"(\\.|[^\"])*\""
	;

LITERAL_CHAR
	: "'[^']*'"
    ;
    
POINTER_TO
	: '*' { $$ = M_NEW_AST_LEAF_NODE_ENUM(unary_operator, POINTER_TO, $n0); }
	;
    
WHITESPACE: "[ \t\r\n]+";

COMMENT_CPP: "\/\/[^\n]*" '\n';

COMMENT_C: '/*' ("[^*]" | '*'+ "[^*\/]")* '*'+ '/';
    
UNPARSABLE_TEXT : ".*" $term -99999
 	;
