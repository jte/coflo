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

#ifndef MERGENODEINSERTIONVISITOR_H
#define MERGENODEINSERTIONVISITOR_H

#include <vector>
#include <stack>

#include <boost/foreach.hpp>
#include <boost/graph/depth_first_search.hpp>

#include "../../debug_utils/debug_utils.hpp"
#include "../ControlFlowGraph.h"
#include "../edges/edge_types.h"
#include "../statements/statements.h"

/**
 * Visitor for inserting Merge nodes.
 *
 * @bug This class is incomplete and known to not function properly.
 *
 * @deprecated It is likely this class is not needed and may be removed in the near future.
 */
template <typename GraphType>
class MergeNodeInsertionVisitor : public boost::default_dfs_visitor
{
public:

	typedef typename boost::graph_traits<GraphType>::vertex_descriptor T_VERTEX_DESC;
	typedef typename boost::graph_traits<GraphType>::edge_descriptor T_EDGE_DESC;
	typedef typename boost::graph_traits<GraphType>::out_edge_iterator T_OUT_EDGE_ITERATOR;
	typedef typename boost::graph_traits<GraphType>::in_edge_iterator T_IN_EDGE_ITERATOR;

	/**
	 * Structure which will be used to return fixup information to the caller.
	 */
	struct MergeInsertionInfo
	{
		/// This is a list of 2 or more terminal vertices, plus a single terminal-target vertex,
		/// below which need to be inserted merge vertices.  The process looks like this:
		/// 1 2 => create merge vertex (1, 2) => -1
		/// 3 => create merge vertex (-1, 3) => -2
		/// 4 => create merge vertex (-2, 4) => -3
		/// 5 => link merge vertex -3 -> 5.
		/// @todo Add a drawing here.
		std::vector<T_EDGE_DESC> m_terminal_edges;
	};

	MergeNodeInsertionVisitor(std::vector<MergeInsertionInfo> *returned_merge_info) : boost::default_dfs_visitor()
	{
		m_returned_merge_info = returned_merge_info;
	};
	~MergeNodeInsertionVisitor() {};

	/*void examine_edge(T_EDGE_DESC e, const GraphType &g)
	{
		dlog_cfg << "Examining edge: " << e << std::endl;
	};*/

	void examine_edge(T_EDGE_DESC e, const GraphType &g)
	{
		T_VERTEX_DESC terminal_target;

		dlog_cfg << "FOUND EDGE=" << e << std::endl;

		terminal_target = boost::target(e, g);

		if(terminal_target == boost::source(e, g))
		{
			// This is a self-edge.
			return;
		}

		long indeg = filtered_in_degree(terminal_target, g);
		if(indeg > 1)
		{
			assert(m_last_decision_vertex_stack.size() > 0);
			m_last_decision_vertex_stack.top().m_merging_edges.push_back(e);

			dlog_cfg << "FOUND BRANCH-TERMINATING EDGE=" << e
					<< "\n IF=" << m_last_decision_vertex_stack.top().m_u
					<< "\n OUT DEGREE=" << m_last_decision_vertex_stack.top().m_remaining_out_degree
					<< "\n TARGET IN DEGREE=" << indeg
					<< std::endl;

			MergeInsertionInfo mi;

			while(m_last_decision_vertex_stack.size() > 0)
			{
				m_last_decision_vertex_stack.top().m_remaining_out_degree--;
				if(m_last_decision_vertex_stack.top().m_remaining_out_degree == 0)
				{
					// We've completed terminating all branches emanating from this decision statement.

					// We now have all the info needed to create a new Merge vertex.
					// Tell the caller what to do to add this Merge vertex.

					dlog_cfg << "INSERT MERGE VERTEX: "
							<< "\n IF=" << m_last_decision_vertex_stack.top().m_u
							<< "\n EDGES:";
					BOOST_FOREACH(T_EDGE_DESC me, m_last_decision_vertex_stack.top().m_merging_edges)
					{
						dlog_cfg << " " << me;
						mi.m_terminal_edges.push_back(me);
					}
					dlog_cfg << std::endl;

					// Pop the decision node stack to the previous decision vertex.
					m_last_decision_vertex_stack.pop();


				}
				else
				{

					break;
				}
			}

			if(mi.m_terminal_edges.size() > 0)
			{
				// We have something to return.
				// First duplicate the first entry to the end.
				mi.m_terminal_edges.push_back(*(mi.m_terminal_edges.begin()));
				m_returned_merge_info->push_back(mi);
			}
		}
	};

