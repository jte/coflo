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

# Subdirectories with their own Makefile{.am}'s.
# @todo Do we need something like the nobase_ stuff here: <http://sources.redhat.com/automake/automake.html#Alternative>
# for the controlflowgraph directory?
SUBDIRS = libexttools

bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Analyzer.cpp Analyzer.h \
	BasicBlockGraph.h \
	Block.cpp Block.h \
	ControlFlowGraph.h \
	controlflowgraph/CFGEdgeTypeBase.cpp controlflowgraph/CFGEdgeTypeBase.h \
	controlflowgraph/CFGEdgeTypeFunctionCallBypass.cpp controlflowgraph/CFGEdgeTypeFunctionCallBypass.h \
	controlflowgraph/CFGEdgeTypeFallthrough.cpp controlflowgraph/CFGEdgeTypeFallthrough.h \
	controlflowgraph/CFGEdgeTypeFunctionCall.cpp controlflowgraph/CFGEdgeTypeFunctionCall.h \
	controlflowgraph/CFGEdgeTypeGoto.cpp controlflowgraph/CFGEdgeTypeGoto.h \
	controlflowgraph/CFGEdgeTypeGotoBackEdge.cpp controlflowgraph/CFGEdgeTypeGotoBackEdge.h \
	controlflowgraph/CFGEdgeTypeReturn.cpp controlflowgraph/CFGEdgeTypeReturn.h \
	Entry.cpp Entry.h \
	Exit.cpp Exit.h \
	Function.cpp Function.h \
	FunctionCall.cpp FunctionCall.h \
	FunctionCallResolved.cpp FunctionCallResolved.h \
	FunctionCallUnresolved.cpp FunctionCallUnresolved.h \
	If.cpp If.h \
	Location.cpp Location.h \
	NoOp.cpp NoOp.h \
	Program.cpp Program.h \
	PseudoStatement.cpp PseudoStatement.h \
	RuleBase.cpp RuleBase.h \
	RuleDFSBase.cpp RuleDFSBase.h \
	RulePrintFunctionCFG.cpp RulePrintFunctionCFG.h \
	RuleReachability.cpp RuleReachability.h \
	Statement.cpp Statement.h \
	Successor.cpp Successor.h \
	SuccessorTypes.h \
	Switch.cpp Switch.h \
	TranslationUnit.cpp Translation.h
	
coflo_CPPFLAGS = $(BOOST_CPPFLAGS)
coflo_LDFLAGS = $(BOOST_LDFLAGS)
coflo_LDADD = ./libexttools/libexttools.a $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
