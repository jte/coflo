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

class Vertex;

/*
 *
 */
class Edge
{
public:
	Edge();
	virtual ~Edge();

	Vertex* Source() { return m_source; };
	Vertex* Target() { return m_target; };



private:
	Vertex *m_source;
	Vertex *m_target;
};

#endif /* EDGE_H_ */
