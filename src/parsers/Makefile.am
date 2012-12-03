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

if !HAVE_DPARSER_EXTERNAL_TARBALL
# Use the distributed DParser distribution.
USE_MAKE_DPARSER = $(top_builddir)/third_party/d/make_dparser$(EXEEXT)
else
# Use the system DParser installation.
USE_MAKE_DPARSER = $(PROG_ABSPATH_MAKE_DPARSER)
endif

# The cpp files generated from the .dparser.g files.
DPARSER_BUILTSRC = $(srcdir)/gcc_gimple_parser.cpp

# Make sure to distribute the parser built source.
# @todo Currently we need to have DParser installed in order to build (need its library), so there's
# not much point in distributing the .cpp files it generates.  We'll put this back once we figure out how to
# avoid the configure-time dependency on an installed DParser.
EXTRA_DIST = $(DPARSER_BUILTSRC)

# The generated parser .cpp files should not normally be cleaned.  Make them maintainer-clean files.
# @todo Put this back once we figure out how to avoid the configure-time dependency on an installed DParser.  For now we're
# always building them, so they go in MOSTLYCLEANFILES.
#MAINTAINERCLEANFILES = $(srcdir)/gcc_gimple_parser.cpp
MOSTLYCLEANFILES = $(DPARSER_BUILTSRC)

#BUILT_SOURCES = $(DPARSER_BUILTSRC)
noinst_LIBRARIES = libparsers.a 
libparsers_a_SOURCES = parser_grammar_helper.h gcc_gimple_parser.dparser.g gcc_gimple_parser.h
libparsers_a_CPPFLAGS = -I $(top_srcdir)/src $(DPARSER_CPPFLAGS) -I $(top_srcdir)/src/debug_utils \
	$(BOOST_TR1_CPPFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS)
libparsers_a_CFLAGS = -Wno-missing-field-initializers $(AM_CFLAGS)
libparsers_a_CXXFLAGS = -Wno-missing-field-initializers -Wno-unused-parameter $(AM_CXXFLAGS)

# Path to the make_dparser executable is PROG_ABSPATH_MAKE_DPARSER,
# which is set at configure time.
#PROG_ABSPATH_MAKE_DPARSER = $(top_builddir)/third_party/make_dparser
# Flags for make_dparser.
MAKE_DPARSER_FLAGS = -v -d -d --ext=cpp
# -v -v -d -d <<- These work too well.
#--verbose=2 --debug=2 <<- These don't work.

# Suffix for DParser grammars.
SUFFIXES = .dparser.g .dparser.g.gdebug

# Rule to make *.cpp from *.dparser.g using DParser.
# Notes:
# - make_dparser as of at least 1.29 can only include grammar fragments relative to the current directory or absolute.
#   Since we can't know where we'll be building, we have to cd into the source dir and use absolute paths to make the
#   grammar includes work.
# - The output file (*.cpp) is built source, but we don't want the user to have to have dparser installed,
# so it is created under $(srcdir) and gets distributed.
# - @TODO Actually, at the moment we're going to require the user to have DParser, so we won't do either.
# - The intermediate file in the rule below, *.g.gdebug, is generated under $(bindir) so as not to cause "make distcheck" to fail.
.dparser.g.cpp:
	set -e; rm -f '$@'; \
	BASENAME="$$(basename "$<" .dparser.g)" && \
	TARGET="$$(basename "$@")" && \
	SOURCE="$$(basename "$<")" && \
	INTER="$$(basename "$<").gdebug" && \
	"$(USE_MAKE_DPARSER)" $(MAKE_DPARSER_FLAGS) --grammar_ident=$${BASENAME} "$(srcdir)/$${SOURCE}" \
		--output="$(builddir)/$${INTER}" && \
	"$(SED)" -r -e 's/\(void\*\)([a-zA-Z_][a-zA-Z0-9_]+)/static_cast<void*>\(\1\)/g' "$(builddir)/$${INTER}" > "$(builddir)/$${TARGET}-temp" && \
	mv "$(builddir)/$${TARGET}-temp" "$(srcdir)/$${TARGET}"
	:;# cd "$(srcdir)" && 

# Generate dependencies.
#.dparser.g.dparser.g.d:
#	set -e; rm -f '$@' && \
#	BASENAME="$$(basename "$<" .dparser.g)" && \
#	echo -n "$${BASENAME}.cpp $@: $< " > '$@-temp' && \
#	$(EGREP) '^include[ ]+' '$<' | \
#	tr '\n' ' ' | \
#	"$(SED)" -r 's,include[ ]+,,g' | \
#	"$(SED)" -r 's,\"([^\"]*)\",\"\$$\(srcdir\)/\1\",g' >> '$@-temp' && \
#	echo; echo '\t
#	mv '$@-temp' '$@'
#
#-include $(libparsers_a_SOURCES:.dparser.g=.dparser.g.d)



