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
 * @file DParser grammar for the CoFlo C parser.
 * 
 * Run this file through make_dparser to generate the parser.
 *
 * @bug As of make_dparser 1.29, apostrophes in at least C-style comments cause the generated code to break
 * (i.e. things like ${scope} don't get expanded.).
 */

/**
 * Internal info:
 * 
 * Reduction structs look like this:
 *   D_Reduction d_reduction_0_coflo_c_parser = {1, 0, NULL, NULL, 0, 0, 0, 0, -1, 0, NULL};
 *
 * Fields are:
 *  - nelements
 *  - symbol
 *  - speculative_code
 *  - final_code
 *  - op_assoc
 *  - rule_assoc
 *  - op_priority
 *  - rule_priority
 *  - action_index
 *  - npass_code
 *  - ptr to pass_code
 *
 */



/// C++ prefix code.
{

#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include "coflo_c_parser.h"
// Include the helper macros and dparse.h.
#define D_ParseNode_Globals coflo_c_parser_ParseNode_Globals
#define D_ParseNode_User coflo_c_parser_ParseNode_User
#define D_UserSym CoFloCParserSymbol
#include "parser_grammar_helper.h"

#include "Location.h"
#include "controlflowgraph/statements/statements.h"
#include "controlflowgraph/statements/ParseHelpers.h"

const char *g_predefined_typedefs[] = {
	"__builtin_va_list",
	0
};

} // End of C++ prefix code.


${declare set_op_priority_from_rule}
${declare longest_match}

/**
 * The start symbol.
 */
translation_unit
	: new_global_scope translation_unit_component+
	    [
    		/* Return to the previous scope we were in before we entered the one created by the new_global_scope rule. */
    		${scope} = enter_D_Scope(${scope}, $n.scope);
    	]
		{
			// Commit the top-level scope.
			${scope} = commit_D_Scope(${scope});

			std::cout << "Top-level scope: " << std::endl;
			$g->m_the_parser->PrintScope(${scope});
		}
	;

new_global_scope:
	[
		${scope} = new_D_Scope(${scope});

		char *s;
		int i = 0;
		s = const_cast<char*>(g_predefined_typedefs[i]);
		while(s != 0)
		{
			std::cout << "PRELOADING TYPEDEFS: " << s << std::endl; 
			D_Sym *sym = NEW_D_SYM(${scope}, s, 0);
			sym->user.m_is_typename = true;
			sym->user.m_namespace_membership = E_OTHER_IDENTIFIER;
			i++;
			s = const_cast<char*>(g_predefined_typedefs[i]);
		}
	]
	;

//whitespace: (WHITESPACE | COMMENT_CPP | COMMENT_C)+;

translation_unit_component
	: preprocessor_element
	{
		std::cout << "Preproc Element: " << M_TO_STR($n0) << std::endl;
	}
	| function_definition
		{
			std::cout << "Found function definition: \n" << *($0.m_ast_node) << std::endl;
			if(0) {
			std::cout << "Found function definition: " << M_TO_STR($n0) << std::endl;
			/* Find the declarator. */
			D_ParseNode *decl;
   			D_ParseNode *return_type = d_find_in_tree(&$n0, ${nterm decl_specs});
    		decl = d_find_in_tree(&$n0, ${nterm declarator});
    		if(decl)
    		{
    			/* Found the declarator. Now find the identifier that is the name of the function. */
    			D_ParseNode *n = d_find_in_tree(decl, ${nterm identifier});
    			if(n)
    			{
    				/* Found the identifier, add it to the scope as a new "other" identifier. */
    				std::cout << "FOUND FUNCTION DEFINITION: \n"
    				 	<< "  IDENTIFIER:  " << std::string(n->start_loc.s, n->end) << "\n"
    				 	<< "  RETURN TYPE: " << std::string(return_type->start_loc.s, return_type->end) << std::endl; 
    				D_Sym *s = NEW_D_SYM(${scope}, n->start_loc.s, n->end);
    				s->user.m_is_typename = false;
    				s->user.m_namespace_membership = E_OTHER_IDENTIFIER;
    			}
    		}
    		else
    		{
    			std::cout << "ERROR: CAN'T FIND DECLARATOR" << std::endl;
    		}
    		}
		}
	| declaration
		{
			std::cout << "Found declaration: \n" << *($0.m_ast_node) << std::endl;
			if(0) {
			if(d_find_in_tree(&$n0, ${nterm function_declarator}))
    		{
    			/* There was a function declarator in the declaration tree.
    			 Find the identifier that was declared to be the new function. */
    			D_ParseNode *n = d_find_in_tree(&$n0, ${nterm identifier});
    			if(n)
    			{
    				/* Found the identifier, add it to the scope as a new typename. */
    				std::cout << "FOUND FUNCTION DECL: \n"
    				 << "  IDENTIFIER: " << std::string(n->start_loc.s, n->end) << "\n"
    				 << "  LOCATION: " << Location(n->start_loc) << std::endl; 
    				//D_Sym *s = NEW_D_SYM(${scope}, n->start_loc.s, n->end);
    				//s->user.m_is_typename = true;
    				//s->user.m_namespace_membership = E_OTHER_IDENTIFIER;
    			}
    		}
    		else
    		{
    			std::cout << "Found declaration: " << *($0.m_ast_node) << std::endl;
    		}
    		}
		}
    ;

/**
 * State for handling the creation of new scopes during the speculative phase.
 */
new_scope: 
	[
		${scope} = new_D_Scope(${scope});
	]
	;
	
function_definition
	/* This is ANSI */
    : function_definition_prefix compound_statement
    	{
    		M_PROPAGATE_AST_NODE($$, $0);
    		$$ += $1;
    	}
    /* This is K&R */
    | function_definition_prefix declaration+ compound_statement
    ;
	
function_definition_prefix
	: decl_specs declarator
		{
			M_PROPAGATE_AST_NODE($$, $0);
			$$ += $1;
		}
	;
	
declaration
    : decl_specs init_declarator_list? extension_gcc_asm? extension_gcc_function_attribute_specifier* ';'
    	[
    		/* Check if this was a typedef declaration. */
    		if(d_find_in_tree(&$n0, ${string typedef}))
    		{
    			/* There was a typedef in the declspecs. Find the identifier that was declared to be a new type. */
    			D_ParseNode *n = d_find_in_tree(&$n1, ${nterm identifier});
    			if(n)
    			{
    				/* Found the identifier, add it to the scope as a new typename. */
    				//std::cout << "SPEC: ADDING NEW TYPEDEF TO SCOPE: " << std::string(n->start_loc.s, n->end) << std::endl; 
    				D_Sym *s = NEW_D_SYM(${scope}, n->start_loc.s, n->end);
    				s->user.m_is_typename = true;
    				s->user.m_namespace_membership = E_OTHER_IDENTIFIER;
    			}
    		}
    	]
    	{
    		${scope} = commit_D_Scope(${scope});
    		//print_scope(${scope});
    		
    		//if(M_SS($$, decl_specs)->m_storage_class_specifier_node != NULL)
    		//{
	    		/* Look at the decl_specs. */
	    		//std::cout << "DECL_SPECS: Storage Class Spec= " << *(M_SS($$, decl_specs)->m_storage_class_specifier_node) << std::endl;
	    	//}
    		
    		/* For each declarator in the init_declarator_list, determine a type for it.
    		The type consists of info from the decl_specs and possibly some info from the init_declarator_list. */

    		/* Check if this was a typedef declaration. */
    		ASTNodeList nodelist = $0.m_ast_node->GetAllChildrenOfType<M_AST_NODE_CLASSNAME(TYPEDEF)>();
    		if(!nodelist.empty())
    		{
    			/* It was. */
    			$$ = M_NEW_AST_NODE_I(decl_typedef);
				// Append the decl_specs.
				$$ += $0;
				/* Declare all the typenames in the init_declarator_list. */
    			M_FOREACH_CHILD(i, $n1)
    			{
    				D_ParseNode *pn = d_get_child(&$n1, i);
    				if(pn != NULL && pn->user.m_ast_node != NULL)
    				{
    					$$ += pn->user;
    				}
    			}
    		}
    		else if(true /* VARDECL */)
    		{
    			$$ = M_NEW_AST_NODE_I(decl_var);
    			// Add the decl spec children.
    			M_APPEND_ALL_CHILD_ASTS($$, $n0);
    		}

       		//std::cout << *($$.m_ast_node) << std::endl;
       	}
    ;

/**
 * Declaration specifiers.
 * For the associated entity, these specify its:
 * - linkage
 * - storage duration
 * - part of its type information
 * The rest of the type information exists in the declarators.
 */
decl_specs
	: decl_spec+
		{
    		// Append all the declaration specifiers.
    		$$ = M_NEW_AST_NODE_I(decl_specs);
    		M_APPEND_ALL_CHILD_ASTS($$, $n0);
    	}
	;
	
decl_spec
	: storage_class_specifier { M_PROPAGATE_AST_NODE($$, $0); }
	| type_specifier { M_PROPAGATE_AST_NODE($$, $0); }
	| type_qualifier { M_PROPAGATE_AST_NODE($$, $0); }
	| function_specifier { M_PROPAGATE_AST_NODE($$, $0); }
	;

/* Per the C99 draft spec 6.7.1, at most one storage_class_specifier may be given in the decl_specs in a declaration */
storage_class_specifier
    : TYPEDEF
		{
    		$$ = M_NEW_AST_NODE(TYPEDEF, $n0);
    	}
	| EXTERN
		{
    		$$ = M_NEW_AST_LEAF_NODE_ENUM(storage_class_specifier, EXTERN, $n0);
		}
	| STATIC
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(storage_class_specifier, STATIC, $n0);
		}
	| AUTO
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(storage_class_specifier, AUTO, $n0);
		}
	| REGISTER
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(storage_class_specifier, REGISTER, $n0);
		}
    ;

