# Third-level Makefile.am for CoFlo
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


noinst_LIBRARIES = libstatements.a
libstatements_a_SOURCES = statements.h \
	AssignmentBase.cpp AssignmentBase.h \
	Entry.cpp Entry.h \
    Exit.cpp Exit.h \
    FlowControlBase.cpp FlowControlBase.h \
    FunctionCall.cpp FunctionCall.h \
    FunctionCallResolved.cpp FunctionCallResolved.h \
    FunctionCallUnresolved.cpp FunctionCallUnresolved.h \
    Goto.cpp Goto.h \
    If.cpp If.h \
    Label.cpp Label.h \
    Merge.cpp Merge.h \
    NoOp.cpp NoOp.h \
    ParseHelpers.cpp ParseHelpers.h \
    Placeholder.cpp Placeholder.h \
    PseudoStatement.cpp PseudoStatement.h \
    StatementBase.cpp StatementBase.h \
    Switch.cpp Switch.h

libstatements_a_CPPFLAGS = -I $(abs_top_srcdir)/src/debug_utils $(AM_CPPFLAGS)
libstatements_a_CFLAGS = $(AM_CFLAGS)
libstatements_a_CXXFLAGS = $(AM_CXXFLAGS)
