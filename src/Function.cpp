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

/** @file */

#include "Function.h"

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <stack>
#include <typeinfo>
#include <cstdlib>

#include <boost/concept/requires.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/unordered_set.hpp>

#include "debug_utils/debug_utils.hpp"

#include "TranslationUnit.h"
#include "SuccessorTypes.h"

#include "controlflowgraph/statements/statements.h"
#include "controlflowgraph/statements/ParseHelpers.h"
#include "controlflowgraph/edges/edge_types.h"
#include "controlflowgraph/ControlFlowGraph.h"
#include "controlflowgraph/visitors/ControlFlowGraphVisitorBase.h"

#include "libexttools/ToolDot.h"

#include "gcc_gimple_parser.h"

/// Property map typedef which allows us to get at the function pointer stored at
/// CFGVertexProperties::m_containing_function in the T_CFG.
typedef boost::property_map<T_CFG, Function* CFGVertexProperties::*>::type T_VERTEX_PROPERTY_MAP;

/// Property map typedef which allows us to get at the edge type pointer stored at
/// CFGEdgeProperties::m_edge_type in the T_CFG.
typedef boost::property_map<T_CFG, CFGEdgeTypeBase* CFGEdgeProperties::*>::type T_EDGE_TYPE_PROPERTY_MAP;

/**
 * Predicate for filtering the CFG for only the vertices of the given function.
 */
struct vertex_filter_predicate
{
	vertex_filter_predicate()
	{
	};
	vertex_filter_predicate(T_VERTEX_PROPERTY_MAP vertex_prop_map,
			Function *parent_function) :
			m_vertex_prop_map(vertex_prop_map),
			m_parent_function(parent_function)
	{
	};
	bool operator()(const T_CFG_VERTEX_DESC& vid) const
	{
		if (m_parent_function == get(m_vertex_prop_map, vid))
		{
			// This vertex belongs to the function we're concerned with.
			return true;
		}
		else
		{
			return false;
		}
	};

	T_VERTEX_PROPERTY_MAP m_vertex_prop_map;
	Function *m_parent_function;
};

Function::Function(TranslationUnit *parent_tu, const std::string &function_id)
{
	// Save a pointer to our parent TranslationUnit for later reference.
	m_parent_tu = parent_tu;

	// Save our identifier.
	m_function_id = function_id;
}

Function::~Function()
{
}

std::string Function::GetDefinitionFilePath() const
{
	return m_parent_tu->GetFilePath();
}

bool Function::IsCalled() const
{
	// Determine if this function is ever called.
	// If the first statement (the ENTRY block) has any in-edges
	// other than its self-edge, it's called by something.
	return boost::in_degree(m_entry_vertex_desc, *m_cfg) > 1;
}

