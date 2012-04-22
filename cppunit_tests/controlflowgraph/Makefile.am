# Third-level Makefile.am for CoFlo, ./cppunit_tests/controlflowgraph directory.
#
# Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
#
# This file is part of CoFlo.
#
# CoFlo is free software: you can redistribute it and/or modify it under the
# terms of version 3 of the GNU General Public License as published by the Free
# Software Foundation.
#
# CoFlo is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# CoFlo.  If not, see <http://www.gnu.org/licenses/>.

#SUBDIRS = algorithms analysis edges statements visitors

noinst_LIBRARIES = libcontrolflowgraphtest.a
libcontrolflowgraphtest_a_SOURCES = \
	GraphTest.cpp GraphTest.h
#SparsePropertyMap.h \#
#CallStackBase.cpp CallStackBase.h \#
#	CallStackFrameBase.cpp CallStackFrameBase.h \#
#	ControlFlowGraphTraversalBase.cpp ControlFlowGraphTraversalBase.h \#
#	ControlFlowGraphTraversalDFS.cpp ControlFlowGraphTraversalDFS.h \#
#	Edge.cpp Edge.h \#
#	Graph.cpp Graph.h \#
#	Vertex.cpp Vertex.h \#
#	VertexID.cpp VertexID.h

#	ControlFlowGraphTemplateHelper.h \#
#	FilteredGraph.cpp FilteredGraph.h \#

libcontrolflowgraphtest_a_CPPFLAGS = -I $(abs_top_srcdir)/src/controlflowgraph -I $(abs_top_srcdir)/src/debug_utils $(AM_CPPFLAGS)
libcontrolflowgraphtest_a_CFLAGS = $(AM_CFLAGS)
libcontrolflowgraphtest_a_CXXFLAGS = $(AM_CXXFLAGS)
