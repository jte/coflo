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
  * @file Grammar for C statements.
  */
  
statement
	: expression_statement { M_PROPAGATE_AST_NODE($$, $0); }
	| compound_statement { M_PROPAGATE_AST_NODE($$, $0); }
	| labeled_statement { M_PROPAGATE_AST_NODE($$, $0); }
	| selection_statement { M_PROPAGATE_AST_NODE($$, $0); }
	| iteration_statement { M_PROPAGATE_AST_NODE($$, $0); }
	| jump_statement { M_PROPAGATE_AST_NODE($$, $0); }
	;

labeled_statement
	: identifier ':' statement
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(labeled_statement, LABEL, $n1);
			$$ += $0;
			$$ += $2;
		}
	| CASE constant_expression ':' statement
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(labeled_statement, CASE, $n0);
			$$ += $1;
			$$ += $3;
		}
	| DEFAULT ':' statement
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(labeled_statement, DEFAULT, $n1);
			$$ += $2;
		}
	;

compound_statement
    : '{' new_scope block_item* '}'
    	[
    		/* Return to the previous scope we were in before we entered the one created by the new_scope rule. */
    		${scope} = enter_D_Scope(${scope}, $n0.scope);
    	]
    	{
    		//std::cout << "Compound statement pre-commit scope: " << std::endl;
			//$g->m_the_parser->PrintScope(${scope});
			/* Commit any changes to the symbol table discovered by the speculative parsing tree. */
    		${scope} = commit_D_Scope(${scope});
    	}
    ;
    
block_item
	: declaration { M_PROPAGATE_AST_NODE($$, $0); }
	| statement { M_PROPAGATE_AST_NODE($$, $0); }
	;

expression_statement
	: expression ';'
	| ';'
	;

selection_statement
	: IF '(' expression ')' statement //$right 100
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(statement, IF, $n0);
			$$ += $2;
			$$ += $4;
		}
	| IF '(' expression ')' statement ELSE statement //$right 101
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(statement, IF, $n0);
			$$ += $2;
			$$ += $4;
			$$ += $6;
		}
	| SWITCH '(' expression ')' statement
		{
			$$ = M_NEW_AST_LEAF_NODE_ENUM(statement, SWITCH, $n0);
			$$ += $2;
			$$ += $4;
		}
	; 

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' new_scope (declaration | expression_statement) expression_statement expression? ')' statement
	    [
    		/* Return to the previous scope we were in before we entered the one created by the new_scope rule. */
    		${scope} = enter_D_Scope(${scope}, $n0.scope);
    	]
   		{
			/* Commit any changes to the symbol table discovered by the speculative parsing tree. */
    		${scope} = commit_D_Scope(${scope});
    	}
	;

jump_statement
	: GOTO identifier ';'
	| CONTINUE ';'
	| BREAK ';'
	| KEYWORD_RETURN expression? ';'
	;