type_specifier
    : built_in_type { $$ = $0; }
    | struct_or_union_specifier { $$ = $0; }
    | enum_specifier { $$ = $0; }
    | extension_specifier { $$ = M_NEW_AST_LEAF_NODE_ENUM(todo, TODO, $n0); }
    | typedef_name { $$ = $0; }
    | extension_gcc_typeof { $$ = M_NEW_AST_LEAF_NODE_ENUM(todo, TODO, $n0); }
    ;
    
built_in_type
	: VOID { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, VOID, $n0); }
	| CHAR { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, CHAR, $n0); }
	| SHORT { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, SHORT, $n0); }
	| INT { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, INT, $n0); }
	| LONG { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, LONG, $n0); }
	| FLOAT { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, FLOAT, $n0); }
	| DOUBLE { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, DOUBLE, $n0); }
	| SIGNED { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, SIGNED, $n0); }
	| UNSIGNED { $$ = M_NEW_AST_LEAF_NODE_ENUM(built_in_type, UNSIGNED, $n0); }
    ;

struct_or_union
	: STRUCT
		{
			$$ = M_NEW_AST_NODE(struct_specifier, $n0);
		}
	| UNION
		{
			$$ = M_NEW_AST_NODE(union_specifier, $n0);
		}
	;

struct_or_union_specifier
	: struct_or_union identifier? struct_declaration_block
		{
			std::cout << "Found struct or union declaration at " << Location($n0.start_loc) << std::endl;
			$$ = $0;
			$$ += $2;
			// Note that the optional identifier is appended last.
			M_APPEND_OPTIONAL_CHILD_AST($$, $n1);
		}
	| struct_or_union identifier
		{
			std::cout << "Found struct type specifier at " << M_LOC_OUT($n0) << std::endl;
			$$ = $0;
			$$ += $1;
		}
    ;

