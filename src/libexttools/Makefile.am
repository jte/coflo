# Second-level Makefile.am for CoFlo, ./libexttools directory.
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

noinst_LIBRARIES = libexttools.a
libexttools_a_SOURCES = ToolBase.cpp ToolBase.h \
    ToolCompiler.cpp ToolCompiler.h \
    ToolDot.cpp ToolDot.h \
    VersionNumber.cpp VersionNumber.h

# Propagate any AM_*FLAGS to the per-target flags.
# We need to do this because per the Automake manual, "In compilations with per-target flags,
# the ordinary ‘AM_’ form of the flags variable is not automatically included in the compilation
# (however, the user form of the variable is included)."
libexttools_a_CPPFLAGS = $(AM_CPPFLAGS)
libexttools_a_CFLAGS = $(AM_CFLAGS)
libexttools_a_CXXFLAGS = $(AM_CXXFLAGS)
