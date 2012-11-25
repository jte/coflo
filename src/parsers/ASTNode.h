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

/** @file */

#ifndef ASTNODE_H_
#define ASTNODE_H_

#include <string>
#include <vector>
#include <iosfwd>

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/foreach.hpp>

#include "Token.h"
#include "Location.h"
#include "safe_enum.h"


struct ast_exception_base: virtual std::exception, virtual boost::exception { };
struct ast_exception_assignment_to_self: virtual ast_exception_base {};

class ASTNodeBase;

class ASTNodeList
{
public:
	typedef std::vector<const ASTNodeBase*> underlying_storage_t;
	typedef underlying_storage_t::iterator iterator;
	typedef underlying_storage_t::const_iterator const_iterator;

public:
	ASTNodeList();
	~ASTNodeList();

	void Append(const ASTNodeBase *n);
	void Append(const ASTNodeList *nl);

	std::ostream& InsertionHelper(std::ostream &os) const;

	/**
	 * Returns the empty/not-empty status of this ASTNodeList.
	 *
	 * @return true if empty, false if not empty.
	 */
	bool empty() const { return m_ast_node_list.empty(); };

	const_iterator begin() const { return m_ast_node_list.begin(); };
	const_iterator end() const { return m_ast_node_list.end(); };
	iterator begin() { return m_ast_node_list.begin(); };
	iterator end() { return m_ast_node_list.end(); };

private:

	/// The list of ASTNodes.
	underlying_storage_t m_ast_node_list;
};

inline std::ostream &operator<<(std::ostream &os, const ASTNodeList& n)
{
	return n.InsertionHelper(os);
};

/**
 * The abstract base class from which all AST nodes will indirectly derive through ASTNodeBase.  We do this
 * in order to avoid issues with partial assignments between different derived classes which implement operator=().
 * @see Meyers, Scott. (1996). Item 33: Make non-leaf classes abstract. In: More Effective C++.
 */
class AbstractASTNodeBase
{
public:
	AbstractASTNodeBase() {};
	/**
	 * Destructor is made pure virtual to:
	 * 	1. Ensure the base class is in fact abstract
	 * 	2. Force derived classes to implement the destructor.
	 *
	 * Note that it is in fact implemented for this class as well, since derived classes's destructors will always
	 * call the base classes' destructors.  Implementing a pure virtual member function in the class which declares it
	 * pure virtual may seem unusual, but it is legitimate.  This is deliberate and necessary here.
	 */
	virtual ~AbstractASTNodeBase() = 0;

protected:
	/**
	 * Assignment operator is made protected to prevent assignments between derived types which are incompatible.
	 *
	 * @param rhs
	 * @return
	 */
	AbstractASTNodeBase& operator=(const AbstractASTNodeBase &rhs);
};


/**
 *
 */
class ASTNodeBase : public AbstractASTNodeBase
{
public:
	ASTNodeBase();

	/**
	 * Create an ASTNodeBase from the Token @a token.
	 * @param token
	 */
	ASTNodeBase(const Token &token);

	ASTNodeBase(const std::string &s, const Location& loc) : m_token(s,loc) {};

	/**
	 * Copy constructor.
	 * @param other
	 */
	ASTNodeBase(const ASTNodeBase &other) :
		m_token(other.m_token),
		m_children(other.m_children)
	{
		if(&other == this)
		{
			// Trying to copy ourself.
			BOOST_THROW_EXCEPTION(ast_exception_assignment_to_self());
		}
	};
	virtual ~ASTNodeBase();

	virtual std::string asString(int indent_level = 0) const;
	virtual std::string asStringTree(int indent_level = 0) const;

	void AddChild(const ASTNodeBase *child) { m_children.Append(child); };

	virtual std::ostream& InsertionHelper(std::ostream &os) const;

	/**
	 * @name Operators.
	 */
	///@{

	ASTNodeBase& operator=(const ASTNodeBase &rhs)
	{
		if(&rhs == this)
		{
			// Trying to assign to ourself.
			BOOST_THROW_EXCEPTION(ast_exception_assignment_to_self());
		}

		// Call the assignment operator of our base class.
		AbstractASTNodeBase::operator =(rhs);

		m_token = rhs.m_token;
		m_children = rhs.m_children;

		return *this;
	};

