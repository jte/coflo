# Third-level Makefile.am for CoFlo, ./src/parsers directory.
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


# Make sure to distribute the parser built source.
# @todo Currently we need to have DParser installed in order to build (need its library), so there's
# not much point in distributing the .cpp files it generates.  We'll put this back once we figure out how to
# avoid the configure-time dependency on an installed DParser.
#EXTRA_DIST = $(srcdir)/gcc_gimple_parser.cpp

# The generated parser .cpp files should not normally be cleaned.  Make them maintainer-clean files.
# @todo Put this back once we figure out how to avoid the configure-time dependency on an installed DParser.  For now we're
# always building them, so they go in MOSTLYCLEANFILES.
#MAINTAINERCLEANFILES = $(srcdir)/gcc_gimple_parser.cpp
MOSTLYCLEANFILES = gcc_gimple_parser.cpp gcc_gimple_parser.g.gdebug

noinst_LIBRARIES = libparsers.a 
libparsers_a_SOURCES = gcc_gimple_parser.g gcc_gimple_parser.h
libparsers_a_CPPFLAGS = -I $(top_srcdir)/src -I $(top_builddir)/third_party/dparser/local/include -I $(top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS)
libparsers_a_CFLAGS = $(AM_CFLAGS)
libparsers_a_CXXFLAGS = $(AM_CXXFLAGS)

# Path to the make_dparser executable.
PROG_ABSPATH_MAKE_DPARSER = $(top_builddir)/third_party/make_dparser
# Flags for make_dparser.
DPARSER_FLAGS = --verbose=2 --debug --ext=cpp

# Suffix for DParser grammars.
SUFFIX = .g

# Rule to make *.cpp from *.g using DParser.
# Note:
# - The output file (*.cpp) is built source, but we don't want the user to have to have dparser installed,
# so it is created under $(srcdir) and gets distributed.
# - @TODO Actually, at the moment we're going to require the user to have DParser, so we won't do either.
# - The intermediate file in the rule below, *.g.gdebug, is generated under $(bindir) so as not to cause "make distcheck" to fail.
.g.cpp:
	BASENAME="$$(basename "$<" .g)" && \
	TARGET="$$(basename "$@")" && \
	SOURCE="$(srcdir)/$$(basename "$<")" && \
	INTER="$$(basename "$<").gdebug" && \
	"$(PROG_ABSPATH_MAKE_DPARSER)" $(DPARSER_FLAGS) --grammar_ident=$${BASENAME} "$${SOURCE}" --output="$${INTER}" && \
	"$(SED)" -r -e 's/\(void\*\)([a-zA-Z_][a-zA-Z0-9_]+)/static_cast<void*>\(\1\)/g' "$${INTER}" > "$${TARGET}-temp" && \
	mv "$${TARGET}-temp" "$${TARGET}"
