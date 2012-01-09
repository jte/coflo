# Third-level Makefile.am for CoFlo, ./controlflowgraph/edges directory.
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

noinst_LIBRARIES = libedges.a
libedges_a_SOURCES = \
	edge_types.h \
	CFGEdgeTypeBase.cpp CFGEdgeTypeBase.h \
	CFGEdgeTypeExceptional.cpp CFGEdgeTypeExceptional.h \
	CFGEdgeTypeFunctionCallBypass.cpp CFGEdgeTypeFunctionCallBypass.h \
	CFGEdgeTypeFallthrough.cpp CFGEdgeTypeFallthrough.h \
	CFGEdgeTypeFunctionCall.cpp CFGEdgeTypeFunctionCall.h \
	CFGEdgeTypeGoto.cpp CFGEdgeTypeGoto.h \
	CFGEdgeTypeIfTrue.cpp CFGEdgeTypeIfTrue.h \
	CFGEdgeTypeIfFalse.cpp CFGEdgeTypeIfFalse.h \
	CFGEdgeTypeImpossible.cpp CFGEdgeTypeImpossible.h \
	CFGEdgeTypeReturn.cpp CFGEdgeTypeReturn.h

# Propagate any AM_*FLAGS to the per-target flags.
# We need to do this because per the Automake manual, "In compilations with per-target flags,
# the ordinary ‘AM_’ form of the flags variable is not automatically included in the compilation
# (however, the user form of the variable is included)."
libedges_a_CPPFLAGS = -I $(abs_top_srcdir)/src/debug_utils $(AM_CPPFLAGS)
libedges_a_CFLAGS = $(AM_CFLAGS)
libedges_a_CXXFLAGS = $(AM_CXXFLAGS)
