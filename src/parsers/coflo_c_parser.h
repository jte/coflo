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

#ifndef COFLO_C_PARSER_H
#define COFLO_C_PARSER_H

#include <vector>
#include <set>
#include <string>

#include "ParserBaseClass.h"

#include "Location.h"
#include "../safe_enum.h"
#include "ASTNode.h"

/// Forward declaration of the parse node struct.
struct D_ParseNode;
struct D_Parser;

// Forward declarations for the User data.
class StatementBase;
class TranslationUnit;

/// @name Declare the AST node types we need.
///@{
M_DECLARE_AST_NODE(nil, ASTNode);
M_DECLARE_AST_NODE(list, ASTNode);
M_DECLARE_AST_NODE(translation_unit, ASTNode);
M_DECLARE_AST_NODE(function_definition, ASTNode);
M_DECLARE_AST_LEAF_NODE_ENUM(todo, nil, TODO);
M_DECLARE_AST_LEAF_NODE_ENUM(storage_class_specifier, ASTNodeBase, TYPEDEF, EXTERN, STATIC, AUTO, REGISTER);
M_DECLARE_AST_LEAF_NODE_ENUM(type_qualifier, ASTNodeBase, CONST, RESTRICT, VOLATILE);
M_DECLARE_AST_LEAF_NODE_ENUM(function_specifier, ASTNodeBase, INLINE);
M_DECLARE_AST_LEAF_NODE_ENUM(literal_pointer, ASTNodeBase, LITERAL_POINTER);
M_DECLARE_AST_NODE(identifier, ASTNode);

/// @name Declarations
///@{
M_DECLARE_AST_NODE(declaration, ASTNode);
M_DECLARE_DERIVED_AST_NODE(decl_var, declaration);
M_DECLARE_DERIVED_AST_NODE(decl_typedef, declaration);
///@}

/// @name Types
///@{
M_DECLARE_AST_NODE(type, ASTNode);
M_DECLARE_AST_NODE(type_specifier, ASTNode);
M_DECLARE_DERIVED_AST_NODE(struct_specifier, type_specifier);
M_DECLARE_DERIVED_AST_NODE(union_specifier, type_specifier);
M_DECLARE_DERIVED_AST_NODE(enum_specifier, type_specifier);
M_DECLARE_AST_NODE(typedef_name, ASTNode);
M_DECLARE_DERIVED_AST_NODE_ENUM(built_in_type, type, VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, SIGNED, UNSIGNED);
///@}

M_DECLARE_AST_NODE(op, ASTNode);
M_DECLARE_DERIVED_AST_NODE_ENUM(unary_operator, op, PRE_INC, PRE_DEC, POST_INC, POST_DEC, SIZEOF, ARRAY_SUBSCRIPT,
		UNARY_PLUS, UNARY_MINUS, LOGICAL_NOT, BITWISE_NOT, DEREFERENCE, POINTER_TO, ADDRESS_OF, FUNCTION_CALL);
M_DECLARE_DERIVED_AST_NODE_ENUM(assignment_operator, op,
		BASIC_ASSIGNMENT,
		ADD_ASSIGNMENT,
		SUB_ASSIGNMENT,
		MUL_ASSIGNMENT,
		DIV_ASSIGNMENT,
		MOD_ASSIGNMENT,
		BITAND_ASSIGNMENT,
		BITOR_ASSIGNMENT,
		BITXOR_ASSIGNMENT,
		LSHIFT_ASSIGNMENT,
		RSHIFT_ASSIGNMENT);
M_DECLARE_DERIVED_AST_NODE_ENUM(binary_operator, op,
		MEMBER_ACCESS_DOT, MEMBER_ACCESS_PTR,
		ADD, SUB, MUL, DIV, MOD,
		BITWISE_AND, BITWISE_OR, BITWISE_XOR,
		LOGICAL_AND, LOGICAL_OR,
		LSHIFT, RSHIFT,
		LESSTHAN, GREATERTHAN, LESSEQ, GREATEREQ, EQUAL, NOT_EQUAL,
		COMMA, BITFIELD_COLON);
M_DECLARE_AST_LEAF_NODE_ENUM(expression, UNARY, BINARY, TERNARY, COMMA, ASSIGNMENT, FUNCTION_CALL, CAST, SIZEOF_EXPR, SIZEOF_TYPE);
M_DECLARE_AST_NODE(statement, ASTNode)
M_DECLARE_DERIVED_AST_NODE_ENUM(selection_statement, statement, IF, SWITCH);
M_DECLARE_DERIVED_AST_NODE_ENUM(iteration_statement, statement, WHILE, FOR, DO);
M_DECLARE_DERIVED_AST_NODE_ENUM(compound_statement, statement, COMPOUND);
M_DECLARE_DERIVED_AST_NODE_ENUM(labeled_statement, statement, LABEL, CASE, DEFAULT);
M_DECLARE_DERIVED_AST_NODE_ENUM(jump_statement, statement, GOTO, CONTINUE, BREAK, RETURN);

/**
 * Declaration specifiers.
 * Declaration specifiers consist of:
 * - At most one storage class specifier
 * - At least one type specifier
 *   - Which can occur in any order
 * -
 */
M_DECLARE_AST_NODE(decl_specs, ASTNode);
M_DECLARE_DERIVED_AST_NODE(TYPEDEF, decl_specs);

