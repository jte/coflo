# Second-level Makefile.am for CoFlo, ./src directory.
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

# Subdirectories with their own Makefile{.am}'s.
SUBDIRS = controlflowgraph debug_utils libexttools

# Make sure to distribute the Doxygen configuration file and the parser built source.
EXTRA_DIST = Doxyfile 
# @todo Currently we need to have DParser installed in order to build (need its library), so there's
# not much point in distributing the .cpp files it generates.
#$(srcdir)/gcc_gimple_parser.cpp

# The generated parser .cpp files should not normally be cleaned.  Make them maintainer-clean files. 
#MAINTAINERCLEANFILES = $(srcdir)/gcc_gimple_parser.cpp

bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Function.cpp Function.h \
	Location.cpp Location.h \
	Program.cpp Program.h \
	ResponseFileParser.cpp ResponseFileParser.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	TranslationUnit.cpp TranslationUnit.h \
	safe_enum.h \
	\
	gcc_gimple_parser.g gcc_gimple_parser.h
	
# Flags for make_dparser.
DPARSER_FLAGS = --verbose=2 --debug --ext=cpp --grammar_ident=gcc_gimple_parser

coflo_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS)
coflo_CFLAGS = $(AM_CFLAGS)
coflo_CXXFLAGS = $(AM_CXXFLAGS)
coflo_LDFLAGS = $(AM_LDFLAGS) $(BOOST_LDFLAGS)
coflo_LDADD = ./controlflowgraph/analysis/libanalysis.a \
	./controlflowgraph/libcontrolflowgraph.a \
	./controlflowgraph/statements/libstatements.a \
	./controlflowgraph/edges/libedges.a \
	./controlflowgraph/visitors/libvisitors.a \
	./debug_utils/libdebugutils.a \
	./libexttools/libexttools.a \
	$(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

SUFFIX = .g

# Rule to make *.cpp from *.g using DParser.
# Note that the output file is built source, but we don't want the user to have to have dparser installed,
# so it gets distributed.
.g.cpp:
	TARGET="$(srcdir)/$$(basename "$@")" && \
	SOURCE="$(srcdir)/$$(basename "$<")" && \
	INTER="$(srcdir)/$$(basename "$<").gdebug" && \
	"$(PROG_MAKE_DPARSER_ABSPATH)" $(DPARSER_FLAGS) "$${SOURCE}" "--output=$${INTER}" && \
	"$(SED)" -r -e 's/\(void\*\)([a-zA-Z_][a-zA-Z0-9_]+)/static_cast<void*>\(\1\)/g' $${INTER} > $${TARGET}-temp && \
	mv $${TARGET}-temp $${TARGET}

