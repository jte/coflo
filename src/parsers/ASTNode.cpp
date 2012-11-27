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

#include "ASTNode.h"

#include <stack>
#include <boost/foreach.hpp>

/*
 * AbstractASTNodeBase
 */

AbstractASTNodeBase::~AbstractASTNodeBase()
{
};

/*
 *  ASTNodeBase
 */

ASTNodeBase::ASTNodeBase() : m_token(std::string("NIL"), Location())
{
}

ASTNodeBase::ASTNodeBase(const Token &token) : m_token(token)
{
}

ASTNodeBase::~ASTNodeBase()
{
}

namespace
{

std::string indent(int level)
{
	std::string retval;

	while(level>0)
	{
		level--;
		retval += " ";
	}

	return retval;
}

}

std::string ASTNodeBase::asString() const
{
	std::string retval;

	retval += m_token.getText();

	return retval;
}

std::string ASTNodeBase::asStringTree(int indent_level) const
{
	if(m_children.empty())
	{
		// No children, just return this ASTNode as a plain string.
		return indent(indent_level) + asString() + "\n";
	}
	else
	{
		// Start with a paren and the string.
		std::string retval = indent(indent_level) + "( " + asString() + "\n";
		indent_level++;
		// Now append all the children.
		BOOST_FOREACH(const ASTNodeBase *c, m_children)
		{
			retval += c->asStringTree(indent_level);
		}
		indent_level--;
		retval += indent(indent_level) + ")\n";

		return retval;
	}
}

ASTNodeList ASTNodeBase::GetAllChildren()
{
	ASTNodeList retval(m_children);
	return retval;
}

ASTNodeList ASTNodeBase::depth_first_search()
{
	std::stack<ASTNodeBase*> node_stack;

	// Prime the search by pushing this node onto the work stack.
	node_stack.push(this);

	while(!node_stack.empty())
	{
		ASTNodeList children;

		// Visit the top node.
		/// @todo

		// Prepare to visit the children by pushing pointers to them on the stack.
		children = node_stack.top()->GetAllChildren();
		node_stack.pop();
		BOOST_REVERSE_FOREACH(ASTNodeBase* p, children)
		{
			node_stack.push(p);
		}
	}
}


std::ostream& ASTNodeBase::InsertionHelper(std::ostream& os, long indent_level) const
{
	if(true /** @todo stream is in "parens AST representation" mode */)
	{
		if(m_children.empty())
		{
			// No children, just insert this node's info.
			os << indent(indent_level) << asString();
		}
		else
		{
			// Stream out this node and all of its children.
			os << indent(indent_level) << "(\n";
			indent_level++;
			os << asStringTree(indent_level);
			indent_level--;
			os << indent(indent_level) << ")\n";
		}
	}

	return os;
}


/*
 * ASTNodeList
 */

ASTNodeList::ASTNodeList()
{
}

ASTNodeList::ASTNodeList(const ASTNodeList &other) : m_ast_node_list(other.m_ast_node_list)
{
}

ASTNodeList::~ASTNodeList()
{
}

void ASTNodeList::Append(ASTNodeBase *n)
{
	m_ast_node_list.push_back(n);
}

void ASTNodeList::Append(const ASTNodeList *nl)
{
	m_ast_node_list.insert(m_ast_node_list.end(), nl->m_ast_node_list.begin(), nl->m_ast_node_list.end());
}

std::ostream& ASTNodeList::InsertionHelper(std::ostream& os, long indent_level) const
{
	const ASTNodeBase *c;

	if(!m_ast_node_list.empty())
	{
		os << "<<<LIST>>>" << std::endl;
		BOOST_FOREACH(c, m_ast_node_list)
		{
			os << " " << *c;
		}
	}
	else
	{
		os << "<<<empty>>>";
	}

	return os;
}
