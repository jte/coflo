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

#include "ParserBaseClass.h"

#include <dparse.h>

#include <fstream>
// For strdup.
#include <cstring>
#include <iostream>
#include <string>

extern int d_verbose_level;
extern int d_debug_level;

ParserBaseClass::ParserBaseClass(const std::string &filename, D_ParserTables *t)
{
	m_filename = filename;
	m_parser_tables = t;
	m_root_parsenode = NULL;
	m_parser = NULL;
	m_verbose_level = 0;
	m_debug_level = 0;
};

ParserBaseClass::~ParserBaseClass()
{
	if(m_parser != NULL)
	{
		if(m_root_parsenode != NULL)
		{
			free_D_ParseTreeBelow(m_parser, m_root_parsenode);
		}
		free_D_Parser(m_parser);
	}
};

D_ParseNode* ParserBaseClass::Parse()
{
	m_parser = new_D_Parser(m_parser_tables, GetUserDataSize());

	// Configure the created parser.
	d_verbose_level = m_verbose_level;
	d_debug_level = m_debug_level;
	m_parser->commit_actions_interval = 0;
	m_parser->error_recovery = 1;
	m_parser->save_parse_tree = 1;
	//parser->free_node_fn = coflo_c_parser_FreeNodeFn;
	// Set up the initial location.
	m_parser->loc.pathname = strdup(m_filename.c_str());
	m_parser->loc.line = 1;
	m_parser->loc.col = 0;
	// Create the initial globals.
	m_parser->initial_globals = InitGlobalData();
	//m_parser->initial_scope = new_D_Scope(NULL);

	// Try to open the file whose name we were passed.
	std::ifstream input_file(m_filename.c_str(), std::ifstream::in);

	// Check if we were able to open the file.
	if(input_file.fail())
	{
		std::cerr << "ERROR: Couldn't open file \"" << m_filename << "\"" << std::endl;
	}

	// Read the file's contents into a buffer.
	m_buffer.clear();
	char previous_char = '\n';

	while (input_file.good())     // loop while extraction from file is possible
	{
		char c;
		c = input_file.get();       // get character from file
		if (input_file.good())
		{
			if(c == '\r')
			{
				// Strip CR's.
				continue;
			}
			else
			{
				m_buffer += c;
			}
			previous_char = c;
		}
	}

	// Close file
	input_file.close();

	// Parse the file.
	m_root_parsenode = dparse(m_parser, const_cast<char*>(m_buffer.c_str()), m_buffer.length());

	if(m_root_parsenode == NULL)
	{
	    fprintf(stderr, "fatal error, '%s' line %d\n", m_parser->loc.pathname, m_parser->loc.line);
	}

	// Return the parse tree.
	return m_root_parsenode;
}

long ParserBaseClass::GetSyntaxErrorCount() const
{
	return m_parser->syntax_errors;
}

void* ParserBaseClass::GetGlobalInfoAsVoidPtr()
{
	return m_root_parsenode->globals;
}

void* ParserBaseClass::GetUserInfoAsVoidPtr(D_ParseNode *tree)
{
	return static_cast<void*>(&(tree->user));
}

void ParserBaseClass::SetVerboseLevel(int val)
{
	m_verbose_level = val;
}

void ParserBaseClass::SetDebugLevel(int val)
{
	m_debug_level = val;
}

void ParserBaseClass::PrintScope(D_Scope* scope)
{
	std::cout << "SCOPE AT ADDRESS " << scope << ":" << std::endl;
	std::cout << "  Owned by user?: " << scope->owned_by_user << std::endl;
	std::cout << "  Kind: " << scope->kind << std::endl;
	std::cout << "  LL?: " << ((scope->ll) ? "yes" : "no") << std::endl;
	std::cout << "  HASH?: " << ((scope->hash) ? "yes" : "no") << std::endl;

	D_Sym *start = 0;
	// Get the first symbol in the given scope.
	D_Sym *sym = next_D_Sym_in_Scope(&scope, &start);
	do
	{
		std::cout << std::string(sym->name, sym->len) << std::endl;
	} while (sym = next_D_Sym_in_Scope(&scope, &sym), sym != 0);
}

void ParserBaseClass::PrintSubtree(const D_ParseNode* start_node)
{

}

bool was_given(const D_ParseNode* nonterminal)
{
	return (d_get_number_of_children(const_cast<D_ParseNode*>(nonterminal)) != 0);
}
