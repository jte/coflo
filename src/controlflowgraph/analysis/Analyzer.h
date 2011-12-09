/*
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#ifndef ANALYZER_H
#define	ANALYZER_H

#include <vector>
#include <string>

#include "../ControlFlowGraph.h"

class Program;
class RuleBase;

class Analyzer
{

public:
	Analyzer();
	Analyzer(const Analyzer& orig);
	virtual ~Analyzer();
	
	void AddConstraints(const std::vector< std::string > &vector_of_constraint_strings);
	
	void AttachToProgram(Program *p) { m_program = p; };
	
	bool Analyze();
	
private:

	/// Pointer to the program to analyze.
	Program *m_program;
	
	/// The list of constraints to check m_program against.
	std::vector< RuleBase* > m_constraints;
};

#endif	/* ANALYZER_H */