void Function::Link(const std::map<std::string, Function*> &function_map,
		T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls)
{
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);

	vertex_filter_predicate the_filter(vpm, this);
	typedef boost::filtered_graph<T_CFG, boost::keep_all,
			vertex_filter_predicate> T_FILTERED_CFG;
	T_FILTERED_CFG graph_of_this_function(*m_cfg, boost::keep_all(),
			the_filter);

	boost::graph_traits<T_FILTERED_CFG>::vertex_iterator vit, vend;
	boost::tie(vit, vend) = boost::vertices(graph_of_this_function);
	for (; vit != vend; vit++)
	{
		FunctionCallUnresolved *fcu =
				dynamic_cast<FunctionCallUnresolved*>((*m_cfg)[*vit].m_statement);
		if (fcu != NULL)
		{
			std::map<std::string, Function*>::const_iterator it;

			// We found an unresolved function call.  Try to resolve it.
			it = function_map.find(fcu->GetIdentifier());

			if (it == function_map.end())
			{
				// Couldn't resolve it.  Add it to the unresolved call list.
				unresolved_function_calls->insert(T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP::value_type(fcu->GetIdentifier(), fcu));
			}
			else
			{
				// Found it.
				// Replace the FunctionCallUnresolved with a FunctionCallResolved.
				FunctionCallResolved *fcr = new FunctionCallResolved(it->second,
						fcu);
				// Delete the FunctionCallUnresolved object...
				delete fcu;
				// ...and replace it with the FunctionCallResolved object.
				(*m_cfg)[*vit].m_statement = fcr;

				// Now add the appropriate CFG edges.
				// The FunctionCall->Function->entrypoint edge.
				CFGEdgeTypeFunctionCall *call_edge_type =
						new CFGEdgeTypeFunctionCall(fcr);
				T_CFG_EDGE_DESC new_edge_desc;
				bool ok;

				boost::tie(new_edge_desc, ok) = boost::add_edge(*vit,
						it->second->GetEntryVertexDescriptor(), *m_cfg);
				if (ok)
				{
					// Edge was added OK, let's connect the edge properties.
					(*m_cfg)[new_edge_desc].m_edge_type = call_edge_type;
				}
				else
				{
					// We couldn't add the edge.  This should never happen.
					std::cerr << "ERROR: Can't add call edge." << std::endl;
				}

				// Add the return edge.
				// The return edge goes from the EXIT of the called function to
				// the node in the CFG which is after the FunctionCall.  There is
				// only ever one normal (i.e. fallthrough) edge from the FunctionCall
				// to the next statement in its containing function.
				T_CFG_EDGE_DESC function_call_out_edge;

				boost::tie(function_call_out_edge, ok) = GetFirstOutEdgeOfType<CFGEdgeTypeFallthrough>(*vit, *m_cfg);
				if (!ok)
				{
					// Couldn't find the return.
					std::cerr
							<< "ERROR: COULDN'T FIND OUT EDGE OF TYPE CFGEdgeTypeFallthrough"
							<< std::endl;
					std::cerr << "Edges found are:" << std::endl;
					//PrintOutEdgeTypes(*vit, *m_cfg);
				}

				boost::tie(new_edge_desc, ok) = boost::add_edge(
						it->second->GetExitVertexDescriptor(),
						boost::target(function_call_out_edge, *m_cfg), *m_cfg);
				if (ok)
				{
					// Return edge was added OK.  Create and connect the edge's properties.
					CFGEdgeTypeReturn *return_edge_type = new CFGEdgeTypeReturn(
							fcr);
					(*m_cfg)[new_edge_desc].m_edge_type = return_edge_type;
					// Copy the fallthrough edge's properties to the newly-added return edge.
					/// @todo Find a cleaner way to do this.
					return_edge_type->MarkAsBackEdge(
							(*m_cfg)[function_call_out_edge].m_edge_type->IsBackEdge());

					// Change the type of FunctionCall's out edge to a "FunctionCallBypass".
					// For graphing just the function itself, we'll look at these edges and not the
					// call/return edges.
					CFGEdgeTypeFunctionCallBypass *fcbp =
							new CFGEdgeTypeFunctionCallBypass();
					// Copy the fallthrough edge's properties to its replacement.
					/// @todo Find a cleaner way to do this.
					fcbp->MarkAsBackEdge(
							(*m_cfg)[function_call_out_edge].m_edge_type->IsBackEdge());
					delete (*m_cfg)[function_call_out_edge].m_edge_type;
					(*m_cfg)[function_call_out_edge].m_edge_type = fcbp;
				}
				else
				{
					// We couldn't add the edge.  This should never happen.
					std::cerr << "ERROR: Can't add return edge." << std::endl;
				}
			}
		}
	}
}

/**
 * Predicate for filtering back edges out of the CFG.
 */
struct back_edge_filter_predicate
{
	/// Must be default constructible because such predicates are stored by-value.
	back_edge_filter_predicate()
	{
	};
	back_edge_filter_predicate(T_EDGE_TYPE_PROPERTY_MAP &edge_type_property_map) :
			m_edge_type_property_map(edge_type_property_map)
	{
	};

	/**
	 * Returns false if @a eid is a back edge.
	 *
	 * @param eid Reference to an edge descriptor.
	 * @return
	 */
	bool operator()(const T_CFG_EDGE_DESC& eid) const
	{
		if (get(m_edge_type_property_map, eid)->IsBackEdge())
		{
			// This is a back edge, filter it out.
			return false;
		}
		else
		{
			// This is not a back edge.
			return true;
		}
	};