M_DECLARE_AST_NODE(declarator, ASTNode);
M_DECLARE_DERIVED_AST_NODE_ENUM(declr, declarator, FUNCTION, ARRAY_OF);

M_DECLARE_AST_NODE(pointer, ASTNode);

M_DECLARE_AST_NODE(literals, ASTNode);
M_DECLARE_DERIVED_AST_NODE(literal_integer, literals);
M_DECLARE_DERIVED_AST_NODE(literal_float, literals);
M_DECLARE_DERIVED_AST_NODE(literal_character, literals);
M_DECLARE_DERIVED_AST_NODE(literal_string, literals);

/// @name Constructor helper macros.
///@{

/// Binary operators
#define M_NEW_AST_BINOP(op_enum, sys_parse_node) \
	M_NEW_AST_LEAF_NODE_ENUM(binary_operator, op_enum, sys_parse_node)

/// Unary operators
#define M_NEW_AST_UNOP(op_enum, sys_parse_node) \
	M_NEW_AST_LEAF_NODE_ENUM(unary_operator, op_enum, sys_parse_node)

/// Assignment operators
#define M_NEW_AST_ASSIGN(op_enum, sys_parse_node) \
	M_NEW_AST_LEAF_NODE_ENUM(assignment_operator, op_enum, sys_parse_node)

///@}

struct VariableDeclInfo : public M_AST_NODE_CLASSNAME(declaration)
{
 // m_type;
};

struct FunctionDeclInfo : public M_AST_NODE_CLASSNAME(declaration)
{
	// m_return_type;
	// m_parameter_type_list;
};

///@}

class CoFloCParser;

struct coflo_c_parser_ParseNode_Globals
{
	/// Pointer to the parser instance.  For use by grammar actions.
	CoFloCParser * m_the_parser;

	ASTNodeBase *m_root_node;

	/// Set containing the keywords of the language.
	/// Useful for preventing the return of invalid identifiers.
	//std::set<std::string> m_keyword_set;

	TranslationUnit *m_translation_unit;
	//FunctionInfoList *m_function_info_list;
};

enum DeclaratorType
{
	E_DECLTYPE_UNKNOWN,
	E_DECLTYPE_FUNCTION,
	E_DECLTYPE_STRUCT_OR_UNION,
	E_DECLTYPE_ENUM,
	E_DECLTYPE_VAR
};

/// Type of the object that gets passed through the parse tree.
class coflo_c_parser_ParseNode_User
{
public:
	coflo_c_parser_ParseNode_User() : m_ast_node(NULL) {};
	virtual ~coflo_c_parser_ParseNode_User() {};

	ASTNodeBase *m_ast_node;

	coflo_c_parser_ParseNode_User& operator=(ASTNodeBase *ast_node) { m_ast_node = ast_node; return *this; };
	coflo_c_parser_ParseNode_User& operator+=(ASTNodeBase *ast_node) { *m_ast_node += ast_node; return *this; };
	coflo_c_parser_ParseNode_User& operator+=(coflo_c_parser_ParseNode_User user) { *m_ast_node += user.m_ast_node; return *this; };
};

/**
 * The 4 different C namespaces a given symbol could be in.
 */
enum CoFloCParserNamespace
{
	/// If it's been defined by a typedef.
	E_LABEL_NAME = 1,
	E_STRUCT_UNION_ENUM_TAG = 2,
	E_STRUCT_UNION_MEMBER = 4,
	/// All other identifiers, e.g. typedefs, variables, functions, etc.
	E_OTHER_IDENTIFIER = 8
};

struct CoFloCParserSymbol
{
	/// Set to true to indicate that this symbol is a typedef'ed typename vs. another identifier.
	bool m_is_typename;
	CoFloCParserNamespace m_namespace_membership;
	bool m_is_lvalue;
	bool m_is_modifiable_lvalue;
	bool m_is_rvalue;

	bool isLvalue() const { return m_is_lvalue; };
	bool isModifiableLvalue() const { return m_is_modifiable_lvalue; };
	bool isRvalue() const { return m_is_rvalue; };
};

extern D_ParserTables parser_tables_coflo_c_parser;

class CoFloCParser : public ParserBaseClass
{
public:
	CoFloCParser(const std::string &filename) : ParserBaseClass(filename, &parser_tables_coflo_c_parser) {};
	virtual ~CoFloCParser() {};

	coflo_c_parser_ParseNode_User* GetUserInfo(D_ParseNode *tree)
	{
		return static_cast<coflo_c_parser_ParseNode_User*>(GetUserInfoAsVoidPtr(tree));
	};

	coflo_c_parser_ParseNode_Globals* GetGlobalInfo()
	{
		return static_cast<coflo_c_parser_ParseNode_Globals*>(GetGlobalInfoAsVoidPtr());
	};

private:
	virtual size_t GetUserDataSize() { return sizeof(coflo_c_parser_ParseNode_User); };

	virtual void* InitGlobalData()
	{
		coflo_c_parser_ParseNode_Globals *retval = new coflo_c_parser_ParseNode_Globals;
		retval->m_the_parser = this;
		retval->m_root_node = NULL;

		return retval;
	};

};

#endif // COFLO_C_PARSER_H
