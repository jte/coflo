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
 * @file
 * C Preprocessor grammar.
 */


preprocessor_element
	: PP_INCLUDE '<' PP_FILESPEC '>'
	| PP_INCLUDE LITERAL_STRING
	| pp_ifline
	| pp_macro_def
	| PRAGMA_HASH "[^\n]*"
	/* Parse line markers generated by GCC. */
	| "#[ ][0-9]+" LITERAL_STRING (LITERAL_INTEGER)*
	;
	
pp_macro_def
	: PP_DEFINE ID pp_macro_text?
	| PP_DEFINE ID '(' ID? (',' ID)* ')' pp_macro_text?
	;

pp_macro_text
	: '(' pp_macro_text ')'
	| ID
	| pp_macro_text basic_binary_operator pp_macro_text
	;

pp_ifline
	: PP_IF pp_const_expr
	| PP_IFDEF ID
	| PP_IFNDEF ID
	| PP_ELSE
	| PP_ENDIF
	;
	
pp_const_expr
	: expression
	| PP_DEFINED '('? ID ')'? ( ( '&&' | '||') PP_DEFINED '('? ID ')'?)
	;