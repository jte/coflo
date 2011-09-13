# Second-level Makefile.am for CoFlo, ./controlflowgraph directory.
#
# Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

SUBDIRS = statements edges

noinst_LIBRARIES = libcontrolflowgraph.a
libcontrolflowgraph_a_SOURCES = \
	BackEdgeFixupVisitor.cpp BackEdgeFixupVisitor.h \
	CFGDFSVisitor.cpp CFGDFSVisitor.h \
	ControlFlowGraph.cpp ControlFlowGraph.h \
	depth_first_traversal.hpp \
	MergeNodeInsertionVisitor.cpp MergeNodeInsertionVisitor.h \
	ImprovedDFSVisitorBase.h \
	topological_visit_kahn.h

libcontrolflowgraph_a_CPPFLAGS = $(AM_CPPFLAGS)
libcontrolflowgraph_a_CFLAGS = $(AM_CXXFLAGS)
libcontrolflowgraph_a_CXXFLAGS = $(AM_CXXFLAGS)