struct_declaration_block
	: '{' new_scope struct_declaration+ '}'
		[
			/* Return to the previous scope we were in before we entered the one created by the new_scope rule. */
		 	${scope} = enter_D_Scope(${scope}, $n0.scope);
		]
		{
			${scope} = commit_D_Scope(${scope});
			
			$$ = M_NEW_AST_NODE_I(nil);
			M_APPEND_ALL_CHILD_ASTS($$, $n2); 
		}
    ;
    
struct_declaration
	: (type_specifier | type_qualifier)+ struct_declarator (',' struct_declarator)* ';'
		{
			$$ = M_NEW_AST_NODE_I(nil);
			ASTNode_nil *list = M_NEW_AST_NODE_I(nil);
			M_APPEND_ALL_CHILD_ASTS(*list, $n0);
			$$ += list;
			ASTNode_nil *list2 = M_NEW_AST_NODE_I(nil);
			*list2 += $1.m_ast_node;
			//M_APPEND_AST_LIST(*list2, $1, $n2);
			M_APPEND_PARENTHESIZED_AST_LIST(*list2, $n2);
			$$ += list2;
		}
	| anonymous_struct_or_union_struct_declaration ';'
		{
			$$ = $0;
		}
	;

struct_declarator
	: declarator { M_PROPAGATE_AST_NODE($$, $0); }
	| declarator? bitfield_colon constant_expression
		{
			$$ = $1;
			$$ += $2;
			M_APPEND_OPTIONAL_CHILD_AST($$, $n0);
		}
    ;

