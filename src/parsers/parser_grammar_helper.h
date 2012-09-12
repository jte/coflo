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

#ifndef PARSER_GRAMMAR_HELPER_H
#define PARSER_GRAMMAR_HELPER_H

#if !defined(D_ParseNode_Globals) || !defined(D_ParseNode_User)
#error "Both D_ParseNode_Globals and D_ParseNode_User must be defined before including this header."
#endif


#include <fstream>
// For strdup.
#include <cstring>
#include <string>
#include <set>

#include <dparse.h>

// Redefine D_PN with C++ casts to avoid warnings.
#undef D_PN
#define D_PN(_x, _o) (reinterpret_cast<D_ParseNode*>(static_cast<char*>(_x) + _o))

/**
 * Macro which creates a std::string from the pointers pointing to the text buffer for production @a the_n of a rule.
 *
 * @param the_n  The system parse node state of the rule node (e.g. "$n3").
 */
#define M_TO_STR(the_n) std::string(the_n.start_loc.s, the_n.end-the_n.start_loc.s)

/**
 * Macro which converts a production's matched string to an int.
 *
 * @param the_n  The system parse node state of the rule node (e.g. "$n3").
 */
#define M_TO_INT(the_n) atoi(M_TO_STR(the_n).c_str())

/**
 * Macro which converts a production's matched string to an double.
 *
 * @param the_n  The system parse node state of the rule node (e.g. "$n3").
 */
#define M_TO_DOUBLE(the_n) strtod(M_TO_STR(the_n).c_str(), NULL)

/**
 * Macro for inserting a parser location out to an ostream.
 */
#define M_LOC_OUT(the_n) std::string(the_n.start_loc.pathname) << ":" << the_n.start_loc.line << ":" << the_n.start_loc.col << ":"

/**
 * Macro which propagates the pointer @a field_name from parse node state @a from to parse node state @a to.
 *
 * @post @a to is assigned and assumes ownership of the pointer previously in @a field_name.
 * @post @a from is assigned NULL.
 */
#define M_PROPAGATE_PTR(from, to, field_name) do { to.field_name = from.field_name; from.field_name = NULL; } while(0)

/**
 * Macro for looping over all immediate children of @a parent_sys_node.
 *
 * @param child_index_name The variable name to use for the loop index.
 * @param parent_sys_node  The parent D_ParseNode () whose children we want to iterate over.
 */
#define M_FOREACH_CHILD(child_index_name, parent_sys_node) \
	for(int num_children = d_get_number_of_children(&parent_sys_node), child_index_name = 0; child_index_name != num_children; ++child_index_name )

#endif // PARSER_GRAMMAR_HELPER_H
