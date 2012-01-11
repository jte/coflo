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

#include "Analyzer.h"

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include "RuleBase.h"
#include "RuleReachability.h"

#include "Program.h"
#include "Function.h"

/// Regex for function-calls-function constraint "f1() -x f2()".
static const boost::regex f_fxf_regex("([[:alpha:]_][[:alnum:]_]+)\\(\\) -x ([[:alpha:]_][[:alnum:]_]+)\\(\\)");

Analyzer::Analyzer() { }

Analyzer::Analyzer(const Analyzer& orig) { }

Analyzer::~Analyzer() { }

void Analyzer::AddConstraints(const std::vector< std::string > &vector_of_constraint_strings)
{
	boost::cmatch capture_results;
	
	std::cerr << "INFO: Adding constraints..." << std::endl;
	BOOST_FOREACH(std::string s, vector_of_constraint_strings)
	{
		// Parse the next constraint.
		if(boost::regex_match(s.c_str(), capture_results, f_fxf_regex))
		{
			Function *f1, *f2;
			
			// Look up the functions.
			f1 = m_program->LookupFunction(capture_results[1]);
			f2 = m_program->LookupFunction(capture_results[2]);
			
			if(f1 == NULL)
			{
				std::cerr << "ERROR: Can't find function: " << capture_results[1] << std::endl;
			}
			else if(f2 == NULL)
			{
				std::cerr << "ERROR: Can't find function: " << capture_results[2] << std::endl;
			}
			else
			{
				std::cerr << "INFO: Adding constraint: "
						<< f1->GetIdentifier() << "() -x "
						<< f2->GetIdentifier() << "()" << std::endl;
				RuleReachability *rule = new RuleReachability(*m_program->GetControlFlowGraphPtr(), f1, f2);
				m_constraints.push_back(rule);
			}
		}
		else
		{
			std::cerr << "ERROR: Can't parse constraint: " << s << std::endl;
		}
	}
}

bool Analyzer::Analyze()
{
	// Run all analyses.
	BOOST_FOREACH(RuleBase *constraint, m_constraints)
	{
		constraint->RunRule();
	}

	/// @todo Make this real.
	return true;
}

