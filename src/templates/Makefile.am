# Third-level Makefile.am for CoFlo, ./src/template directory.
#
# Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#EXTRA_DIST = index.template.html

noinst_LIBRARIES = libtemplates.a
# The .cpp files below are all built sources.
libtemplates_a_SOURCES = templates.h \
	index.template.html.cpp
	
# Propagate any AM_*FLAGS to the per-target flags.
# We need to do this because per the Automake manual, "In compilations with per-target flags,
# the ordinary "AM_" form of the flags variable is not automatically included in the compilation
# (however, the user form of the variable is included)."
libtemplates_a_CPPFLAGS = $(AM_CPPFLAGS)
libtemplates_a_CFLAGS = $(AM_CFLAGS)
libtemplates_a_CXXFLAGS = $(AM_CXXFLAGS)

MOSTLYCLEANFILES = $(srcdir)/index.template.html.cpp

SUFFIXES = .html.cpp .html

.html.html.cpp:
	rm -f "$@.tmp"
	@ :;{ \
	echo "// Generated file, do not edit." && \
	echo "// File generated $$(date)" && \
	echo "const char "$$(basename "$@" | sed 's/\./_/'g)"[] = {" ; \
	} > "$@.tmp"
	$(XXD) -i < "$<" >> "$@.tmp"
	echo ', 0 };' >> "$@.tmp"
	mv "$@.tmp" "$@"
