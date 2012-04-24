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

/** @file */

#include "Edge.h"

#include "Vertex.h"

Edge::Edge()
{
	ClearSourceAndTarget();
}

Edge::Edge(Vertex* source, Vertex* target)
{
	SetSourceAndTarget(source, target);
}

Edge::~Edge()
{
	// TODO Auto-generated destructor stub
}


void Edge::ChangeSource(Vertex* source)
{
	if(m_source != NULL)
	{
		// This Edge is already has a source Vertex.  Remove it from that Vertex's out edge list.
		m_source->RemoveOutEdge(this);
	}

	// Add this edge to the new source Vertex.
	source->AddOutEdge(this);
	m_source = source;
}

void Edge::ChangeTarget(Vertex* target)
{
	if(m_target != NULL)
	{
		// This Edge is already has a source Vertex.  Remove it from that Vertex's out edge list.
		m_target->RemoveInEdge(this);
	}

	// Add this edge to the new source Vertex.
	target->AddInEdge(this);
	m_target = target;
}

void Edge::SetSourceAndTarget(Vertex* source, Vertex* target)
{
	/// @todo Handle the case where the edge is already pointed at a source and target.

	m_source = source;
	m_target = target;
}

void Edge::ClearSourceAndTarget()
{
	m_source = NULL;
	m_target = NULL;
}

void Edge::SetSource(Vertex* source)
{
	m_source = source;
}

void Edge::SetTarget(Vertex* target)
{
	m_target = target;
}



