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

#include "WriteGraphvizDotFileVisitor.h"

#include <ostream>

#include "../edges/edge_types.h"
#include "../Function.h"

WriteGraphvizDotFileVisitor::WriteGraphvizDotFileVisitor(ControlFlowGraph &g, std::ostream& out)
	: ControlFlowGraphVisitorBase(g), m_out_stream(out)
{

}

WriteGraphvizDotFileVisitor::~WriteGraphvizDotFileVisitor()
{
	// TODO Auto-generated destructor stub
}

vertex_return_value_t WriteGraphvizDotFileVisitor::start_vertex(
		ControlFlowGraph::vertex_descriptor u)
{
	// The first vertex will be an Entry vertex.  Get the name of the Function it belongs to.
	Function *f = u->GetOwningFunction();
	std::string name = f->GetIdentifier();

	// Now stream out the dot graph definition info.
	m_out_stream <<
"\
digraph G {\n\
graph [clusterrank=local colorscheme=svg]\n\
subgraph cluster0 {\n\
label = \"" << name << "\";\n\
labeljust = \"l\";\n\
node [shape=rectangle fontname=\"Helvetica\"]\n\
edge [style=solid]\n\
{ rank = source; 0; }\n\
{ rank = sink; 1; }\n\
" << std::endl;

	return vertex_return_value_t::ok;
}

edge_return_value_t WriteGraphvizDotFileVisitor::examine_edge(
		ControlFlowGraph::edge_descriptor u)
{
	// If this edge is a...
	if(u->IsType<CFGEdgeTypeFunctionCall>() // Function call...
			|| u->IsType<CFGEdgeTypeReturn>() // Function call return...
			|| u->Target()->IsType<Entry>()  // Any type of edge entering the function's Entry vertex...
			|| u->Source()->IsType<Exit>())  // Any type of edge leaving the function's Exit vertex...
	{
		// Ignore this edge.
		return edge_return_value_t::terminate_branch;
	}

	// Otherwise keep going.
	return edge_return_value_t::ok;
}

edge_return_value_t WriteGraphvizDotFileVisitor::tree_edge(
		ControlFlowGraph::edge_descriptor u)
{
	// Stream out the edge attributes.
	StreamOutEdgeAttributes(u);

	return edge_return_value_t::ok;
}

edge_return_value_t WriteGraphvizDotFileVisitor::back_edge(
		ControlFlowGraph::edge_descriptor u)
{
	// For back edges, we stream out the edge attributes as well.
	StreamOutEdgeAttributes(u);

	// Don't try to follow back edges.
	return edge_return_value_t::terminate_branch;
}

edge_return_value_t WriteGraphvizDotFileVisitor::forward_or_cross_edge(
		ControlFlowGraph::edge_descriptor u)
{
	// For forward or cross edges, we stream out the edge attributes as well.
	StreamOutEdgeAttributes(u);

	// Don't try to follow forward or cross edges.
	return edge_return_value_t::terminate_branch;
}

vertex_return_value_t WriteGraphvizDotFileVisitor::discover_vertex(
		ControlFlowGraph::vertex_descriptor u)
{
	// Stream out this vertex.
	m_out_stream << u->GetIndex() << " ";
	m_out_stream << "[label=\"";

	if(true /** @todo Make this switchable. */)
	{
		m_out_stream << u->GetIndex() << " ";
	}

	// Insert the core text of the vertex.
	m_out_stream << u->GetStatementTextDOT();

	if(true /** @todo Make this switchable. */)
	{
		// Insert only the line number and column.
		m_out_stream << "\\n" << u->GetLocation().asLineColumn();
	}
	m_out_stream  << "\", color=" << u->GetDotSVGColor();
	m_out_stream << ", shape=" << u->GetShapeTextDOT();
	m_out_stream << "];" << std::endl;

	return vertex_return_value_t::ok;
}

void WriteGraphvizDotFileVisitor::StreamOutEdgeAttributes(
		ControlFlowGraph::edge_descriptor u)
{
	m_out_stream << u->Source()->GetIndex() << "->" << u->Target()->GetIndex() << " ";
	m_out_stream << "[";
	m_out_stream << "label=\"" << u->GetDotLabel() << "\"";
	m_out_stream << ", color=" << u->GetDotSVGColor();
	m_out_stream << ", style=" << u->GetDotStyle();
	m_out_stream << "];" << std::endl;
}
