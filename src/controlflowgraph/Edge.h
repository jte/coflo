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

#ifndef EDGE_H_
#define EDGE_H_

#include <cstddef>

class Vertex;

/*
 *
 */
class Edge
{
public:
	Edge();
	Edge(Vertex *source, Vertex *target);
	virtual ~Edge();

	Vertex* Source() const { return m_source; };
	Vertex* Target() const { return m_target; };

	void ChangeSource(Vertex *source);
	void ChangeTarget(Vertex *target);

	void ClearSourceAndTarget();
	void SetSourceAndTarget(Vertex *source, Vertex *target);
	void SetSource(Vertex *source);
	void SetTarget(Vertex *target);

	/// @todo Friend this only to DescriptorBaseClass.
	/// This is for the use of the DescriptorBaseClass.
	std::size_t GetIndex() const { return m_edge_index; };

private:
	Vertex *m_source;
	Vertex *m_target;

	// This Edge's index.
	std::size_t m_edge_index;
};

#endif /* EDGE_H_ */
