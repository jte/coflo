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
SUBDIRS = controlflowgraph debug_utils libexttools

# Make sure to distribute the Doxygen configuration file and the parser built source.
EXTRA_DIST = Doxyfile 
# @todo Currently we need to have DParser installed in order to build (need its library), so there's
# not much point in distributing the .cpp files it generates.  We'll put this back once we figure out how to
# avoid the configure-time dependency on an installed DParser.
#$(srcdir)/gcc_gimple_parser.cpp

# The generated parser .cpp files should not normally be cleaned.  Make them maintainer-clean files.
# @todo Put this back once we figure out how to avoid the configure-time dependency on an installed DParser.  For now we're
# always building them, so they go in MOSTLYCLEANFILES.
#MAINTAINERCLEANFILES = $(srcdir)/gcc_gimple_parser.cpp
MOSTLYCLEANFILES = gcc_gimple_parser.cpp gcc_gimple_parser.g.gdebug

# The Automake rules for the CoFlo executable.
bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Function.cpp Function.h \
	Location.cpp Location.h \
	Program.cpp Program.h \
	ResponseFileParser.cpp ResponseFileParser.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	TranslationUnit.cpp TranslationUnit.h \
	UEI.cpp UEI.h \
	safe_enum.h \
	\
	gcc_gimple_parser.g gcc_gimple_parser.h
	
# Path to the make_dparser executable.
PROG_ABSPATH_MAKE_DPARSER = $(abs_top_builddir)/third_party/make_dparser
# Flags for make_dparser.
DPARSER_FLAGS = --verbose=2 --debug --ext=cpp --grammar_ident=gcc_gimple_parser

# Collect all the Boost libraries we found into a single variable.
if BOOST_USE_BUILT_BOOST
BOOST_LOCAL_LIB = $(abs_top_builddir)/third_party/boost_1_48_0/stage/lib
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

coflo_CPPFLAGS = -I $(abs_top_builddir)/third_party/dparser/local/include -I $(abs_top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS) 
coflo_CFLAGS = $(AM_CFLAGS)
coflo_CXXFLAGS = $(AM_CXXFLAGS)
# Note that the "BOOST_<lib>_LDFLAGS" are used only by boost.m4, not the Autoconf Macro Achive macros,
# so they'll evaluate to empty when we're using the latter.
coflo_LDFLAGS = $(BOOST_LIBTOOL_FLAGS) $(BOOST_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(AM_LDFLAGS)
coflo_LDADD = ./controlflowgraph/analysis/libanalysis.a \
	./controlflowgraph/libcontrolflowgraph.a \
	./controlflowgraph/statements/libstatements.a \
	./controlflowgraph/edges/libedges.a \
	./controlflowgraph/visitors/libvisitors.a \
	./debug_utils/libdebugutils.a \
	./libexttools/libexttools.a \
	$(top_builddir)/third_party/dparser/local/lib/libdparse.a \
	$(ALLBOOSTLIBS)

# Suffix for DParser grammars.
SUFFIX = .g

# Rule to make *.cpp from *.g using DParser.
# Note:
# - The output file (*.cpp) is built source, but we don't want the user to have to have dparser installed,
# so it is created under $(srcdir) and gets distributed.
# - @TODO Actually, at the moment we're going to require the user to have DParser, so we won't do either.
# - The intermediate file in the rule below, *.g.gdebug, is generated under $(bindir) so as not to cause "make distcheck" to fail.
.g.cpp:
	TARGET="$$(basename "$@")" && \
	SOURCE="$(srcdir)/$$(basename "$<")" && \
	INTER="$$(basename "$<").gdebug" && \
	"$(PROG_ABSPATH_MAKE_DPARSER)" $(DPARSER_FLAGS) "$${SOURCE}" --output="$${INTER}" && \
	"$(SED)" -r -e 's/\(void\*\)([a-zA-Z_][a-zA-Z0-9_]+)/static_cast<void*>\(\1\)/g' "$${INTER}" > "$${TARGET}-temp" && \
	mv "$${TARGET}-temp" "$${TARGET}"

