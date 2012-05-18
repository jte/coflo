# Second-level Makefile.am for CoFlo, ./src directory.
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

# Subdirectories with their own Makefile{.am}'s.
SUBDIRS = controlflowgraph debug_utils libexttools parsers

# Make sure to distribute the Doxygen configuration file.
EXTRA_DIST = Doxyfile 

COMMONSOURCES = Function.cpp Function.h \
	Location.cpp Location.h \
	Program.cpp Program.h \
	ResponseFileParser.cpp ResponseFileParser.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	TranslationUnit.cpp TranslationUnit.h \
	UEI.cpp UEI.h \
	safe_enum.h

# The Automake rules for the CoFlo executable.
bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp $(COMMONSOURCES)

# Collect all the Boost libraries we found into a single variable.
if BOOST_USE_BUILT_BOOST
BOOST_LOCAL_LIB = $(top_builddir)/third_party/$(BOOST_TARBALL_DIRNAME)/stage/lib
ALLBOOSTLIBS = $(BOOST_LOCAL_LIB)/libboost_graph.a \
	$(BOOST_LOCAL_LIB)/libboost_program_options.a \
	$(BOOST_LOCAL_LIB)/libboost_regex.a \
	$(BOOST_LOCAL_LIB)/libboost_system.a \
	$(BOOST_LOCAL_LIB)/libboost_filesystem.a
else
# Boost libs determined by Autoconf Macro Archive macros.
ALLBOOSTLIBS = $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
# Boost libs determined by boost.m4.
#ALLBOOSTLIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_FILESYSTEM_LIBS)
endif

NORMALLIBS = ./controlflowgraph/algorithms/libalgorithms.a \
	./parsers/libparsers.a \
	./controlflowgraph/analysis/libanalysis.a \
	./controlflowgraph/libcontrolflowgraph.a \
	./controlflowgraph/statements/libstatements.a \
	./controlflowgraph/edges/libedges.a \
	./controlflowgraph/visitors/libvisitors.a \
	./debug_utils/libdebugutils.a \
	./libexttools/libexttools.a \
	$(top_builddir)/third_party/dparser/local/lib/libdparse.a

coflo_CPPFLAGS = -I $(top_builddir)/third_party/dparser/local/include -I $(top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS) 
coflo_CFLAGS = $(AM_CFLAGS)
coflo_CXXFLAGS = $(AM_CXXFLAGS)
# Note that the "BOOST_<lib>_LDFLAGS" are used only by boost.m4, not the Autoconf Macro Achive macros,
# so they'll evaluate to empty when we're using the latter.
coflo_LDFLAGS = $(BOOST_LIBTOOL_FLAGS) $(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(AM_LDFLAGS)
coflo_LDADD = $(NORMALLIBS) $(ALLBOOSTLIBS)


###
### Automake rules for the coflotest unit test executable.
###

TESTLIBS = ./controlflowgraph/libcontrolflowgraphtest.a

# For Google Test, we need the following to be defined by configure:
# - GTEST_ROOT: The directory of source distribution.

gtest-all.cc: $(GTEST_ROOT)/src/gtest-all.cc
	cp $< $@

BUILT_SOURCES = gtest-all.cc
MOSTLYCLEANFILES = gtest-all.cc
check_PROGRAMS = coflotest
coflotest_SOURCES = coflotest.cpp $(COMMONSOURCES)
nodist_coflotest_SOURCES = gtest-all.cc
coflotest_CPPFLAGS = -I $(GTEST_ROOT)/include -I $(GTEST_ROOT) -I $(top_builddir)/third_party/dparser/local/include -I $(top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS) 
coflotest_CFLAGS = $(CPPUNIT_CFLAGS) $(AM_CFLAGS)
coflotest_CXXFLAGS = $(AM_CXXFLAGS)
coflotest_LDFLAGS = $(BOOST_LIBTOOL_FLAGS) $(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(AM_LDFLAGS)
coflotest_LDADD = $(TESTLIBS) $(NORMALLIBS) $(CPPUNIT_LIBS) $(ALLBOOSTLIBS)
