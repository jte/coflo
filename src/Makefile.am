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

# Make sure to distribute the Doxygen configuration file and the parser source.
EXTRA_DIST = Doxyfile $(srcdir)/gcc_cfg_parser.g

# The generated parser .cpp files should not normally be cleaned.  Make them maintainer-clean files. 
MAINTAINERCLEANFILES = $(srcdir)/gcc_cfg_parser.g.d_parser.cpp

bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Analyzer.cpp Analyzer.h \
	BasicBlockGraph.h \
	Block.cpp Block.h \
	Function.cpp Function.h \
	Location.cpp Location.h \
	Program.cpp Program.h \
	ResponseFileParser.cpp ResponseFileParser.h \
	RuleBase.cpp RuleBase.h \
	RuleDFSBase.cpp RuleDFSBase.h \
	RuleReachability.cpp RuleReachability.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	TranslationUnit.cpp TranslationUnit.h \
	safe_enum.h
	
# \	gcc_cfg_parser.g.d_parser.cpp

coflo_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS)
coflo_CFLAGS = $(AM_CXXFLAGS)
coflo_CXXFLAGS = $(AM_CXXFLAGS)
coflo_LDFLAGS = $(BOOST_LDFLAGS)
coflo_LDADD = ./controlflowgraph/libcontrolflowgraph.a \
	./controlflowgraph/statements/libstatements.a \
	./controlflowgraph/edges/libedges.a \
	./controlflowgraph/visitors/libvisitors.a \
	./debug_utils/libdebugutils.a \
	./libexttools/libexttools.a \
	$(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
	
#	/usr/local/lib/libdparse.a \#

# Rule to make gcc_cfg_parser.g.d_parser.cpp
# Note that the output file is built source, but we don't want the user to have to have dparser installed,
# so it gets distributed.
$(srcdir)/gcc_cfg_parser.g.d_parser.cpp: $(srcdir)/gcc_cfg_parser.g
	 make_dparser --ext=cpp "$<"