	T_EDGE_TYPE_PROPERTY_MAP m_edge_type_property_map;
};

static void indent(long i)
{
	while (i > 0)
	{
		std::cout << "    ";
		i--;
	};
}

static long filtered_in_degree(T_CFG_VERTEX_DESC v, const T_CFG &cfg, bool only_decision_predecessors = false)
{
	boost::graph_traits<T_CFG>::in_edge_iterator ieit, ieend;

	boost::tie(ieit, ieend) = boost::in_edges(v, cfg);

	long i = 0;
	bool saw_function_call_already = false;
	for (; ieit != ieend; ++ieit)
	{
		if (cfg[*ieit].m_edge_type->IsBackEdge())
		{
			// Always skip anything marked as a back edge.
			continue;
		}

		if(only_decision_predecessors)
		{
			// Is the predecessor a decision statement?
			if(!cfg[boost::source(*ieit,cfg)].m_statement->IsDecisionStatement())
			{
				continue;
			}
		}

		// Count up all the incoming edges, with two exceptions:
		// - Ignore Return edges.  They will always have exactly one matching FunctionCallBypass, which
		//   is what we'll count instead.
		// - Ignore all but the first CFGEdgeTypeFunctionCall.  The situation here is that we'd be
		//   looking at a vertex v that's an ENTRY statement, with a predecessor of type FunctionCallResolved.
		//   Any particular instance of an ENTRY has at most only one valid FunctionCall edge.
		//   For our current purposes, we only care about this one.
		if ((dynamic_cast<CFGEdgeTypeReturn*>(cfg[*ieit].m_edge_type) == NULL)
				&& (saw_function_call_already == false))
		{
			i++;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(cfg[*ieit].m_edge_type)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return i;
}

T_CFG_EDGE_DESC first_filtered_out_edge(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
{
	boost::graph_traits<T_CFG>::in_edge_iterator ieit, ieend;

	boost::tie(ieit, ieend) = boost::in_edges(v, cfg);

	bool saw_function_call_already = false;
	for (; ieit != ieend; ++ieit)
	{
		if (cfg[*ieit].m_edge_type->IsBackEdge())
		{
			// Always skip anything marked as a back edge.
			continue;
		}

		// Count up all the incoming edges, with two exceptions:
		// - Ignore Return edges.  They will always have exactly one matching FunctionCallBypass, which
		//   is what we'll count instead.
		// - Ignore all but the first CFGEdgeTypeFunctionCall.  The situation here is that we'd be
		//   looking at a vertex v that's an ENTRY statement, with a predecessor of type FunctionCallResolved.
		//   Any particular instance of an ENTRY has at most only one valid FunctionCall edge.
		//   For our current purposes, we only care about this one.
		if ((dynamic_cast<CFGEdgeTypeReturn*>(cfg[*ieit].m_edge_type) == NULL)
				&& (saw_function_call_already == false))
		{
			return *ieit;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(cfg[*ieit].m_edge_type)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return *ieend;
}

static long filtered_out_degree(T_CFG_VERTEX_DESC v, const T_CFG &cfg)
{
	boost::graph_traits<T_CFG>::out_edge_iterator eit, eend;

	boost::tie(eit, eend) = boost::out_edges(v, cfg);

	long i = 0;
	for (; eit != eend; ++eit)
	{
		if (cfg[*eit].m_edge_type->IsBackEdge())
		{
			// Skip anything marked as a back edge.
			continue;
		}
		i++;
	}
	return i;
}

using std::cerr;
using std::cout;
using std::endl;

/**
 * Visitor which, when passed to topological_visit_kahn, prints out the control flow graph.
 */
class function_control_flow_graph_visitor: public ControlFlowGraphVisitorBase
{
public:
	function_control_flow_graph_visitor(ControlFlowGraph &g,
			T_CFG_VERTEX_DESC last_statement,
			bool cfg_verbose,
			bool cfg_vertex_ids) :
			ControlFlowGraphVisitorBase(g)
	{
		m_last_statement = last_statement;
		m_cfg_verbose = cfg_verbose;
		m_cfg_vertex_ids = cfg_vertex_ids;
		m_last_discovered_vertex_is_recursive = false;
	};
	function_control_flow_graph_visitor(
			function_control_flow_graph_visitor &original) :
			ControlFlowGraphVisitorBase(original)
	{
	};
	~function_control_flow_graph_visitor()
	{
	};

	vertex_return_value_t start_vertex(T_CFG_EDGE_DESC u)
	{
		// The very first vertex has been popped.

		// We're at the first function entry point.
		PushCallStack(NULL);
		m_indent_level = 0;

		return vertex_return_value_t::ok;
	};

	vertex_return_value_t discover_vertex(T_CFG_VERTEX_DESC u, T_CFG_EDGE_DESC e)
	{
		// We found a new vertex.

		StatementBase *p = m_graph[u].m_statement;

		if(p->IsType<Entry>())
		{
			// We're visiting a function entry point.
			// Push a new call stack frame.

			indent(m_indent_level);
			std::cout << "[" << std::endl;
			//PushCallStack(m_next_function_call_resolved);
			m_indent_level++;
		}

		// Check if this vertex is the first vertex of a new branch of the control flow graph.
		long fid = filtered_in_degree(u, m_graph);
		if(fid==1)
		{
			T_CFG_VERTEX_DESC predecessor;
			predecessor = boost::source(e, m_graph);
			if(m_graph[predecessor].m_statement->IsDecisionStatement())
			{
				// Predecessor was a decision statement, so this vertex starts a new branch.
				// Print a block start marker at the current indent level minus one.
				indent(m_indent_level);
				std::cout << "{" << endl;
				m_indent_level++;
			}
		}
		else if (fid > 2)
		{
			// This vertex has some extra incoming edges that haven't been outdented.
			for(long i=fid-2; i>0; --i)
			{
				m_indent_level--;
				indent(m_indent_level);
				std::cout << "}" << std::endl;
			}
		}

		// Check if this vertex meets the criteria for printing the statement.
		if(m_cfg_verbose || (p->IsDecisionStatement() || (p->IsFunctionCall())))
		{
			// Indent and print the statement corresponding to this vertex.
			indent(m_indent_level);
			std::cout << p->GetIdentifierCFG();
			if(m_cfg_vertex_ids)
			{
				// Print the vertex ID.
				std::cout << " [" << u << "]";
			}
			std::cout << " <" << p->GetLocation() << ">" << std::endl;
		}

		if (u == m_last_statement)
		{
			//std::clog << "INFO: Found last statement of function" << std::endl;
			// We've reached the end of the function, terminate the search.
			// We should never have to do this, the topological search should always
			// terminate on the EXIT vertex unless there is a branch which erroneously terminates.
			//return terminate_search;				
		}

		if (p->IsType<FunctionCallResolved>())
		{
			// This is a function call which has been resolved (i.e. has a link to the
			// actual Function that's being called).  Track the call context, and 
			// check if we're going recursive.
			FunctionCallResolved *fcr;

			fcr = dynamic_cast<FunctionCallResolved*>(p);

			// Assume we're not.
			m_last_discovered_vertex_is_recursive = false;

			if(AreWeRecursing(fcr->m_target_function))
			{
				// We're recursing, we need to treat this vertex as if it were a FunctionCallUnresolved.
				std::cout << "RECURSION DETECTED: Function \"" << fcr->m_target_function << "\"" << std::endl;
				m_last_discovered_vertex_is_recursive = true;
			}
			else
			{
				// We're not recursing, push a normal stack frame and do the call.
				PushCallStack(fcr);
			}
		}

		return vertex_return_value_t::ok;
	}

	edge_return_value_t examine_edge(T_CFG_EDGE_DESC ed)
	{
		// Filter out any edges that we want to pretend aren't even part of the
		// graph we're looking at.
		CFGEdgeTypeBase *edge_type;
		CFGEdgeTypeFunctionCall *fc;
		CFGEdgeTypeReturn *ret;
		CFGEdgeTypeFunctionCallBypass *fcb;

		edge_type = m_graph[ed].m_edge_type;

		// Attempt dynamic casts to call/return types to see if we need to handle
		// these specially.
		fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(edge_type);
		ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);
		fcb = dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(edge_type);

		if(ed.m_source == ed.m_target)
		{
			// Skip all edges which have the same source and target.  That means they're ENTRY or EXIT pseudostatements.
			return edge_return_value_t::terminate_branch;
		}

		if (edge_type->IsBackEdge())
		{
			// Skip all back edges.
			return edge_return_value_t::terminate_branch;
		}

		if (ret != NULL)
		{
			// This is a return edge.

			if(IsCallStackEmpty())
			{
				// Should never get here.
				cout << "EMPTY" << endl;
			}
			else if(TopCallStack() == NULL)
			{
				// We're at the top of the call stack, and we're trying to return.
				cout << "NULL" << endl;
				return edge_return_value_t::terminate_branch;
			}
			else if(ret->m_function_call != TopCallStack())
			{
				// This edge is a return, but not the one corresponding to the FunctionCall
				// that brought us here.  Or, the call stack is empty, indicating that we got here
				// by means other than tracing the control-flow graph (e.g. we started tracing the
				// graph at an internal vertex).
				// Skip it.
				/// @todo An empty call stack here could also be an error in the program.  We should maybe
				/// add a fake "call" when starting a cfg trace from an internal vertex.
				return edge_return_value_t::terminate_branch;
			}
		}

		// Handle recursion.
		// We deal with recursion by deciding here which path to take out of a FunctionCallResolved vertex.
		// Note that this is currently the only vertex type which can result in recursion.
		if ((fcb != NULL) && (m_last_discovered_vertex_is_recursive == false))
		{
			// If we're not in danger of infinite recursion,
			// skip FunctionCallBypasses entirely.  Otherwise take them.
			return edge_return_value_t::terminate_branch;
		}
		else if ((fc != NULL)
				&& (m_last_discovered_vertex_is_recursive == true))
		{
			// If we are in danger of infinite recursion,
			// skip FunctionCalls entirely.  Otherwise take them.
			cout << "t3" << endl;
			return edge_return_value_t::terminate_branch;
		}

		return edge_return_value_t::ok;
	}

	void vertex_visit_complete(T_CFG_VERTEX_DESC u, long num_vertices_pushed, T_CFG_EDGE_DESC e)
	{
		// Check if we're leaving an Exit vertex.
		StatementBase *p = m_graph[u].m_statement;
		if(p->IsType<Exit>())
		{
			// We're leaving the function we were in, pop the call stack entry it pushed.
			PopCallStack();

			// Outdent.
			m_indent_level--;
			indent(m_indent_level);
			std::cout << "]" << std::endl;

			if(m_last_statement == u)
			{
				// This is the last statement of the function we were printing.
				// No need to check if we need to outdent due to a branch termination.
				return;
			}
		}

		// Check if the vertex we terminate on has more than just us coming in.
		if(num_vertices_pushed == 0)
		{
			// No target vertices pushed by this vertex.  That means that some other vertex did push our target vertex,
			// or that we have no out edges.  Either way we terminate the branch.
			m_indent_level--;
			indent(m_indent_level);

			// We're leaving a branch indent context.
			std::cout << "}" << std::endl;
		}

		if	((num_vertices_pushed == 1) && (filtered_in_degree(boost::target(e, m_graph), m_graph) > 1))
		{
			// The edge will end on a merge vertex.  Outdent.
			m_indent_level--;
			indent(m_indent_level);
			std::cout << "}" << std::endl;
		}
	}

private:

	/// Vertex corresponding to the last statement of the function.
	/// We'll terminate the search when we find this.
	T_CFG_VERTEX_DESC m_last_statement;

	/// Flag indicating if we should only print function calls and flow control constructs.
	bool m_cfg_verbose;

	/// Flag indicating if we should print the vertex ID.
	bool m_cfg_vertex_ids;

	/// The current indentation level of the output control flow graph.
	/// This is affected by both intra-function branch-producing instructions (if()'s and switch()'s) and
	/// by inter-Function operations (function calls).
	long m_indent_level;

	bool m_last_discovered_vertex_is_recursive;
};


void Function::PrintControlFlowGraph(bool cfg_verbose, bool cfg_vertex_ids)
{
#if 0
	// Set up the color map stack.
	typedef boost::color_traits<boost::default_color_type> T_COLOR;
	typedef std::map< T_CFG_VERTEX_DESC, boost::default_color_type > T_COLOR_MAP;
	std::vector< T_COLOR_MAP* > color_map_stack;

	// Set up the visitor.
	function_control_flow_graph_visitor cfg_visitor(*m_cfg, m_exit_vertex_desc);

	// Do a depth-first search of the control flow graph.
	improved_depth_first_visit(*m_cfg, m_entry_vertex_desc, cfg_visitor, color_map_stack);
#else
	// Set up the visitor.
	function_control_flow_graph_visitor cfg_visitor(*m_the_cfg, m_exit_vertex_desc, cfg_verbose, cfg_vertex_ids);
	topological_visit_kahn(*m_cfg, m_entry_vertex_self_edge, cfg_visitor);
#endif
}

/// Functor for writing GraphViz dot-compatible info for the function's entire CFG.
struct graph_property_writer
{
	graph_property_writer(Function * function) : m_function(function) {};

	void operator()(std::ostream& out) const
	{
		out << "graph [clusterrank=local colorscheme=svg]" << std::endl;
		out << "subgraph cluster0 {" << std::endl;
		out << "label = \"" << m_function->GetIdentifier() << "\";" << std::endl;
		out << "labeljust = \"l\";" << std::endl;
		out << "node [shape=rectangle fontname=\"Helvetica\"]" << std::endl;
		out << "edge [style=solid]" << std::endl;
		out << "{ rank = source; " << m_function->GetEntryVertexDescriptor() << "; }" << std::endl;
		out << "{ rank = sink; " << m_function->GetExitVertexDescriptor() << "; }" << std::endl;
	}

	Function* m_function;
};

/**
 * Class for a vertex property writer, for use with write_graphviz().
 */
class cfg_vertex_property_writer
{
public:
	cfg_vertex_property_writer(T_CFG g) :	m_graph(g)	{ };

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


void Function::PrintControlFlowGraphDot(bool cfg_verbose, bool cfg_vertex_ids, const std::string & output_filename)
{
	T_VERTEX_PROPERTY_MAP vpm = boost::get(
			&CFGVertexProperties::m_containing_function, *m_cfg);

	vertex_filter_predicate the_filter(vpm, this);
	boost::filtered_graph<T_CFG, boost::keep_all, vertex_filter_predicate> graph_of_this_function(
			*m_cfg, boost::keep_all(), the_filter);

	std::clog << "Creating " << output_filename << std::endl;

	std::ofstream outfile(output_filename.c_str());

	boost::write_graphviz(outfile, graph_of_this_function,
			cfg_vertex_property_writer(*m_cfg),
			cfg_edge_property_writer(*m_cfg),
			graph_property_writer(this));

	// graph_property_writer() added a subgraph, which "uses up" the "}" that write_graphviz streams out.
	// Terminate the graph appropriately.
	outfile << "\n}" << std::endl;

	outfile.close();
}


void Function::PrintControlFlowGraphBitmap(ToolDot *the_dot, const boost::filesystem::path& output_filename)
{
	boost::filesystem::path dot_filename;

	dot_filename = output_filename;
	dot_filename.replace_extension(".dot");

	PrintControlFlowGraphDot(true, true, dot_filename.generic_string());

	std::clog << "Compiling " << dot_filename.generic_string() << " to " << output_filename.generic_string() << std::endl;
	the_dot->CompileDotToPNG(dot_filename.generic_string(), output_filename.generic_string());
}

class LabelMap : public std::map< std::string, T_CFG_VERTEX_DESC>
{

};

bool Function::CreateControlFlowGraph(ControlFlowGraph & cfg, const std::vector< StatementBase* > &statement_list)
{
	LabelMap label_map;
	T_CFG_VERTEX_DESC prev_vertex;
	bool prev_vertex_ended_basic_block = false;

	// A list of basic block leader info.
	std::vector< BasicBlockLeaderInfo > list_of_leader_info;
	std::vector< T_CFG_VERTEX_DESC > list_of_statements_with_no_in_edge_yet;
	std::vector< T_CFG_VERTEX_DESC > list_of_unlinked_flow_control_statements;

	dlog_cfg << "Creating CFG for Function \"" << m_function_id << "\"" << std::endl;

	m_the_cfg = &cfg;
	m_cfg = &cfg.GetT_CFG();

	// Create ENTRY and EXIT vertices.
	Entry *entry_ptr = new Entry(Location("[" + GetDefinitionFilePath() + " : 0]"));
	Exit *exit_ptr = new Exit(Location("[" + GetDefinitionFilePath() + " : 0]"));

	m_entry_vertex_desc = cfg.AddVertex(entry_ptr, this);
	m_exit_vertex_desc = cfg.AddVertex(exit_ptr, this);

	// Add EXIT to the label map, so that ReturnUnlinked instances can find it.
	label_map["EXIT"] = m_exit_vertex_desc;

	prev_vertex = m_entry_vertex_desc;

	// Add all the statements to the Function's CFG.
	BOOST_FOREACH(StatementBase *sbp, statement_list)
	{
		// Add this Statement to the Control Flow Graph.
		T_CFG_VERTEX_DESC vid;
		vid = cfg.AddVertex(sbp, this);

		// Find all the label definitions in the function.
		if(sbp->IsType<Label>())
		{
			// This is a label, add it to the map.
			Label *lp = dynamic_cast<Label*>(sbp);
			if(label_map.count(lp->GetIdentifier()) != 0)
			{
				// There shouldn't be a label with this name already in the map.
				dlog_cfg << "WARNING: Detected duplicate label \"" << lp->GetIdentifier()
						<< "\"in function \"" << m_function_id << "\"" << std::endl;
			}
			label_map[lp->GetIdentifier()] = vid;
			dlog_cfg << "Added label " << lp->GetIdentifier() << std::endl;
		}

		// See what kind of edge we need to add.
		if(prev_vertex_ended_basic_block == false)
		{
			// The previous vertex didn't end its basic block.  Therefore, all we have to do is add a simple
			// fallthrough link to the this vertex.
			cfg.AddEdge(prev_vertex, vid, new CFGEdgeTypeFallthrough());
		}
		else
		{
			// The previous vertex did end its basic block.  This means it was a control transfer statement, such as an 'if', 'switch', or 'goto',
			// and that this vertex is a block leader.
			// We therefore don't add an edge into this statement, because the only way we'll get here is by an explicit jump via a
			// similar flow control statement.  This should be taken care of when we do the Label linking.  We'll add this vertex to a "watch list"
			// and check it at the end to make sure it has such an in-edge.
			list_of_statements_with_no_in_edge_yet.push_back(vid);

			// Record the basic block leaders.  We also capture the vertex which ended the immediately-preceding basic block, in case we
			// end up with no control transfers to this block.  In that case, we'll add an Impossible edge between the two to maintain
			// the invariant that EXIT post-dominates all vertices of the function.
			list_of_leader_info.push_back(BasicBlockLeaderInfo(vid, prev_vertex));
		}

		// Did the current statement end its basic block?
		if(sbp->IsType<FlowControlUnlinked>())
		{
			// It did, by its nature of being a flow control statement.
			// Note that for our purposes here, FunctionCalls do not count as flow control statements.
			list_of_unlinked_flow_control_statements.push_back(vid);
			prev_vertex_ended_basic_block = true;
		}
		else
		{
			prev_vertex_ended_basic_block = false;
		}

		// Now this vertex is the previous vertex.
		prev_vertex = vid;
	}

	// Was the last vertex in the list *not* a flow control statement?
	if(prev_vertex_ended_basic_block == false)
	{
		// It wasn't, which means it falls through to the EXIT vertex.
		// Add an edge to the EXIT vertex.
		cfg.AddEdge(prev_vertex, m_exit_vertex_desc, new CFGEdgeTypeFallthrough());
	}

	//
	// At this point, we've created the basic blocks and at the same time added all the fallthrough edges.
	// Now we must link the basic blocks together.
	//

	// Link the FlowControlUnlinked-derived statements (i.e. link jumps to their targets).
	dlog_cfg << "INFO: Linking FlowControlUnlinked-derived statements." << std::endl;
	BOOST_FOREACH(T_CFG_VERTEX_DESC vd, list_of_unlinked_flow_control_statements)
	{
		FlowControlUnlinked *fcl = dynamic_cast<FlowControlUnlinked*>(cfg.GetStatementPtr(vd));
		dlog_cfg << "INFO: Linking " << typeid(*fcl).name() << std::endl;
		StatementBase* replacement_statement = fcl->ResolveLinks(cfg, vd, label_map);

		if(replacement_statement != NULL)
		{
			// The ResolveLinks call succeeded.  Replace the *Unlinked() class instance with a suitable linked instance.
			dlog_cfg << "INFO: Linked " << typeid(*fcl).name() << std::endl;
			cfg.GetT_CFG()[vd].m_statement = replacement_statement;
			delete fcl;
		}
		else
		{
			// The ResolveLinks call failed.  Not sure we can do much here, but we won't delete the FlowControlUnlinked object since
			// we don't have anything to replace it with.
			dlog_cfg << "ERROR: ResolveLinks() call failed." << std::endl;
		}
	}
	dlog_cfg << "INFO: Linking complete." << std::endl;

	// Now we have to add Impossible in edges for any leader vertices which we haven't already linked above.
	// This happens in the following cases:
	//  - Infinite loops
	//  - Dead code that's been "unlinked" by gcc before we get a chance to look at it.
	dlog_cfg << "INFO: Adding impossible edges." << std::endl;
	AddImpossibleEdges(cfg, list_of_leader_info);
	dlog_cfg << "INFO: Impossible edge addition complete." << std::endl;


	dlog_cfg << "INFO: Checking for unreachable code." << std::endl;
	std::vector< T_CFG_VERTEX_DESC > statements_with_no_in_edge;
	CheckForNoInEdges(cfg, list_of_statements_with_no_in_edge_yet, &statements_with_no_in_edge);
	dlog_cfg << "INFO: Check complete." << std::endl;

	// Add self edges to the ENTRY and EXIT vertices.
	m_entry_vertex_self_edge = cfg.AddEdge(m_entry_vertex_desc, m_entry_vertex_desc, new CFGEdgeTypeImpossible);
	m_exit_vertex_self_edge = cfg.AddEdge(m_exit_vertex_desc, m_exit_vertex_desc, new CFGEdgeTypeImpossible);

	dlog_cfg << "INFO: Fixing up back edges." << std::endl;
	cfg.FixupBackEdges(this);
	dlog_cfg << "INFO: Fix up complete." << std::endl;

	return true;
}

void Function::AddImpossibleEdges(ControlFlowGraph & cfg, std::vector<BasicBlockLeaderInfo> & leader_info_list)
{
	BOOST_FOREACH(BasicBlockLeaderInfo p, leader_info_list)
	{
		// Check if the leader has been linked.
		long in_degree;

		in_degree = cfg.InDegree(p.m_leader);

		if(in_degree != 0)
		{
			// It's been linked, skip it.
			continue;
		}

		// Link it to its immediate predecessor with an Impossible edge.
		cfg.AddEdge(p.m_immediate_predecessor, p.m_leader, new CFGEdgeTypeImpossible);
	}
}

bool Function::CheckForNoInEdges(ControlFlowGraph & cfg,
		std::vector< T_CFG_VERTEX_DESC > &list_of_statements_with_no_in_edge_yet,
		std::vector< T_CFG_VERTEX_DESC > *output)
{
	bool retval = false;

	BOOST_FOREACH(T_CFG_VERTEX_DESC vd, list_of_statements_with_no_in_edge_yet)
	{
		long in_degree;

		in_degree = cfg.InDegree(vd);

		if(in_degree == 0)
		{
			std::cerr << "WARNING: CFG of function " << GetIdentifier() << " is not connected." << std::endl;
			output->push_back(vd);

			// We found a statement with no in edges.
			retval = true;
		}
	}

	return retval;
}

void Function::DumpCFG()
{
	/// @todo Implement.
}