	void discover_vertex(T_VERTEX_DESC u, const GraphType &g)
	{
		if(g[u].m_statement->IsDecisionStatement())
		{
			// Found a Decision statement, push a new context.
			std::cout << "Decision node " << u << " " << g[u].m_statement->GetIdentifierCFG() << std::endl;
			DecisionVertexStackEntry se;
			se.m_u = u;
			se.m_initial_out_degree = filtered_out_degree(u, g);
			se.m_remaining_out_degree = se.m_initial_out_degree;
			m_last_decision_vertex_stack.push(se);
		}
#if 0
		T_OUT_EDGE_ITERATOR ei, eend;

		// Get a pair of iterators over the out edges of this node.
		boost::tie(ei, eend) = boost::out_edges(u, g);

		for(; ei != eend; ++ei)
		{
			T_VERTEX_DESC terminal_target;
			terminal_target = boost::target(*ei, g);
			if(terminal_target==u)
			{
				// This is a self-edge.
				continue;
			}

			StatementBase *sb;
			sb = g[terminal_target].m_statement;

			if(dynamic_cast<Merge*>(sb) != NULL)
			{
				// This is already a merge node, don't add another one in front of it.
				continue;
			}

			// If the in degree of the target > 1, u is a vertex which ends a branch.
			long indeg = boost::in_degree(terminal_target, g);//filtered_in_degree(terminal_target, g);
			if(indeg > 1)
			{
				// Ends the branch.
				assert(m_last_decision_vertex_stack.size() > 0);
				m_last_decision_vertex_stack.top().m_merging_edges.push_back(*ei);

				dlog_cfg << "FOUND BRANCH-TERMINATING EDGE=" << *ei
						<< "\n IF=" << m_last_decision_vertex_stack.top().m_u
						<< "\n OUT DEGREE=" << m_last_decision_vertex_stack.top().m_remaining_out_degree
						<< std::endl;

				MergeInsertionInfo mi;

				while(m_last_decision_vertex_stack.size() > 0)
				{
					m_last_decision_vertex_stack.top().m_remaining_out_degree--;
					if(m_last_decision_vertex_stack.top().m_remaining_out_degree == 0)
					{
						// We've completed terminating all branches emanating from this decision statement.

						// We now have all the info needed to create a new Merge vertex.
						// Tell the caller what to do to add this Merge vertex.

						dlog_cfg << "INSERT MERGE VERTEX: "
								<< "\n IF=" << m_last_decision_vertex_stack.top().m_u
								<< "\n EDGES:";
						BOOST_FOREACH(T_EDGE_DESC e, m_last_decision_vertex_stack.top().m_merging_edges)
						{
							dlog_cfg << " " << e;
							mi.m_terminal_edges.push_back(e);
						}
						dlog_cfg << std::endl;

						// Pop the decision node stack to the previous decision vertex.
						m_last_decision_vertex_stack.pop();

						/// @TODO Need to do something here to fill in the second edge of the now-top decision vertex.
					}
					else
					{

						break;
					}
				}

				if(mi.m_terminal_edges.size() > 0)
				{
					// We have something to return.
					// First duplicate the first entry to the end.
					mi.m_terminal_edges.push_back(*(mi.m_terminal_edges.begin()));
					m_returned_merge_info->push_back(mi);
				}
			}
		}
#endif
	};

private:

	long filtered_in_degree(T_VERTEX_DESC v, const GraphType &cfg)
	{
		T_IN_EDGE_ITERATOR ieit, ieend;

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
	};

	long filtered_out_degree(T_VERTEX_DESC v, const GraphType &cfg)
	{
		T_OUT_EDGE_ITERATOR eit, eend;

		boost::tie(eit, eend) = boost::out_edges(v, cfg);

		long i = 0;
		for (; eit != eend; ++eit)
		{
			if(v == boost::target(*eit, cfg))
			{
				// Skip any self edges.
				continue;
			}

			if (cfg[*eit].m_edge_type->IsBackEdge())
			{
				// Skip anything marked as a back edge.
				continue;
			}

			i++;
		}
		return i;
	};

	struct DecisionVertexStackEntry
	{
		// The Decision statement which needs the merge node.
		T_VERTEX_DESC m_u;
		long m_initial_out_degree;
		long m_remaining_out_degree;
		std::vector<T_EDGE_DESC> m_merging_edges;
	};

	std::stack<DecisionVertexStackEntry> m_last_decision_vertex_stack;

	std::vector<MergeInsertionInfo> *m_returned_merge_info;

};

#endif /* MERGENODEINSERTIONVISITOR_H */