/**
 * Extension supported by GCC and others.
 */
anonymous_struct_or_union_struct_declaration
	: GCC_EXTENSION? struct_or_union_specifier
		{
			/// @todo?
			$$ = $1;
		}
    ;

enum_specifier
	: ENUM identifier? ('{' enumerator (',' enumerator)* ','? '}')?
    	{
			$$ = M_NEW_AST_NODE(enum_specifier, $n0);
    	}
    ;

enumerator
	: identifier ('=' constant_expression )?
    ;
    
type_qualifier
    : CONST	{ $$ = M_NEW_AST_LEAF_NODE_ENUM(type_qualifier, CONST, $n0); }
    | RESTRICT { $$ = M_NEW_AST_LEAF_NODE_ENUM(type_qualifier, RESTRICT, $n0); }
    | VOLATILE { $$ = M_NEW_AST_LEAF_NODE_ENUM(type_qualifier, VOLATILE, $n0); }
    ;

function_specifier
	: INLINE { $$ = M_NEW_AST_LEAF_NODE_ENUM(function_specifier, INLINE, $n0); }
	| extension_function_specifier { $$ = M_NEW_AST_LEAF_NODE_ENUM(todo, TODO, $n0); }
    ;
    

    
init_declarator_list
    : init_declarator (',' init_declarator)*
    	{
    		$$ = M_NEW_AST_NODE_I(nil);
     		M_APPEND_AST_LIST($$, $0, $n1);
    	}
	;
    
init_declarator
    : declarator ('=' initializer)?
    	{
   			$$ = $0;
   			M_APPEND_PARENTHESIZED_AST_LIST($$, $n1);
    	}
	;
	
declarator
    : pointer? direct_declarator
    	[
    		$$.m_decltype = $1.m_decltype;
    	]
    	{
    		/* (pointer-to)? */
    		$$ = M_NEW_AST_NODE_I(declarator);
    		M_APPEND_OPTIONAL_CHILD_AST($$, $n0);
    		$$ += $1;
    	}
    ;

pointer
	: POINTER_TO (POINTER_TO | type_qualifier)*
		{
			$$ = $0;
			M_APPEND_ALL_CHILD_ASTS($$, $n1);
		}
    ;
    
