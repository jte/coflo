# Second-level Makefile.am for CoFlo, ./cppunit_tests directory.
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

SUBDIRS = controlflowgraph

bin_PROGRAMS = coflotest
coflotest_SOURCES = coflotest.cpp
coflotest_CPPFLAGS = -I $(abs_top_builddir)/third_party/dparser/local/include -I $(abs_top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS) 
coflotest_CFLAGS = $(CPPUNIT_CFLAGS) $(AM_CFLAGS)
coflotest_CXXFLAGS = $(AM_CXXFLAGS)
coflotest_LDFLAGS = $(BOOST_LIBTOOL_FLAGS) $(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(AM_LDFLAGS)
coflotest_LDADD = ./controlflowgraph/libcontrolflowgraphtest.a \
	$(abs_top_builddir)/src/controlflowgraph/algorithms/libalgorithms.a \
	$(abs_top_builddir)/src/controlflowgraph/analysis/libanalysis.a \
	$(abs_top_builddir)/src/controlflowgraph/libcontrolflowgraph.a \
	$(abs_top_builddir)/src/controlflowgraph/statements/libstatements.a \
	$(abs_top_builddir)/src/controlflowgraph/edges/libedges.a \
	$(abs_top_builddir)/src/controlflowgraph/visitors/libvisitors.a \
	$(abs_top_builddir)/src/debug_utils/libdebugutils.a \
	$(abs_top_builddir)/src/libexttools/libexttools.a \
	$(top_builddir)/third_party/dparser/local/lib/libdparse.a \
	$(CPPUNIT_LIBS) $(ALLBOOSTLIBS)
