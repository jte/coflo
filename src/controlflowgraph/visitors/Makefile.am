# Non-root Makefile.am for CoFlo, /CoFlo/src/controlflowgraph/visitors/Makefile.am directory.
#
# Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

noinst_LIBRARIES = libvisitors.a
libvisitors_a_SOURCES = \
BackEdgeFixupVisitor.cpp BackEdgeFixupVisitor.h \
	ControlFlowGraphVisitorBase.cpp ControlFlowGraphVisitorBase.h \
	ControlFlowGraphOutputVisitor.cpp ControlFlowGraphOutputVisitor.h \
	MergeNodeInsertionVisitor.cpp MergeNodeInsertionVisitor.h \
	ImprovedDFSVisitorBase.h \
	ReachabilityVisitor.cpp ReachabilityVisitor.h

# Propagate any AM_*FLAGS to the per-target flags.
# We need to do this because per the Automake manual, "In compilations with per-target flags,
# the ordinary ‘AM_’ form of the flags variable is not automatically included in the compilation
# (however, the user form of the variable is included)."
libvisitors_a_CPPFLAGS = -I $(abs_top_srcdir)/src/controlflowgraph -I $(abs_top_srcdir)/src/debug_utils $(AM_CPPFLAGS)
libvisitors_a_CFLAGS = $(AM_CFLAGS)
libvisitors_a_CXXFLAGS = $(AM_CXXFLAGS)