direct_declarator
	/* This is the single identifier being declared by this declarator. */
    : extension_gcc_attribute? identifier
    	[
    		$$.m_decltype = E_DECLTYPE_UNKNOWN;
    	]
    	{
    		/* We found an identifier that is being declared. */
    		$$ = M_NEW_AST_LEAF_NODE_ID($n1);
    	}
    /* extension_gcc_attribute is for function pointer declarations. */
    | '(' extension_gcc_attribute? declarator ')'
    	[
    		$$.m_decltype = $2.m_decltype;
    	]
    	{
    		/** @todo Possibly Pointer-to-function */
    		$$ = $2;
    	}
    | array_declarator
    	{
    		M_PROPAGATE_AST_NODE($$,$0);
    	}
    | direct_declarator '('  identifier (',' identifier)* ')'
    | function_declarator
    	[
    		$$.m_decltype = $0.m_decltype;
    	]
    	{
    		M_PROPAGATE_AST_NODE($$,$0);
    	}
    ;
    
array_declarator
	/* Array declarator, unspecified size (==incomplete type). */
	: direct_declarator '[' ']'
    /* (C99) Array declarator, variable length array of unspecified size. In function prototypes only. */
    | direct_declarator '[' '*' ']'
    | direct_declarator '[' STATIC? type_qualifier+ (STATIC? assignment_expression | '*')? ']'
    | direct_declarator '[' assignment_expression ']'
	;
	
function_declarator
	: direct_declarator '(' parameter_type_list ')'
    	[
    		$$.m_decltype = E_DECLTYPE_FUNCTION;
    	]
    	{
    		$$ = M_NEW_AST_NODE(decl_func, $n1);
    		$$ += $0;
    		// Append the AST tree describing the parameter types.
    		$$ += $2; //M_APPEND_ALL_CHILD_ASTS($$, $n2);    		
    	}
    | direct_declarator '(' ')'
    	[
    		$$.m_decltype = E_DECLTYPE_FUNCTION;
    	]
    	{
    		$$ = M_NEW_AST_NODE(decl_func, $n1);
    		$$ += $0;
    	}
	;

parameter_type_list
    : parameter_decl (',' parameter_decl)* (',' '...')?
    	{
    		$$ = M_NEW_AST_NODE_I(nil);
     		M_APPEND_AST_LIST($$, $0, $n1);
     		/// @todo
    	}
	;

parameter_decl
    : decl_specs (declarator | abstract_declarator)? extension_gcc_attribute?
    	{
    		$$ = M_NEW_AST_NODE_I(nil);
    		/// @todo
    	}
	;

type_name
	: (type_specifier | type_qualifier)+ abstract_declarator?
//		{
//			std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXX" << *($0.m_ast_node) << std::endl;
//		}
    ;

abstract_declarator
	: pointer? direct_abstract_declarator?
    ;

direct_abstract_declarator
	: ('(' abstract_declarator ')'
	| '(' parameter_type_list ')'
	| '(' ')'
	| '[' assignment_expression ']'
	| '[' '*' ']'
	| '[' ']' )+
	;
	
initializer
	: assignment_expression { M_PROPAGATE_AST_NODE($$, $0); }
	| '{' initializer (',' initializer)* ','? '}'
		{
			$$ = M_NEW_AST_NODE_I(nil);
			M_APPEND_AST_LIST($$, $1, $n2);
		}
	;
	
_
	: { $$ = M_NEW_AST_NODE_I(nil); }
	;

//
// Subgrammar includes.
//

// Include the preprocessor subgrammar.
include "coflo_c_parser_preprocessor.include.g"

// C statements subgrammar.
include "coflo_c_parser_statements.include.g"

// Include the expressions subgrammar.
include "coflo_c_parser_expressions.include.g"

// Include the terminal definitions.
include "coflo_c_parser_terminals.include.g"
    
// Include the extensions subgrammar.
include "coflo_c_parser_extensions.include.g"

