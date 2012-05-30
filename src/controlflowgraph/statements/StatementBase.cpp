/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include <string>
#include <utility>
#include <boost/regex.hpp>

#include "StatementBase.h"
#include "If.h"
#include "Switch.h"
#include "FunctionCallUnresolved.h"
#include "../../Location.h"

StatementBase::StatementBase(const Location &location) : m_location(location)
{
}

StatementBase::StatementBase(const StatementBase& orig) : Vertex(orig), m_location(orig.m_location)
{
	// Do a deep copy of the Location object.
}

StatementBase::~StatementBase()
{
}

void StatementBase::SetOwningFunction(Function *owning_function)
{
	m_owning_function = owning_function;
}

Function* StatementBase::GetOwningFunction() const
{
	return m_owning_function;
}

void StatementBase::OutEdges(StatementBase::out_edge_iterator* ibegin,
		StatementBase::out_edge_iterator* iend)
{
	*ibegin = boost::make_transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator >(m_out_edges.begin());
	*iend = boost::make_transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator >(m_out_edges.end());
}

void StatementBase::InEdges(StatementBase::in_edge_iterator* ibegin,
		StatementBase::in_edge_iterator* iend)
{
	*ibegin = boost::make_transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator >(m_in_edges.begin());
	*iend = boost::make_transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator >(m_in_edges.end());
}

std::string StatementBase::EscapeifyForUseInDotLabel(const std::string & str)
{
	static const boost::regex expr("(\\\"|\\\\n)");
	std::string retval;
	retval = boost::regex_replace(str, expr, "\\\\$1",  boost::match_default | boost::format_perl);
	return retval;
}


