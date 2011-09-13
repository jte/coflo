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

#ifndef CONTROLFLOWGRAPH_H
#define	CONTROLFLOWGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "statements/statements.h"
#include "edges/CFGEdgeTypeBase.h"

class Function;


/// \name Control Flow Graph definitions.
//@{
	
/// Vertex properties for the CFG graph.
struct CFGVertexProperties
{
	/// The Statement at this vertex of the CFG.
	StatementBase *m_statement;
	
	/// The Function which contains this vertex.
	Function *m_containing_function;
};

/// Edge properties for the CFG graph.
struct CFGEdgeProperties
{
	/// The Edge Type.
	/// May be a FunctionCall, fallthrough, goto, etc.
	CFGEdgeTypeBase *m_edge_type;
};

/// Typedef for the CFG graph.
typedef boost::adjacency_list
		<boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		CFGVertexProperties,
		CFGEdgeProperties
		> T_CFG;

/// Typedef for the vertex_descriptors in the control flow graph.
typedef boost::graph_traits<T_CFG>::vertex_descriptor T_CFG_VERTEX_DESC;

/// Typedef for the edge_descriptors in the control flow graph.
typedef boost::graph_traits<T_CFG>::edge_descriptor T_CFG_EDGE_DESC;

/// Typedef for vertex iterators for the CFG.
typedef boost::graph_traits< T_CFG >::vertex_iterator T_CFG_VERTEX_ITERATOR;

/// Typedef for out-edge iterators for the CFG.
typedef boost::graph_traits< T_CFG >::out_edge_iterator T_CFG_OUT_EDGE_ITERATOR;

/// Typedef for in-edge iterators for the CFG.
typedef boost::graph_traits< T_CFG >::in_edge_iterator T_CFG_IN_EDGE_ITERATOR;

/// Typedef for the type used to represent vertex degrees.
typedef boost::graph_traits< T_CFG >::degree_size_type T_CFG_DEGREE_SIZE_TYPE;

template < typename CFGEdgeType >
boost::tuple<T_CFG_EDGE_DESC, bool> GetFirstOutEdgeOfType(T_CFG_VERTEX_DESC vdesc, const T_CFG &cfg)
{
	boost::graph_traits< T_CFG >::out_edge_iterator eit, eend;
	boost::tuple<T_CFG_EDGE_DESC, bool> retval;
	
	boost::tie(eit, eend) = boost::out_edges(vdesc, cfg);
	for(; eit != eend; eit++)
	{
		if(NULL != dynamic_cast<CFGEdgeType*>(cfg[*eit].m_edge_type))
		{
			// Found it.
			retval = boost::make_tuple(*eit, true);
			return retval;
		}
	}

	// Couldn't find one.
	retval = boost::make_tuple(*eit, false);
	return retval;
}


/// Functor for writing GraphViz dot-compatible info for the function's entire CFG.
struct graph_property_writer
{
	void operator()(std::ostream& out) const
	{
		out << "graph [clusterrank=local colorscheme=svg]" << std::endl;
		out << "node [shape=rectangle fontname=\"Helvetica\"]" << std::endl;
		out << "edge [style=solid]" << std::endl;
	}
};

/**
 * Class for a vertex property writer, for use with write_graphviz().
 */
class cfg_vertex_property_writer
{
public:
	cfg_vertex_property_writer(T_CFG _g) :
			g(_g)
	{
	}

	void operator()(std::ostream& out, const T_CFG_VERTEX_DESC& v)
	{
		if (g[v].m_statement != NULL)
		{
			out << "[label=\"";
			out << g[v].m_statement->GetStatementTextDOT();
			out << "\\n" << g[v].m_statement->GetLocation() << "\"";
			out << ", color=" << g[v].m_statement->GetDotSVGColor();
			out << ", shape=" << g[v].m_statement->GetShapeTextDOT();
			out << "]";
		}
		else
		{
			out << "[label=\"NULL STMNT\"]";
		}
	}
private:

	/// The graph whose vertices we're writing the properties of.
	T_CFG& g;
};

/**
 * Class for an edge property writer, for use with write_graphviz().
 */
class cfg_edge_property_writer
{
public:
	cfg_edge_property_writer(T_CFG _g) :
			g(_g)
	{
	}
	void operator()(std::ostream& out, const T_CFG_EDGE_DESC& e)
	{
		// Set the edge attributes.
		out << "[";
		out << "label=\"" << g[e].m_edge_type->GetDotLabel() << "\"";
		out << ", color=" << g[e].m_edge_type->GetDotSVGColor();
		out << ", style=" << g[e].m_edge_type->GetDotStyle();
		out << "]";
	}
	;
private:

	/// The graph whose edges we're writing the properties of.
	T_CFG& g;
};


/**
 * The primary control flow graph class.
 * @todo Well, it will be.
 */
class ControlFlowGraph
{
public:
	ControlFlowGraph();
	~ControlFlowGraph();

	void PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	void PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc);

	T_CFG& GetT_CFG() { return m_cfg; };


	/// @name Graph construction helpers
	//@{

	void FixupBackEdges(Function *f);

	void InsertMergeNodes(Function *f);

	//@}

private:

	void InitializeControlFlowGraph();
	bool IsDummyVertex(const T_CFG_VERTEX_DESC &v);
	T_CFG_VERTEX_DESC GetDummyVertex();

	/// The Boost Graph Library graph we'll use for our underlying graph implementation.
	T_CFG m_cfg;
};

//@}


/// @name Other headers in this library.
//@{
#include "CFGDFSVisitor.h"
#include "topological_visit_kahn.h"
//@}

#endif	/* CONTROLFLOWGRAPH_H */
