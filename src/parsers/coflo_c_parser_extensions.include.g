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
  *
  * Grammar for various extensions to the C language.  
  */
 
/// @name GCC extensions.
///@{
 
extension_gcc_statements_within_expressions
	: GCC_EXTENSION? '(' compound_statement ')'
		{
			$$ = $2;
		}
	;
	
extension_function_specifier
	: extension_gcc_function_attribute_specifier 
	| PRAGMA_UNDERSCORE '(' LITERAL_STRING ')'
    ;
    
extension_gcc_function_attribute_specifier
	: INLINE_ANSI_C89
	| GCC_ATTRIBUTE '(' '(' extension_gcc_function_attribute (',' extension_gcc_function_attribute)* ')' ')'
    ;
    
extension_gcc_attribute
	: GCC_ATTRIBUTE '(' '(' extension_gcc_function_attribute (',' extension_gcc_function_attribute)* ')' ')'
	;
    
extension_gcc_format_arg
	: '__printf__'
	| '__scanf__'
	| LITERAL_INTEGER
    ;
    
extension_gcc_function_attribute
	: 'alias'
	| '__always_inline__'
	| '__cdecl__'
	| 'deprecated'
	| 'dllimport'
	| '__format__' '(' extension_gcc_format_arg (',' extension_gcc_format_arg)* ')'
	| '__format_arg__' '(' LITERAL_INTEGER ')'
	| '__warning__' '(' LITERAL_STRING ')'
	| 'noreturn'
	| '__noreturn__'
	| 'warn_unused_result'
	| extension_gcc_function_parameter_decl_attribute
    ;
    
extension_gcc_function_parameter_decl_attribute
	: 'unused'
	;

extension_gcc_typeof
	: GCC_TYPEOF '(' type_name | expression ')'
		{
			std::cout << "GCC_TYPEOF" << std::endl;
		}
	;
	
/**
 * This class of specifiers are strictly for extensions which apply to all declarations.
 * If a specifier applies only to a function, it should not go in here.
 */  
extension_specifier
	: GCC_EXTENSION
	;
    
extension_gcc_asm
	: ASM '(' LITERAL_STRING+ ')'
    ;
    
///@}
