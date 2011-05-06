# Second-level Makefile.am for CoFlo
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

bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Block.cpp Block.h \
	CFGEdgeTypeBase.cpp CFGEdgeTypeBase.h \
	CFGEdgeTypeFallthrough.cpp CFGEdgeTypeFallthrough.h \
	CFGEdgeTypeFunctionCall.cpp CFGEdgeTypeFunctionCall.h \
	Function.cpp Function.h \
	FunctionCall.cpp FunctionCall.h \
	FunctionCallResolved.cpp FunctionCallResolved.h \
	FunctionCallUnresolved.cpp FunctionCallUnresolved.h \
	If.cpp If.h \
	Location.cpp Location.h \
	NoOp.cpp NoOp.h \
	Program.cpp Program.h \
	RuleBase.cpp RuleBase.h \
	RuleReachability.cpp RuleReachability.h \
	Statement.cpp Statement.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	Switch.cpp Switch.h \
	TranslationUnit.cpp Translation.h

nobase_check_SCRIPTS = test1.sh

# Tests to run for "make check".
TESTS_ENVIRONMENT = export "TEST_CPPFLAGS=$(CPPFLAGS)" && export "TEST_CXXFLAGS=$(CXXFLAGS)" && $(SHELL) -x
TESTS = test1.sh

coflo_CPPFLAGS = $(BOOST_CPPFLAGS)
coflo_LDFLAGS = $(BOOST_LDFLAGS) -Wl,--enable-auto-import
coflo_LDADD = $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