	/**
	 * Append a child node derived from ASTNodeBase.
	 *
	 * @param rhs
	 * @return
	 */
	ASTNodeBase& operator+=(const ASTNodeBase *rhs)
	{
		if(rhs == this)
		{
			// Trying to assign to ourself.
			BOOST_THROW_EXCEPTION(ast_exception_assignment_to_self());
		}

		// Append the child node.
		AddChild(rhs);

		return *this;
	};

	///@}

	template <typename Type>
	bool isType() const { return NULL != dynamic_cast<const Type*>(this); };

	/*
	 * @return
	 */
	template <typename Type>
	ASTNodeList GetAllChildrenOfType()
	{
		ASTNodeList retval;

		BOOST_FOREACH(const ASTNodeBase *c, m_children)
		{
			if(c->isType<Type>())
			{
				retval.Append(c);
			}
		}

		return retval;
	};

private:

	/// @name The input Token from which we created this AST node.
	/// The input token is specified by its location and its text.
	Token m_token;

	/// The list of child AST nodes.
	ASTNodeList m_children;

public:
	virtual std::string GetNodeTypeName() const { return "ASTNodeBase"; };
	virtual ASTNodeBase* GetTypedThis() { return this; };
};

inline std::ostream &operator<<(std::ostream &os, const ASTNodeBase& n)
{
	return n.InsertionHelper(os);
};


struct ASTLeafNode_default_namestruct { static const char *GetName() { return "UNNAMED"; }; };

/**
 * Class template for AST leaf nodes.
 *
 * @tparam T The value type which this leaf node will contain.
 */
template <typename T, typename NameStruct = ASTLeafNode_default_namestruct>
class ASTLeafNode : public ASTNodeBase
{
public:
	/// Member typedef defining the value type of this node.
	typedef T value_type;
	typedef ASTLeafNode<T, NameStruct> this_t;

public:
	ASTLeafNode() {};
	ASTLeafNode(T value, const Token &token) : ASTNodeBase(token) { m_value = value; };
	virtual ~ASTLeafNode() {};

	virtual std::ostream& InsertionHelper(std::ostream &os) const
	{
		os << m_value;
		return os;
	}

	virtual this_t* GetTypedThis() { return this; };
	virtual std::string GetNodeTypeName() const { return NameStruct::GetName(); };

	/// The value of this leaf node.
	value_type m_value;
};

/**
 * Class template for AST non-leaf nodes.
 *
 * @tparam Kind The "kind" type of this node.  This is essentially a subtype for the node,
 * 			e.g. if the node type is an "expression" node, the "kind" is the kind of expression, such as "UNARY" or "BINARY".
 * @tparam T The value type which this leaf node will contain.
 */
template <typename Kind, typename T, typename NameStruct = ASTLeafNode_default_namestruct>
class ASTNode : public ASTNodeBase
{
public:
	typedef ASTNode<Kind, T, NameStruct> this_t;
	typedef Kind kind_t;

private:
	Kind m_kind;

public:
	ASTNode() {};
	ASTNode(const Token &token) : ASTNodeBase(token) {};
	ASTNode(const Location &loc) : ASTNodeBase(loc) {};
	ASTNode(const std::string &s, const Location& loc) : ASTNodeBase(s, loc) {};
	virtual ~ASTNode() {};

	virtual this_t* GetTypedThis() { return this; };
	virtual std::string GetNodeTypeName() const
	{
		return std::string(NameStruct::GetName());
	};

};


/**
 * Macro which takes the production name and creates a suitable name for the associated AST node class.
 */
#define M_AST_NODE_CLASSNAME(root_name) \
		ASTNode_##root_name

/**
 * Macro for declaring new non-leaf AST node types.
 */
