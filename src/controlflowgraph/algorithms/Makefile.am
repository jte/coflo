# Non-root Makefile.am for CoFlo, /CoFlo/src/controlflowgraph/algorithms/Makefile.am directory.
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

noinst_LIBRARIES = libalgorithms.a
libalgorithms_a_SOURCES = \
	cfg_algs.cpp cfg_algs.h \
	depth_first_traversal.hpp \
	topological_visit_kahn.h
	
	
libalgorithms_a_CPPFLAGS = -I $(top_srcdir)/src/controlflowgraph -I $(top_srcdir)/src/debug_utils $(AM_CPPFLAGS)
libalgorithms_a_CFLAGS = $(AM_CFLAGS)
libalgorithms_a_CXXFLAGS = $(AM_CXXFLAGS)

