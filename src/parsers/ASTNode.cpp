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

std::string ASTNodeBase::asString(int indent_level) const
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
		return asString(indent_level);
	}
	else
	{
		// Start with a paren and the string.
		std::string retval = "(" + asString();
		indent_level++;
		// Now append all the children.
		BOOST_FOREACH(const ASTNodeBase *c, m_children)
		{
			retval += " " + c->asStringTree(indent_level);
		}
		indent_level--;
		retval += ")";

		return retval;
	}
}


std::ostream& ASTNodeBase::InsertionHelper(std::ostream& os) const
{
	if(true /** @todo stream is in "parens AST representation" mode */)
	{
		if(m_children.empty())
		{
			// No children, just insert this node's info.
			os << asString();
		}
		else
		{
			// Stream out this node and all of its children.
			os << "(" << asStringTree() << ")";
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

ASTNodeList::~ASTNodeList()
{
}

void ASTNodeList::Append(const ASTNodeBase *n)
{
	m_ast_node_list.push_back(n);
}

void ASTNodeList::Append(const ASTNodeList *nl)
{
	m_ast_node_list.insert(m_ast_node_list.end(), nl->m_ast_node_list.begin(), nl->m_ast_node_list.end());
}

std::ostream& ASTNodeList::InsertionHelper(std::ostream& os) const
{
	const ASTNodeBase *c;

	if(!m_ast_node_list.empty())
	{
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
