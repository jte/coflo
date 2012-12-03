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

# For Google Test, we need the following to be defined by configure:
# - GTEST_ROOT: The directory of source distribution.
#GTEST_ROOT = $(top_builddir)/third_party/gtest-1.6.0/fused-src

# Subdirectories with their own Makefile{.am}'s.
SUBDIRS = controlflowgraph debug_utils libexttools parsers templates

if !HAVE_DPARSER_EXTERNAL_TARBALL
# Use the distributed DParser distribution.
USE_DPARSER_LDADD = $(top_builddir)/third_party/d/libdparse.a
USE_DPARSER_CPPFLAGS = -I $(top_builddir)/third_party/d/ 
else
# Use the system DParser installation.
USE_DPARSER_LDADD = $(DPARSER_LDADD)
USE_DPARSER_CPPFLAGS = $(DPARSER_CPPFLAGS)
endif

# Make sure to distribute:
# - The Doxygen configuration file.
# - The /etc/coflo.conf template file.
EXTRA_DIST = Doxyfile coflo.conf.in

# The /etc/coflo.conf file.
dist_sysconf_DATA = coflo.conf

# Source files common to both the normal CoFlo and the coflotest executables.
COMMONSOURCES = Function.cpp Function.h \
	Location.cpp Location.h \
	Program.cpp Program.h \
	ResponseFileParser.cpp ResponseFileParser.h \
	RuntimeConfiguration.cpp RuntimeConfiguration.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	TranslationUnit.cpp TranslationUnit.h \
	UEI.cpp UEI.h \
	safe_enum.h safe_enum.cpp
	
TESTSOURCES = RuntimeConfiguration_test.cpp

# The Automake rules for the CoFlo executable.
bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp $(COMMONSOURCES)

# Collect all the Boost libraries we found into a single variable.
if COFLO_USE_BUILT_BOOST
BOOST_LOCAL_LIB = $(top_builddir)/third_party/$(BOOST_TARBALL_DIRNAME)/stage/lib
ALLBOOSTLIBS = $(BOOST_LOCAL_LIB)/libboost_graph.a \
	$(BOOST_LOCAL_LIB)/libboost_program_options.a \
	$(BOOST_LOCAL_LIB)/libboost_regex.a \
	$(BOOST_LOCAL_LIB)/libboost_system.a \
	$(BOOST_LOCAL_LIB)/libboost_filesystem.a
else
# Boost libs determined by Autoconf Macro Archive macros.
#ALLBOOSTLIBS = $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
# Boost libs determined by boost.m4.
ALLBOOSTLIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_FILESYSTEM_LIBS)
endif

NORMALLIBS = ./controlflowgraph/algorithms/libalgorithms.a \
	./parsers/libparsers.a \
	./controlflowgraph/analysis/libanalysis.a \
	./controlflowgraph/statements/libstatements.a \
	./controlflowgraph/edges/libedges.a \
	./controlflowgraph/visitors/libvisitors.a \
	./controlflowgraph/libcontrolflowgraph.a \
	./debug_utils/libdebugutils.a \
	./libexttools/libexttools.a \
	./templates/libtemplates.a

	
#	$(top_builddir)/third_party/dparser/local/lib/libdparse.a
#-I $(top_builddir)/third_party/dparser/local/include

coflo_CPPFLAGS = $(USE_DPARSER_CPPFLAGS) -I $(top_srcdir)/src/parsers \
	-I $(top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) \
	-DCOFLO_PKGDATA_DIR='$(pkgdatadir)' $(AM_CPPFLAGS) 
coflo_CFLAGS = $(AM_CFLAGS)
coflo_CXXFLAGS = $(AM_CXXFLAGS)
# Note that the "BOOST_<lib>_LDFLAGS" are used only by boost.m4, not the Autoconf Macro Achive macros,
# so they'll evaluate to empty when we're using the latter.
coflo_LDFLAGS = $(BOOST_LIBTOOL_FLAGS) $(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(AM_LDFLAGS)
coflo_LDADD = $(NORMALLIBS) $(USE_DPARSER_LDADD) $(ALLBOOSTLIBS)


###
### Automake rules for the coflotest unit test executable.
###

TESTLIBS = ./controlflowgraph/libcontrolflowgraphtest.a

gtest-all.cpp: $(top_builddir)/third_party/gtest-1.6.0/fused-src/gtest/gtest-all.cc
	cp "$<" "$@"

check_PROGRAMS = coflotest
coflotest_SOURCES = coflotest.cpp $(TESTSOURCES) $(COMMONSOURCES)
nodist_coflotest_SOURCES = gtest-all.cpp
coflotest_CC = $(PTHREAD_CC)
coflotest_CPPFLAGS = -I $(GTEST_ROOT)/gtest -I $(GTEST_ROOT) \
	-I $(top_builddir)/third_party/dparser/local/include -I $(top_srcdir)/src/debug_utils -I $(top_srcdir)/src/parsers \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS) 
coflotest_CFLAGS = $(PTHREAD_CFLAGS) $(AM_CFLAGS)
coflotest_CXXFLAGS = $(PTHREAD_CFLAGS) $(AM_CXXFLAGS)
coflotest_LDFLAGS = $(BOOST_LIBTOOL_FLAGS) $(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(AM_LDFLAGS)
coflotest_LDADD = $(TESTLIBS) $(NORMALLIBS) $(USE_DPARSER_LDADD) $(ALLBOOSTLIBS) $(PTHREAD_LIBS)
