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

#ifndef GRAPHADAPTER_H
#define GRAPHADAPTER_H

#include "Graph.h"
#include "coflo_exceptions.hpp"

/*
namespace boost {
  typedef Graph ugraph;
  typedef const Graph ugraph_const;

  struct Graph_traversal_tag :
    public virtual vertex_list_graph_tag,
    public virtual incidence_graph_tag,
    public virtual adjacency_graph_tag { };

  template <> struct graph_traits<Graph> {
    typedef Graph::vertex_descriptor vertex_descriptor;
    typedef Graph::edge_descriptor edge_descriptor;
    typedef Graph::out_edge_iterator out_edge_iterator;
    typedef Graph::in_edge_iterator in_edge_iterator;
    typedef void adjacency_iterator;
    typedef Graph::vertex_iterator vertex_iterator;
    typedef Graph::edge_iterator edge_iterator;
    typedef long vertices_size_type;
    typedef long edges_size_type;
    typedef long degree_size_type;
    typedef directed_tag directed_category;
    typedef Graph_traversal_tag traversal_category;
    typedef allow_parallel_edge_tag edge_parallel_category;
  };
  template <> struct graph_traits<const Graph> {
    typedef Graph::vertex_descriptor vertex_descriptor;
    typedef Graph::edge_descriptor edge_descriptor;
    typedef Graph::out_edge_iterator out_edge_iterator;
    typedef Graph::in_edge_iterator in_edge_iterator;
    typedef void adjacency_iterator;
    typedef Graph::vertex_iterator vertex_iterator;
    typedef Graph::edge_iterator edge_iterator;
    typedef long vertices_size_type;
    typedef long edges_size_type;
    typedef long degree_size_type;
    typedef directed_tag directed_category;
    typedef Graph_traversal_tag traversal_category;
    typedef allow_parallel_edge_tag edge_parallel_category;
  };
}
*/

/// @name Free-function declarations for adapting this graph class to the Boost graph library.
//@{
//namespace boost
//{
	inline Graph::vertex_descriptor target(const Graph::edge_descriptor &e, const Graph &/*g*/)
	{
		return VertexDescriptor(e->Target());
	};

	inline Graph::vertex_descriptor source(const Graph::edge_descriptor &e, const Graph &/*g*/)
	{
		return VertexDescriptor(e->Source());
	};

	inline Graph::degree_size_type out_degree(Graph::vertex_descriptor u, const Graph& /*g*/)
	{
		return u->OutDegree();
	};

	inline Graph::degree_size_type in_degree(Graph::vertex_descriptor u, const Graph& /*g*/)
	{
		return u->InDegree();
	};

	inline std::pair<Graph::out_edge_iterator, Graph::out_edge_iterator> out_edges(Graph::vertex_descriptor u, const Graph &/*g*/)
	{
		return u->OutEdges();
	};

	inline std::pair<Graph::in_edge_iterator, Graph::in_edge_iterator> in_edges(Graph::vertex_descriptor u, const Graph &/*g*/)
	{
		return u->InEdges();
	};

	inline std::pair<Graph::vertex_iterator, Graph::vertex_iterator> vertices(const Graph& g)
	{
		std::pair<Graph::vertex_iterator, Graph::vertex_iterator> retval;

		g.Vertices(&retval);

		return retval;
	};

	inline Graph::vertices_size_type num_vertices(const Graph& g)
	{
		return g.NumVertices();
	};

	// VertexIndexGraphConcept requires this.
	inline void renumber_vertex_indices(Graph &g) {};

	/// Vertex index (vertex_index_t) property map.
	class Graph_vertex_index_map
	{
	public:
		typedef boost::readable_property_map_tag category;
		typedef std::size_t value_type;
		typedef std::size_t reference;
		typedef Graph::vertex_descriptor key_type;
		Graph_vertex_index_map() /*: m_g(0)*/ { };
		Graph_vertex_index_map(const Graph& /*g*/) /*: m_g(&g)*/ { }
		Graph_vertex_index_map(const Graph_vertex_index_map& /*other*/) {};
		//reference operator[](const key_type v) const { return (*v)->GetVertexIndex(); };
	protected:
		//const Graph* m_g;
	};

	inline Graph_vertex_index_map::reference get(const Graph_vertex_index_map& /*pmap*/,  const Graph_vertex_index_map::key_type& key)
	{
		return key->GetVertexIndex();
	};

	inline Graph_vertex_index_map get(boost::vertex_index_t, Graph& g)
	{
		return Graph_vertex_index_map(g);
	};

	inline Graph_vertex_index_map get(boost::vertex_index_t, const Graph& g)
	{
		return Graph_vertex_index_map(g);
	};

	inline long get(boost::vertex_index_t, const Graph& /*g*/, Graph::vertex_descriptor v)
	{
		return v->GetVertexIndex();
	};

