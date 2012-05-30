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
#include "controlflowgraph/algorithms/cfg_algs.h"
#include "controlflowgraph/SparsePropertyMap.h"
#include "controlflowgraph/algorithms/topological_visit_kahn.h"
#include "controlflowgraph/visitors/ControlFlowGraphVisitorBase.h"
#include "controlflowgraph/algorithms/depth_first_traversal.hpp"
#include "controlflowgraph/visitors/WriteGraphvizDotFileVisitor.h"

// For the use of function_control_flow_graph_visitor
//#include "controlflowgraph/CallStackFrameBase.h"
#include "controlflowgraph/visitors/FunctionCFGVisitor.h"

#include "libexttools/ToolDot.h"

#include "parsers/gcc_gimple_parser.h"


Function::Function(TranslationUnit *parent_tu, const std::string &function_id)
{
	// Save a pointer to our parent TranslationUnit for later reference.
	m_parent_tu = parent_tu;

	// Save our identifier.
	m_function_id = function_id;

	// Create a new ControlFlowGraph for this function.
	m_the_cfg = new ControlFlowGraph;
}

Function::~Function()
{
	// Delete the ControlFlowGraph we created in the constructor.
	delete m_the_cfg;
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
	return m_entry_vertex_desc->InDegree() > 1;
}

void Function::Link(const std::map<std::string, Function*> &function_map,
		T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP *unresolved_function_calls)
{
	// Iterate over all the vertices in this CFG, looking for FunctionCallUnresolved's.

	boost::graph_traits<ControlFlowGraph>::vertex_iterator vit, vend;
	boost::tie(vit, vend) = vertices(*m_the_cfg);

	typedef boost::graph_traits<ControlFlowGraph>::vertex_descriptor T_VERTEX_DESC;
	typedef std::pair<T_VERTEX_DESC, T_VERTEX_DESC> T_VERTEX_DESCRIPTOR_PAIR;
	std::vector<T_VERTEX_DESCRIPTOR_PAIR> vertex_replacement_info;

	// Visit all vertices in this Function looking for unresolved function calls.
	for (; vit != vend; vit++)
	{
		FunctionCallUnresolved *fcu = dynamic_cast<FunctionCallUnresolved*>(*vit);

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

				// Replace the FunctionCallUnresolved with a FunctionCallResolved.  We can't do the replacement here
				// because the iterators would be invalidated, so we create the replacement vertex
				// and add them both to the vertex_replacement_info list, which we'll traverse later and
				// do the actual replacement.

				// Create the replacement vertex.
				FunctionCallResolved *fcr = new FunctionCallResolved(it->second, fcu);

				// Add the vertexes to the replacement info list.
				vertex_replacement_info.push_back(std::make_pair(*vit, fcr));
			}
		}
	}

	// Now replace the unlinked vertices with the linked ones.
	BOOST_FOREACH(T_VERTEX_DESCRIPTOR_PAIR p, vertex_replacement_info)
	{
		dlog_cfg << "INFO: Replacing Vertex..." << std::endl;
		m_the_cfg->ReplaceVertex(p.first, p.second);
		dlog_cfg << "INFO: Replaced Vertex." << std::endl;
		dlog_cfg << "INFO: Deleting old Vertex..." << std::endl;
		delete p.first;
		dlog_cfg << "INFO: Deleted old Vertex." << std::endl;

		// Now add the FunctionCall and Return edges.
		FunctionCallResolved *fcr = dynamic_cast<FunctionCallResolved*>(p.second);
		CFGEdgeTypeFunctionCall *call_edge = new CFGEdgeTypeFunctionCall(fcr);
		CFGEdgeTypeReturn *return_edge = new CFGEdgeTypeReturn(fcr);
		CFGEdgeTypeFallthrough *function_calls_fallthrough_edge = fcr->GetFirstOutEdgeOfType<CFGEdgeTypeFallthrough>();
		m_the_cfg->AddEdge(fcr, fcr->GetCalledFunction()->GetEntryVertexDescriptor(), call_edge);
		// The return edge goes from the called Function's Exit vertex, which is in a different ControlFlowGraph,
		// to the next vertex after the FunctionCallResolved vertex.
		ControlFlowGraph *other_cfg = fcr->GetCalledFunction()->GetCFGPointer();
		other_cfg->AddEdge(fcr->GetCalledFunction()->GetExitVertexDescriptor(),
				function_calls_fallthrough_edge->Target(), return_edge);
	}
}

