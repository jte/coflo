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

#ifndef VERTEX_H_
#define VERTEX_H_

#include <boost/unordered_set.hpp>
#include <boost/any.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <utility>

#include "VertexID.h"

class Graph;
//class Edge;
#include "Edge.h"

typedef Edge* EdgeDescriptor;


struct EdgeDescriptorConv
{
	EdgeDescriptor operator()(Edge* e) const { return EdgeDescriptor(e); };

	/// This is for boost::result_of().
	typedef EdgeDescriptor result_type;
};


/**
 *
 */
class Vertex
{
public:
	typedef boost::unordered_set< Edge* > edge_list_type;
	typedef edge_list_type::iterator base_edge_list_iterator;
	typedef boost::transform_iterator<EdgeDescriptorConv, base_edge_list_iterator, EdgeDescriptor, EdgeDescriptor> edge_iterator;
	typedef boost::transform_iterator<EdgeDescriptorConv, base_edge_list_iterator, EdgeDescriptor, EdgeDescriptor> out_edge_iterator;
	typedef boost::transform_iterator<EdgeDescriptorConv, base_edge_list_iterator, EdgeDescriptor, EdgeDescriptor> in_edge_iterator;
	typedef edge_list_type::size_type degree_size_type;

public:
	/**
	 * Default vertex constructor.
	 */
	Vertex();
	/**
	 * Copy constructor.
	 * @param other
	 */
	Vertex(const Vertex& other);
	/**
	 * Destructor.
	 */
	virtual ~Vertex();

	void CopyFrom(Vertex *other);

	void TransferOwnedResourcesTo(Vertex *other);

	/**
	 * Return this Vetex's ID.
	 *
	 * @return
	 */
	//VertexID GetID() const;

	/**
	 * Get the vertex's index.  The index is guaranteed to be unique to the containing Graph.
	 * Note that Vertex's don't have indexes until they're added to a Graph.
	 */
	std::size_t GetIndex() const { return m_vertex_index; };

	void AddInEdge(Edge *e);
	void RemoveInEdge(Edge *e);
	void AddOutEdge(Edge *e);
	void RemoveOutEdge(Edge *e);

	/**
	 * Get a begin() and end() iterator over all in edges.
	 *
	 * @return
	 */
	std::pair<Vertex::in_edge_iterator, Vertex::in_edge_iterator> InEdges();
	std::pair<Vertex::out_edge_iterator, Vertex::out_edge_iterator> OutEdges();

	template <typename Predicate>
	std::pair<boost::filter_iterator<Predicate,Vertex::in_edge_iterator>, boost::filter_iterator<Predicate,Vertex::in_edge_iterator> >
	InEdges()
	{
		typedef boost::filter_iterator<Predicate,Vertex::in_edge_iterator> T_RETVAL_ITER;
		typedef std::pair<T_RETVAL_ITER, T_RETVAL_ITER> T_RETVAL;
		T_RETVAL retval;

		retval = InEdges();

		return retval;
	};

	template <typename Predicate>
	std::pair<boost::filter_iterator<Predicate,Vertex::out_edge_iterator>, boost::filter_iterator<Predicate,Vertex::out_edge_iterator> >
	OutEdges()
	{
		typedef boost::filter_iterator<Predicate,Vertex::out_edge_iterator> T_RETVAL_ITER;
		typedef std::pair<T_RETVAL_ITER, T_RETVAL_ITER> T_RETVAL;
		T_RETVAL retval;

		retval = OutEdges();

		return retval;
	}

	degree_size_type InDegree() { return m_in_edges.size(); };
	degree_size_type OutDegree() { return m_out_edges.size(); };


	Edge* FindOutEdgePointingToVertex(const Vertex *target);

	/// Allow the Graph class access to m_vertex_index.
	friend class Graph;

	/// @todo Friend this only to DescriptorBaseClass.
	/// This is for the use of the DescriptorBaseClass.
	std::size_t GetDescriptorIndex() const { return m_vertex_index; };

private:
	void SetVertexIndex(VertexID vertex_index);

	edge_iterator MakeIterator(base_edge_list_iterator i);

protected:

	Vertex::edge_list_type m_out_edges;
	Vertex::edge_list_type m_in_edges;

	// This Vertex's index.
	std::size_t m_vertex_index;

};

#endif /* VERTEX_H_ */