namespace boost
{
	/// Property map traits specializations.
	template <>
	struct property_map<Graph, boost::vertex_index_t>
	{
		typedef Graph_vertex_index_map type;
		typedef const Graph_vertex_index_map const_type;
	};
	template <>
	struct property_map<const Graph, boost::vertex_index_t>
	{
		typedef const Graph_vertex_index_map const_type;
	};
};

	/// @name Free functions for implementing the MutableGraph concept.
	//@{

	/**
	 * Create a new Vertex and add it to Graph @a g.
	 *
	 * @param g The instance of Graph to add the new Vertex to.
	 * @return Descriptor of the new vertex.
	 */
	inline Graph::vertex_descriptor add_vertex(Graph& g)
	{
		/// @todo We probably need to use the "virtual constructor" pattern here.
		Vertex *v = new Vertex();

		g.AddVertex(v);

		return VertexDescriptor(v);
	};

	inline std::pair<Graph::edge_descriptor, bool> add_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g)
	{
		Edge *e = new Edge();

		g.AddEdge(u, v, e);

		/// @todo From the BGL docs: "If the graph disallows parallel edges, and the edge (u,v) is already in the graph,
		/// then the bool flag returned is false and the returned edge descriptor points to the already existing edge.".

		return std::make_pair(e, true);
	};

	inline void remove_edge(Graph::vertex_descriptor u, Graph::vertex_descriptor v, Graph &g)
	{
		BOOST_THROW_EXCEPTION( not_implemented() );
	};

	inline void remove_edge(Graph::edge_descriptor e, Graph &g)
	{
		BOOST_THROW_EXCEPTION( not_implemented() );
	};

	inline void clear_vertex(Graph::vertex_descriptor u, Graph &g)
	{
		BOOST_THROW_EXCEPTION( not_implemented() );
	};

	inline void remove_vertex(Graph::vertex_descriptor u, Graph &g)
	{
		BOOST_THROW_EXCEPTION( not_implemented() );
	};
	//@}

	/// @name Free functions for implementing the EdgeListGraph concept.
	//@{
	std::pair<Graph::edge_iterator, Graph::edge_iterator> edges(const Graph& g);
	Graph::edges_size_type num_edges(const Graph& g);

	//@}


//}
//@}

/**
 * Concept checker function for the Graph class.
 * This is templatized so that it never actually gets instantiated, hence wasting space in the resulting binary.
 *
 * @note Don't try to instantiate or use this function.  It's solely here to check concepts associated with Graph,
 * which are done at compile time.
 */
template <typename Dummy>
void Graph_concept_checker()
{
	// Is our vertex index map a ReadablePropertyMap?
	BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyMapConcept<const Graph_vertex_index_map, const Graph::vertex_descriptor> ));
	// Is our vertex index map a WritablePropertyMap?
	//BOOST_CONCEPT_ASSERT(( boost::WritablePropertyMapConcept<boost::Graph_vertex_id_map, Graph::vertex_descriptor> ));
	// Is our vertex index map a ReadWritePropertyMap?
	//BOOST_CONCEPT_ASSERT(( boost::ReadWritePropertyMapConcept<boost::Graph_vertex_id_map, Graph::vertex_descriptor> ));

	// Check iterators.
	BOOST_CONCEPT_ASSERT(( boost::DefaultConstructibleConcept< Graph::vertex_descriptor > ));
    BOOST_CONCEPT_ASSERT(( boost::EqualityComparableConcept< Graph::vertex_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::AssignableConcept<Graph::vertex_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::DefaultConstructibleConcept<Graph::edge_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::EqualityComparableConcept<Graph::edge_descriptor> ));
	BOOST_CONCEPT_ASSERT(( boost::AssignableConcept<Graph::edge_descriptor> ));

	// Is our Graph class a Boost Graph?
	BOOST_CONCEPT_ASSERT(( boost::GraphConcept<Graph> ));

	// Need to model IncidenceGraph for depth_first_search.
	// According to the docs, we need to model VertexListGraph too, but the code doesn't appear to need it.
	BOOST_CONCEPT_ASSERT(( boost::IncidenceGraphConcept<Graph> ));
	// Is our Graph class a ReadablePropertyGraph for vertex_index_t?
	BOOST_CONCEPT_ASSERT(( boost::ReadablePropertyGraphConcept<Graph, Graph::vertex_descriptor, boost::vertex_index_t> ));

	// Need to model VertexListGraph for a number of algorithms.
	BOOST_CONCEPT_ASSERT(( boost::VertexListGraphConcept<Graph> ));

	// Need to model EdgeListGraph for write_graphviz().
	BOOST_CONCEPT_ASSERT(( boost::EdgeListGraphConcept<Graph> ));

	// Is our Graph class a PropertyGraph for vertex_color_t?
	//BOOST_CONCEPT_ASSERT(( boost::PropertyGraphConcept<Graph, Graph::vertex_descriptor, boost::vertex_color_t> ));
	BOOST_CONCEPT_ASSERT(( boost::VertexIndexGraphConcept<Graph> ));

	BOOST_CONCEPT_ASSERT(( boost::VertexMutableGraphConcept<Graph> ));
	BOOST_CONCEPT_ASSERT(( boost::EdgeMutableGraphConcept<Graph> ));
	BOOST_CONCEPT_ASSERT(( boost::MutableGraphConcept<Graph> ));
};

#endif