long filtered_in_degree(ControlFlowGraph::vertex_descriptor v, bool only_decision_predecessors = false)
{
	StatementBase::in_edge_iterator ieit, ieend;

	v->InEdges(&ieit, &ieend);

	long i = 0;
	bool saw_function_call_already = false;
	for (; ieit != ieend; ++ieit)
	{
		if ((*ieit)->IsBackEdge())
		{
			// Always skip anything marked as a back edge.
			continue;
		}

		if(only_decision_predecessors)
		{
			// Is the predecessor a decision statement?
			if(!(*ieit)->Source()->IsDecisionStatement())
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
		if ((dynamic_cast<CFGEdgeTypeReturn*>(*ieit) == NULL)
				&& (saw_function_call_already == false))
		{
			i++;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(*ieit)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return i;
}



ControlFlowGraph::edge_descriptor first_filtered_out_edge(ControlFlowGraph::vertex_descriptor v)
{
	StatementBase::out_edge_iterator ieit, ieend;

	v->OutEdges(&ieit, &ieend);

	bool saw_function_call_already = false;
	for (; ieit != ieend; ++ieit)
	{
		if ((*ieit)->IsBackEdge())
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
		if ((dynamic_cast<CFGEdgeTypeReturn*>(*ieit) == NULL)
				&& (saw_function_call_already == false))
		{
			return *ieit;
		}

		if (dynamic_cast<CFGEdgeTypeFunctionCall*>(*ieit)
				!= NULL)
		{
			// Multiple incoming function calls only count as one for convergence purposes.
			saw_function_call_already = true;
		}
	}

	return *ieend;
}

static long filtered_out_degree(ControlFlowGraph::vertex_descriptor v)
{
	StatementBase::out_edge_iterator eit, eend;

	v->OutEdges(&eit, &eend);

	long i = 0;
	for (; eit != eend; ++eit)
	{
		if ((*eit)->IsBackEdge())
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

#if 0
/**
 * Visitor which, when passed to topological_visit_kahn, prints out the control flow graph.
 */
class function_control_flow_graph_visitor: public ControlFlowGraphVisitorBase
{
public:
	function_control_flow_graph_visitor(ControlFlowGraph &g,
			ControlFlowGraph::vertex_descriptor last_statement,
			bool cfg_verbose,
			bool cfg_vertex_ids) :
			ControlFlowGraphVisitorBase(g)
	{
		m_last_statement = last_statement;
		m_cfg_verbose = cfg_verbose;
		m_cfg_vertex_ids = cfg_vertex_ids;
		m_last_discovered_vertex_is_recursive = false;
		m_indent_level = 0;
	};
	function_control_flow_graph_visitor(
			function_control_flow_graph_visitor &original) :
			ControlFlowGraphVisitorBase(original)
	{
	};
	~function_control_flow_graph_visitor()
	{
	};

	vertex_return_value_t start_vertex(ControlFlowGraph::edge_descriptor u)
	{
		// The very first vertex has been popped.

		// We're at the first function entry point.
		m_call_stack->PushCallStack(NULL);
		m_indent_level = 0;

		return vertex_return_value_t::ok;
	};

	vertex_return_value_t discover_vertex(ControlFlowGraph::vertex_descriptor u, ControlFlowGraph::edge_descriptor e)
	{
		// We found a new vertex.

		StatementBase *p = u;

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
		long fid = filtered_in_degree(u);
		if(fid==1)
		{
			ControlFlowGraph::vertex_descriptor predecessor;
			predecessor = e->Source();
			if(predecessor->IsDecisionStatement())
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

			if(m_call_stack->AreWeRecursing(fcr->m_target_function))
			{
				// We're recursing, we need to treat this vertex as if it were a FunctionCallUnresolved.
				std::cout << "RECURSION DETECTED: Function \"" << fcr->m_target_function << "\"" << std::endl;
				m_last_discovered_vertex_is_recursive = true;
			}
			else
			{
				// We're not recursing, push a normal stack frame and do the call.
				m_call_stack->PushCallStack(new CallStackFrameBase(fcr, fcr->m_target_function->GetCFGPointer()));
			}
		}

		return vertex_return_value_t::ok;
	}

	edge_return_value_t examine_edge(ControlFlowGraph::edge_descriptor ed)
	{
		// Filter out any edges that we want to pretend aren't even part of the
		// graph we're looking at.
		CFGEdgeTypeBase *edge_type;
		CFGEdgeTypeFunctionCall *fc;
		CFGEdgeTypeReturn *ret;
		CFGEdgeTypeFunctionCallBypass *fcb;

		edge_type = ed;

		// Attempt dynamic casts to call/return types to see if we need to handle
		// these specially.
		fc = dynamic_cast<CFGEdgeTypeFunctionCall*>(edge_type);
		ret = dynamic_cast<CFGEdgeTypeReturn*>(edge_type);
		fcb = dynamic_cast<CFGEdgeTypeFunctionCallBypass*>(edge_type);

		if(ed->Source() == ed->Target())
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

			if(m_call_stack->IsCallStackEmpty())
			{
				// Should never get here.
				cout << "EMPTY" << endl;
			}
			else if(m_call_stack->TopCallStack() == NULL)
			{
				// We're at the top of the call stack, and we're trying to return.
				cout << "NULL" << endl;
				return edge_return_value_t::terminate_branch;
			}
			else if(ret->m_function_call != m_call_stack->TopCallStack()->GetPushingCall())
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

	void vertex_visit_complete(ControlFlowGraph::vertex_descriptor u, long num_vertices_pushed, ControlFlowGraph::edge_descriptor e)
	{
		// Check if we're leaving an Exit vertex.
		StatementBase *p = u;
		if(p->IsType<Exit>())
		{
			// We're leaving the function we were in, pop the call stack entry it pushed.
			m_call_stack->PopCallStack();

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

		if	((num_vertices_pushed == 1) && (filtered_in_degree(e->Target()) > 1))
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
	ControlFlowGraph::vertex_descriptor m_last_statement;

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
#endif

struct filtered_in_degree_functor
{
	const long operator()(ControlFlowGraph::vertex_descriptor vd) const { return filtered_in_degree(vd); };
};

void Function::PrintControlFlowGraph(bool cfg_verbose, bool cfg_vertex_ids)
{
	// Set up the RemainingInDegreeMap.
	typedef SparsePropertyMap<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor,
				typename boost::graph_traits<ControlFlowGraph>::degree_size_type,
				0,
				filtered_in_degree_functor> T_IN_DEGREE_MAP;
	T_IN_DEGREE_MAP remaining_in_degree_map;

	// Set up the visitor.
	FunctionCFGVisitor cfg_visitor(*m_the_cfg, m_exit_vertex_desc, cfg_verbose, cfg_vertex_ids);
	topological_visit_kahn(*m_the_cfg, m_entry_vertex_self_edge, cfg_visitor, remaining_in_degree_map);
}


void Function::PrintControlFlowGraphDot(bool cfg_verbose, bool cfg_vertex_ids, const std::string & output_filename)
{
	std::clog << "Creating " << output_filename << std::endl;

	std::ofstream outfile(output_filename.c_str());

	// Create the visitor which will insert the GraphViz info into outfile.
	WriteGraphvizDotFileVisitor visitor(*m_the_cfg, outfile);

	// Let the visitor visit all vertices and edges in the graph.
	improved_depth_first_visit(*m_the_cfg, m_entry_vertex_desc, visitor);

	// Terminate the graph appropriately.
	outfile << " }" << std::endl;
	outfile << "}" << std::endl;

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


class LabelMap : public std::map< std::string, ControlFlowGraph::vertex_descriptor>
{

};

bool Function::CreateControlFlowGraph(const std::vector< StatementBase* > &statement_list)
{
	LabelMap label_map;
	ControlFlowGraph::vertex_descriptor prev_vertex;
	bool prev_vertex_ended_basic_block = false;

	// A list of basic block leader info.
	std::vector< BasicBlockLeaderInfo > list_of_leader_info;
	std::vector< ControlFlowGraph::vertex_descriptor > list_of_statements_with_no_in_edge_yet;
	std::vector< ControlFlowGraph::vertex_descriptor > list_of_unlinked_flow_control_statements;

	dlog_cfg << "Creating CFG for Function \"" << m_function_id << "\"" << std::endl;

	// Create ENTRY and EXIT vertices.
	Entry *entry_ptr = new Entry(Location("[" + GetDefinitionFilePath() + " : 0]"));
	Exit *exit_ptr = new Exit(Location("[" + GetDefinitionFilePath() + " : 0]"));

	entry_ptr->SetOwningFunction(this);
	exit_ptr->SetOwningFunction(this);

	m_the_cfg->AddVertex(entry_ptr);
	m_entry_vertex_desc = entry_ptr;
	m_the_cfg->AddVertex(exit_ptr);
	m_exit_vertex_desc = exit_ptr;

	// Add EXIT to the label map, so that ReturnUnlinked instances can find it.
	label_map["EXIT"] = m_exit_vertex_desc;

	prev_vertex = m_entry_vertex_desc;

	// Add all the statements to the Function's CFG.
	BOOST_FOREACH(StatementBase *sbp, statement_list)
	{
		// Add this Statement to the Control Flow Graph.
		ControlFlowGraph::vertex_descriptor vid;
		sbp->SetOwningFunction(this);
		m_the_cfg->AddVertex(sbp);
		vid = sbp;

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
			m_the_cfg->AddEdge(prev_vertex, vid, new CFGEdgeTypeFallthrough());
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
		m_the_cfg->AddEdge(prev_vertex, m_exit_vertex_desc, new CFGEdgeTypeFallthrough());
	}

	//
	// At this point, we've created the basic blocks and at the same time added all the fallthrough edges.
	// Now we must link the basic blocks together.
	//

	// Link the FlowControlUnlinked-derived statements (i.e. link jumps to their targets).
	dlog_cfg << "INFO: Linking FlowControlUnlinked-derived statements." << std::endl;
	BOOST_FOREACH(ControlFlowGraph::vertex_descriptor vd, list_of_unlinked_flow_control_statements)
	{
		FlowControlUnlinked *fcl = dynamic_cast<FlowControlUnlinked*>(vd);
		dlog_cfg << "INFO: Linking " << typeid(*fcl).name() << std::endl;
		StatementBase* replacement_statement = fcl->ResolveLinks(*m_the_cfg, vd, label_map);

		if(replacement_statement != NULL)
		{
			// The ResolveLinks call succeeded.  Replace the *Unlinked() class instance with a suitable linked instance.
			dlog_cfg << "INFO: Linked " << typeid(*fcl).name() << std::endl;
			//cfg.GetT_CFG()[vd].m_statement = replacement_statement;
			//delete fcl;
			//cfg.ReplaceStatementPtr(vd, replacement_statement);
			/// @todo This is probably wrong, it probably invalidates the iterator.
			m_the_cfg->ReplaceVertex(vd, replacement_statement);
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
	AddImpossibleEdges(*m_the_cfg, list_of_leader_info);
	dlog_cfg << "INFO: Impossible edge addition complete." << std::endl;


	dlog_cfg << "INFO: Checking for unreachable code." << std::endl;
	std::vector< ControlFlowGraph::vertex_descriptor > statements_with_no_in_edge;
	CheckForNoInEdges(*m_the_cfg, list_of_statements_with_no_in_edge_yet, &statements_with_no_in_edge);
	dlog_cfg << "INFO: Check complete." << std::endl;

	// Add self edges to the ENTRY and EXIT vertices.
	m_entry_vertex_self_edge = new CFGEdgeTypeImpossible;
	m_the_cfg->AddEdge(m_entry_vertex_desc, m_entry_vertex_desc, m_entry_vertex_self_edge);
	m_exit_vertex_self_edge = new CFGEdgeTypeImpossible;
	m_the_cfg->AddEdge(m_exit_vertex_desc, m_exit_vertex_desc, m_exit_vertex_self_edge);

	dlog_cfg << "INFO: Fixing up back edges." << std::endl;
	FixupBackEdges(m_the_cfg, m_entry_vertex_desc);
	dlog_cfg << "INFO: Fix up complete." << std::endl;

	dlog_cfg << "INFO: Removing redundant nodes." << std::endl;
	RemoveRedundantNodes(m_the_cfg);
	dlog_cfg << "INFO: Redundant node removal complete." << std::endl;

	return true;
}

void Function::AddImpossibleEdges(ControlFlowGraph & cfg, std::vector<BasicBlockLeaderInfo> & leader_info_list)
{
	BOOST_FOREACH(BasicBlockLeaderInfo p, leader_info_list)
	{
		// Check if the leader has been linked.
		long in_degree;

		in_degree = p.m_leader->InDegree();

		if(in_degree != 0)
		{
			// It's been linked, skip it.
			continue;
		}

		// Link it to its immediate predecessor with an Impossible edge.
		m_the_cfg->AddEdge(p.m_immediate_predecessor, p.m_leader, new CFGEdgeTypeImpossible);
	}
}

bool Function::CheckForNoInEdges(ControlFlowGraph & cfg,
		std::vector< ControlFlowGraph::vertex_descriptor > &list_of_statements_with_no_in_edge_yet,
		std::vector< ControlFlowGraph::vertex_descriptor > *output)
{
	bool retval = false;

	BOOST_FOREACH(ControlFlowGraph::vertex_descriptor vd, list_of_statements_with_no_in_edge_yet)
	{
		long in_degree;

		in_degree = vd->InDegree();

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

