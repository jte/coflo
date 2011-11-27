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
#include <boost/utility.hpp>

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
	graph_property_writer(std::string function_name) : m_function_name(function_name) {};

	void operator()(std::ostream& out) const
	{
		out << "graph [clusterrank=local colorscheme=svg]" << std::endl;
		out << "subgraph cluster0 {" << std::endl;
		out << "label = \"" << m_function_name << "\";" << std::endl;
		out << "labeljust = \"l\";" << std::endl;
		out << "node [shape=rectangle fontname=\"Helvetica\"]" << std::endl;
		out << "edge [style=solid]" << std::endl;
	}

	std::string m_function_name;
};

/**
 * Class for a vertex property writer, for use with write_graphviz().
 */
class cfg_vertex_property_writer
{
public:
	cfg_vertex_property_writer(T_CFG _g) :
			m_graph(_g)
	{
	}

	void operator()(std::ostream& out, const T_CFG_VERTEX_DESC& v)
	{
		if (m_graph[v].m_statement != NULL)
		{
			out << "[label=\"";
			out << v << " " << m_graph[v].m_statement->GetStatementTextDOT();
			out << "\\n" << m_graph[v].m_statement->GetLocation() << "\"";
			out << ", color=" << m_graph[v].m_statement->GetDotSVGColor();
			out << ", shape=" << m_graph[v].m_statement->GetShapeTextDOT();
			out << "]";
		}
		else
		{
			out << "[label=\"NULL STMNT\"]";
		}
	}
private:

	/// The graph whose vertices we're writing the properties of.
	T_CFG& m_graph;
};

/**
 * Class for an edge property writer, for use with write_graphviz().
 */
class cfg_edge_property_writer
{
public:
	cfg_edge_property_writer(T_CFG _g) :
			m_graph(_g)
	{
	}
	void operator()(std::ostream& out, const T_CFG_EDGE_DESC& e)
	{
		// Set the edge attributes.
		out << "[";
		out << "label=\"" << m_graph[e].m_edge_type->GetDotLabel() << "\"";
		out << ", color=" << m_graph[e].m_edge_type->GetDotSVGColor();
		out << ", style=" << m_graph[e].m_edge_type->GetDotStyle();
		out << "]";
	}
	;
private:

	/// The graph whose edges we're writing the properties of.
	T_CFG& m_graph;
};


/**
 * The primary control flow graph class.
 * @todo Well, it will be.  At the moment too much functionality is implemented in terms of the Boost Graph Library's
 *       free functions acting on the underlying T_CFG.
 */
class ControlFlowGraph : boost::noncopyable
{
public:
	ControlFlowGraph();
	ControlFlowGraph(const ControlFlowGraph& other);
	~ControlFlowGraph();


	/**
	 * This returns a reference to the underlying T_CFG object.
	 *
	 * @deprecated Ultimately, the goal is to make exposing this unnecessary.  Avoid using this interface if
	 * at all possible.
	 *
	 * @return A reference to the underlying T_CFG (Boost Graph Library adjacency_list) object.
	 */
	const T_CFG& GetConstT_CFG() const { return m_cfg; };

	T_CFG& GetT_CFG() { return m_cfg; };

	/// @name Graph construction helpers
	//@{

	void FixupBackEdges(Function *f);

	void InsertMergeNodes(Function *f);

	void SplitCriticalEdges(Function *f);

	void StructureCompoundConditionals(Function *f);

	//@}

	/// @name Debugging helper functions
	//@{
	void PrintOutEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	void PrintInEdgeTypes(T_CFG_VERTEX_DESC vdesc);
	//@}

	T_CFG_VERTEX_DESC AddVertex(StatementBase * statement, Function *containing_function);
	T_CFG_EDGE_DESC AddEdge(const T_CFG_VERTEX_DESC &source, const T_CFG_VERTEX_DESC &target, CFGEdgeTypeBase *edge_type);

	/// @name Edge attribute accessors.
	//@{

	/**
	 * Return the source vertex of edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return The source vertex of @a e.
	 */
	T_CFG_VERTEX_DESC Source(T_CFG_EDGE_DESC e) { return boost::source(e, m_cfg); };

	/**
	 * Return the target vertex of edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return The target vertex of @a e.
	 */
	T_CFG_VERTEX_DESC Target(T_CFG_EDGE_DESC e) { return boost::target(e, m_cfg); };

	/**
	 * Returns a pointer to the CFGEdgeTypeBase-derived edge type class associated with edge @a e.
	 *
	 * @param e The graph edge to examine.
	 * @return Pointer to the CFGEdgeTypeBase-derived edge type class associated with edge @a e.
	 */
	CFGEdgeTypeBase* GetEdgeTypePtr(T_CFG_EDGE_DESC e) { return m_cfg[e].m_edge_type; };

	//@}

	StatementBase* GetStatementPtr(T_CFG_VERTEX_DESC v) { return m_cfg[v].m_statement; };

	/**
	 * Return the in degree of vertex @a v.
	 *
	 * @param v
	 * @return The in degree of @v.
	 */
	long InDegree(T_CFG_VERTEX_DESC v) { return boost::in_degree(v, m_cfg); };


private:

	/// @name Edge manipulation routines.
	//@{

	/**
	 * Add an edge between the given source and target vertices.
	 *
	 * @param source Source vertex descriptor.
	 * @param target Target vertex descriptor.
	 */
	void AddEdge(const T_CFG_VERTEX_DESC &source, const T_CFG_VERTEX_DESC &target);
	void RemoveEdge(const T_CFG_EDGE_DESC &e);
	void ChangeEdgeTarget(T_CFG_EDGE_DESC &e, const T_CFG_VERTEX_DESC &target);
	void ChangeEdgeSource(T_CFG_EDGE_DESC &e, const T_CFG_VERTEX_DESC &source);

	//@}

	/// The Boost Graph Library graph we'll use for our underlying graph implementation.
	T_CFG m_cfg;
};

//@}


/// @name Other headers in this library.
//@{
#include "visitors/ControlFlowGraphVisitorBase.h"
#include "topological_visit_kahn.h"
//@}

#endif	/* CONTROLFLOWGRAPH_H */
