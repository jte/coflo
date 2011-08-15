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
SUBDIRS = controlflowgraph debug libexttools statements

# Make sure the Doxygen configuration file is distributed.
EXTRA_DIST = Doxyfile

bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Analyzer.cpp Analyzer.h \
	BasicBlockGraph.h \
	Block.cpp Block.h \
	CFGDFSVisitor.cpp CFGDFSVisitor.h \
	ControlFlowGraph.cpp ControlFlowGraph.h \
	Function.cpp Function.h \
	ImprovedDFSVisitorBase.h \
	Location.cpp Location.h \
	Program.cpp Program.h \
	RuleBase.cpp RuleBase.h \
	RuleDFSBase.cpp RuleDFSBase.h \
	RulePrintFunctionCFG.cpp RulePrintFunctionCFG.h \
	RuleReachability.cpp RuleReachability.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	TranslationUnit.cpp TranslationUnit.h \
	safe_enum.h

coflo_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS)
coflo_CFLAGS = $(AM_CXXFLAGS)
coflo_CXXFLAGS = $(AM_CXXFLAGS)
coflo_LDFLAGS = $(BOOST_LDFLAGS)
coflo_LDADD = ./controlflowgraph/libcontrolflowgraph.a ./debug/libdebugutils.a ./libexttools/libexttools.a ./statements/libstatements.a $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
