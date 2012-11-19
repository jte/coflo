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

#ifndef STATEMENTBASE_H
#define	STATEMENTBASE_H

#include <string>
#include <utility>

#include <boost/iterator/transform_iterator.hpp>

#include "../../debug_utils/debug_utils.hpp"
#include "../../Location.h"
#include "../Vertex.h"
//#include "../Edge.h"

class Function;
//class CFGEdgeTypeBase;
#include "../edges/CFGEdgeTypeBase.h"

/// Edge descriptor type for ControlFlowGraphs.
typedef CFGEdgeTypeBase* CFGEdgeDescriptor;

/**
 * Functor for use by the various edge iterators of StatementBase to convert iterators which dereference to
 * EdgeDescriptors into iterators which dereference to CFGEdgeDescriptors.
 */
struct CFGEdgeDescriptorConv
{
	CFGEdgeDescriptor operator()(Edge* e) const { return CFGEdgeDescriptor(e); };

	/// This is for boost::result_of().
	typedef CFGEdgeDescriptor result_type;
};

/**
 * Abstract base class for all statements and expressions in the control flow graph.
 */
class StatementBase : public Vertex
{
public:
	typedef boost::transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator, CFGEdgeDescriptor, CFGEdgeDescriptor> edge_iterator;
	typedef boost::transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator, CFGEdgeDescriptor, CFGEdgeDescriptor > out_edge_iterator;
	typedef boost::transform_iterator< CFGEdgeDescriptorConv, Vertex::base_edge_list_iterator, CFGEdgeDescriptor, CFGEdgeDescriptor > in_edge_iterator;

public:
	StatementBase() { m_owning_function = NULL; };
	explicit StatementBase(const Location &location);
	StatementBase(const StatementBase& orig);
	virtual ~StatementBase();
	
	virtual void InEdges(StatementBase::in_edge_iterator* ibegin, StatementBase::in_edge_iterator* iend);
	virtual void OutEdges(StatementBase::out_edge_iterator* ibegin, StatementBase::out_edge_iterator* iend);

	template < typename EdgeType >
	EdgeType* GetFirstOutEdgeOfType()
	{
		out_edge_iterator eit, eend;
		EdgeType* retval;

		OutEdges(&eit, &eend);
		for(; eit != eend; eit++)
		{
			retval = dynamic_cast<EdgeType*>(*eit);
			if(NULL != retval)
			{
				// Found it.
				return retval;
			}
		}

		// Couldn't find one.
		retval = NULL;
		return retval;
	};

	void SetOwningFunction(Function *owning_function);
	Function* GetOwningFunction() const;

	void SetLocation(const Location &new_location) { m_location = new_location; };

	/**
	 * Get text suitable for setting the statement's attributes in a dot file.
	 *
     * @return 
     */
	virtual std::string GetStatementTextDOT() const = 0;
	
	/**
	 * Get text representing this StatementBase which is suitable for
	 * printing in a textual CFG printout.
	 * 
     * @return std::string containing the identifier text.
     */
	virtual std::string GetIdentifierCFG() const = 0;
	
	/**
	 * Returns a string suitable for use in a Dot "color=" node border attribute.
	 * 
	 * The SVG color scheme is assumed.  Defaults to "black" if this function
	 * is not overridden in derived classes.
	 * 
     * @return A DOT color for the node's border.
     */
	virtual std::string GetDotSVGColor() const { return "black"; };

	virtual std::string GetShapeTextDOT() const { return "rectangle"; };

	/**
	 * Returns the Location corresponding to this StatementBase.
	 * 
     * @return The Location of the StatementBase.
     */
	const Location GetLocation() const { return m_location; };

	
	/// @name Functions for returning info about the statement.
	//@{

	/**
	 * Is this statement a decision statement, e.g. an if() or a switch(), which will
	 * naturally have more than one out edge?
	 *
	 * @return true if this StatementBase is a decision statement.
	 */
	virtual bool IsDecisionStatement() const { return false; };

	/**
	 * Is this statement a function call, either resolved or unresolved?
	 *
	 * @return true if the statement is a function call, false if it isn't.
	 */
	virtual bool IsFunctionCall() const { return false; };

	/**
	 * When called like "IsType<SomeDerivedType>()", returns whether it's dynamic_castable to
	 * that type or not.
	 *
	 * @deprecated This is one step removed from switch/case.  At the moment this exists for the
	 * benefit of function_control_flow_graph_visitor, but there's got to be a better way to do it.
	 *
	 * @return
	 */
	template<typename DerivedType>
	bool IsType() const { return NULL != dynamic_cast<const DerivedType*>(this); };

	//@}

	/**
	 * Utility function for escaping literal "\n" and double quotes in a string so that they
	 * can be used in a dot label.
	 *
	 * @param str
	 * @return
	 */
	static std::string EscapeifyForUseInDotLabel(const std::string &str);

private:

	/// The Location of this statement.
	Location m_location;

	/// The Function this statement belongs to.
	Function *m_owning_function;
};


#endif	/* STATEMENTBASE_H */

