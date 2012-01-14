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

#ifndef CFGEDGETYPEBASE_H
#define	CFGEDGETYPEBASE_H

#include <string>
#include "coflo_exceptions.hpp"

/**
 * Base class for control flow graph edge types.
 */
class CFGEdgeTypeBase
{

public:
	CFGEdgeTypeBase();
	/**
	 *  Copy constructor.
	 *
	 *  @param orig Reference to the CFGEdgeTypeBase object to copy.
	 */
	CFGEdgeTypeBase(const CFGEdgeTypeBase& orig);
	/**
	 *  Destructor.
	 *  Pure virtual to force this to be an abstract base class.
	 */
	virtual ~CFGEdgeTypeBase() = 0;
	
	/**
	 * Mark the edge as a back edge.  We keep this info around because we need it
	 * for doing certain operations such as topological sorting, and for efficiency
	 * reasons - both design and run-rime - we want to just determine this once.
	 * 
     * @param is_back_edge true if the edge has been determined to be a back edge.
     */
	void MarkAsBackEdge(bool is_back_edge) { m_is_back_edge = is_back_edge; };
	
	/// @name Informational Predicates
	//@{

	/**
	 * Indicates if this is a back edge in the control flow graph, as would be
	 * caused by a loop.
	 * 
     * @return true if this is a back edge, false if it isn't.
     */
	bool IsBackEdge() const { return m_is_back_edge; };
	
	/**
	 * Indicates whether this is a normal (as opposed to an Exceptional or Impossible) edge.
	 *
	 * @return true if this edge is a normal edge.
	 */
	virtual bool IsNormal() const { BOOST_THROW_EXCEPTION(not_implemented()); return false; };
	virtual bool IsExceptional() const { BOOST_THROW_EXCEPTION(not_implemented()); return false; };
	virtual bool IsImpossible() const { return false; };

	//@}

	/**
	 * Returns a string suitable for printing as the edge's label.
	 * E.g., "true" or "false" for out edges of an if() statement.
	 *
	 * Unless overridden in derived classes, returns an empty string.
	 *
     * @return A string suitable for printing as the edge's label.
     */
	virtual std::string GetLabel() const { return std::string(); };

	/**
	 * Returns a string suitable for use as an edge label in a Dot graph.
	 * 
	 * Unless overridden in derived classes, returns an empty string.
	 * 
     * @return A string suitable for use as an edge label in a Dot graph.
     */
	virtual std::string GetDotLabel() const { return std::string(); };
	
	/**
	 * Returns a string suitable for use in a Dot "color=" edge attribute.
	 * 
	 * The SVG color scheme is assumed.  Defaults to "black" if this function
	 * is not overridden in derived classes.
	 * 
     * @return 
     */
	virtual std::string GetDotSVGColor() const { return "black"; };
	
	/**
	 * Returns a string suitable for use in a Dot "style=" edge attribute.
	 * 
     * @return 
     */
	std::string GetDotStyle() const;

private:
	
	/// Flag indicating whether this edge has been determined to be a back edge or not.
	bool m_is_back_edge;

};

#endif	/* CFGEDGETYPEBASE_H */