#define M_DECLARE_AST_NODE(new_class, base_class) \
	DECLARE_ENUM_CLASS(ASTNode_##new_class##_enum_type); \
	struct ASTNode_name_functor_##new_class { static const char *GetName() { return #new_class ; }; };\
	class M_AST_NODE_CLASSNAME(new_class) : public base_class<ASTNode_##new_class##_enum_type, int, ASTNode_name_functor_##new_class> \
	{ \
	public: \
		ASTNode_##new_class(const Token &token) : base_class(token) {}; \
		virtual ~ASTNode_##new_class() {};\
		virtual std::string asString() const { return base_class::asString() + "<node_type=" + GetNodeTypeName() + ">";  }; \
	};

#define M_DECLARE_DERIVED_AST_NODE(new_class, base_class) \
	class M_AST_NODE_CLASSNAME(new_class) : public M_AST_NODE_CLASSNAME(base_class) \
	{ \
	public: \
		/** Construct from a Token. */ \
		M_AST_NODE_CLASSNAME(new_class)(const Token &token) : M_AST_NODE_CLASSNAME(base_class)(token) {};\
		virtual std::string GetNodeTypeName() const { return std::string( #new_class ); }; \
	};



/**
 * Macro for declaring new ASTNode types with an enumerated value_type.
 *
 * @param enumerators A comma-separated list of the enumerators.
 */
#define M_DECLARE_AST_NODE_ENUM(new_class, value_type, ...) \
	DECLARE_ENUM_CLASS(ASTNode_##new_class##_enum_type, __VA_ARGS__ ); \
	struct ASTNode_name_functor_##new_class { static const char *GetName() { return #new_class ; }; };\
	typedef ASTNode< ASTNode_##new_class##_enum_type, value_type, ASTNode_name_functor_##new_class > ASTNode_##new_class

/**
 * Macro for declaring new enumeration ASTLeafNode<> types.
 */
#define M_DECLARE_AST_LEAF_NODE_ENUM(new_class, ...) \
	DECLARE_ENUM_CLASS(ASTNode_##new_class##_enum_type, __VA_ARGS__); \
	struct ASTNode_name_functor_##new_class { static const char *GetName() { return #new_class ; }; };\
	typedef ASTLeafNode< ASTNode_##new_class##_enum_type, ASTNode_name_functor_##new_class > ASTNode_##new_class

/**
 * Macro for declaring new std::string ASTLeafNode<> types.
 */
#define M_DECLARE_AST_LEAF_NODE_STRING(node_name)\
	struct ASTNode_name_functor_##node_name { static const char *GetName() { return #node_name ; }; };\
	typedef ASTLeafNode< std::string, ASTNode_name_functor_##node_name > ASTNode_##node_name

/**
 * @name Macros for instantiating new instances of the above types.
 */
///@{

/// Identifiers.
#define M_NEW_AST_LEAF_NODE_ID(sys_parse_node) \
	new ASTNode_identifier(Token(sys_parse_node))

/// Generic non-leaf nodes.
#define M_NEW_AST_NODE(node_name, sys_parse_node) \
		new ASTNode_##node_name(sys_parse_node)

/// Create a new imaginary node.
#define M_NEW_AST_NODE_I(node_name) \
		new ASTNode_##node_name(Token(std::string(#node_name)))

/// Create a new imaginary node.
#define M_NEW_AST_NODE_I2(node_name, sys_parse_node) \
		new ASTNode_##node_name(Token(std::string(#node_name)))

/// Nodes with enumerated kinds.
#define M_NEW_AST_NODE_ENUM(node_name, kind, value, sys_parse_node) \
	new ASTNode_##node_name(ASTNode_##node_name::kind_t::kind, Token(sys_parse_node));

/// Generic leaf nodes.
#define M_NEW_AST_LEAF_NODE(node_name, value, sys_parse_node) \
	new ASTNode_##node_name(ASTNode_##node_name::value_type::value, Token(sys_parse_node));

/// Leaf nodes with enumerated values.
#define M_NEW_AST_LEAF_NODE_ENUM(node_name, value, sys_parse_node) \
	new ASTNode_##node_name(ASTNode_##node_name::value_type::value, Token(sys_parse_node));

/// Undifferentiated text leaf nodes.
#define M_NEW_AST_LEAF_NODE_STRING(node_name, sys_parse_node) \
	new ASTNode_##node_name(Token(sys_parse_node));

#define M_NEW_AST_LEAF_NODE_UNEXPECTED_STRING(node_name, sys_parse_node) \
		new M_AST_NODE_CLASSNAME(node_name)(Token(sys_parse_node));

///@}

/**
 * Upcast the given user data type pointer to the given ASTNode type.
 */
#define M_SS(ss, node_name) static_cast<M_AST_NODE_CLASSNAME(node_name)*>((ss).m_ast_node->GetTypedThis())

/**
 * Move the ASTNode specified by @a from to the ASTNode specified by @a to.
 *
 * @pre Parameter @a to does not point to a valid object.
 * @post Parameter @a to = @a from, @a from = NULL.
 */
#define M_PROPAGATE_AST_NODE(to, from) do { to.m_ast_node = from.m_ast_node; from.m_ast_node = NULL; } while(0)

#define M_NEW_AST_NODE_LIST(node_name, sys_node_with_children) \
	new M_AST_NODE_CLASSNAME(node_name)(sys_node_with_children)

/**
 * Move the ASTNode specified by @a from to a child node of the ASTNode specified by @a to.
 */
#define M_APPEND_AST_NODE(to, from) do { to.m_ast_node->AddChild(from.m_ast_node); from.m_ast_node = NULL; } while(0)

#define M_APPEND_ALL_CHILD_ASTS(usr_node_to_append_to, parent_sys_node) \
	do { \
	M_FOREACH_CHILD(i, parent_sys_node) \
    { \
    	/* Get the next child. */ \
    	D_ParseNode *pn = d_get_child(&parent_sys_node, i); \
    	\
		if(pn->user.m_ast_node != NULL) \
		{ \
			usr_node_to_append_to += pn->user.m_ast_node; \
		} \
	} \
	} while(0)

#define M_APPEND_OPTIONAL_CHILD_AST(usr_node_to_append_to, parent_sys_node) M_APPEND_ALL_CHILD_ASTS(usr_node_to_append_to, parent_sys_node)

#define M_APPEND_EVERY_NTH_CHILD_AST(usr_node_to_append_to, parent_sys_node, start, n) \
	do { \
	M_FOREACH_NTH_CHILD(i, parent_sys_node, start, n) \
    { \
    	/* Get the next decl_spec. */ \
    	D_ParseNode *pn = d_get_child(&parent_sys_node, i); \
    	\
		if(pn->user.m_ast_node != NULL) \
		{ \
			usr_node_to_append_to += pn->user.m_ast_node; \
			pn->user.m_ast_node = NULL; \
		} \
		else \
		{ \
			std::cout << "WARNING: NULL child ASTNode* found at index " << i << " of " << num_children << std::endl;\
		} \
	} \
	} while(0)

#define M_APPEND_AST_LIST_EACH(usr_node_to_append_to, first_element_usernode, optional_element_sysnode, which_child) \
	do { \
		/* Append the first element. */ \
		usr_node_to_append_to += first_element_usernode; \
		M_FOREACH_CHILD(i, optional_element_sysnode) \
		{\
			D_ParseNode *pn = d_get_child(&optional_element_sysnode, i);\
			/* Get the second part of this subrule's instance, since the first is the separator. */ \
			D_ParseNode *child_pn = d_get_child(pn, which_child); \
			if(child_pn->user.m_ast_node != NULL) \
			{ \
				usr_node_to_append_to += child_pn->user.m_ast_node; \
				child_pn->user.m_ast_node = NULL; \
			} \
		} \
	} while(0)

/**
 * Append a parenthesized list of symbols (either terminal or non-terminal) to the given DParser user node.
 * The rule should be of the form:
 *   production: symbol1 (separator symbol2)*
 * where:
 *   - symbol1 corresponds to first_element_usernode.
 *   - separator is whatever symbol separates the list elements.  The only restriction is that it is a single symbol.
 *   - symbol2 corresponds to the list of zero or more subsequent symbols which will be appended to the list.
 */
#define M_APPEND_AST_LIST(usr_node_to_append_to, first_element_usernode, optional_element_sysnode) \
	do { \
		/* Append the first element. */ \
		usr_node_to_append_to += first_element_usernode; \
		M_APPEND_PARENTHESIZED_AST_LIST(usr_node_to_append_to, optional_element_sysnode);\
	} while(0)


#define M_APPEND_PARENTHESIZED_AST_LIST(usr_node_to_append_to, optional_element_sysnode) \
		do { \
			M_FOREACH_CHILD(i, optional_element_sysnode) \
			{\
				D_ParseNode *pn = d_get_child(&optional_element_sysnode, i);\
				if(pn == NULL) { M_ERR("PARSENODE=NULL"); continue; } \
				/* Get the second part of this subrule's instance, since the first is the separator. */ \
				D_ParseNode *child_pn = d_get_child(pn, 1); \
				if(child_pn == NULL) { M_ERR("CHILDNODE=NULL"); continue; } \
				if(child_pn->user.m_ast_node != NULL) \
				{ \
					usr_node_to_append_to += child_pn->user.m_ast_node; \
					child_pn->user.m_ast_node = NULL; \
				} \
			} \
		} while(0)

#define M_ERR(msg) do { std::cout << msg << std::endl; } while(0)

#endif /* ASTNODE_H_ */
