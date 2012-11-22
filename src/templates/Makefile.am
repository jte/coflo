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

SUFFIXES = .js .js.cpp .html .html.cpp .css .css.cpp .tar.cpp

# The template source files.  These will be converted to C++ arrays and compiled into the binary.
templates = index.template.html \
	css/index.template.css \
	js/coflo.resizer.js

# The .cpp files below are all built sources.
intermediate_cpp_files = index.template.html.cpp \
	css/index.template.css.cpp \
	js/coflo.resizer.js.cpp
	
noinst_LIBRARIES = libtemplates.a
libtemplates_a_SOURCES = templates.h FileTemplate.h FileTemplate.cpp $(intermediate_cpp_files)
libtemplates_a_LIBADD = $(builddir)/report_boilerplate.tar.$(OBJEXT)

# Note that we don't list the template files in the _SOURCES Automake variable, even though they are sources
# for the libtemplates.a library.  If we do so, Automake generates dependency tracking files (*.Po) in the source tree
# for them.  I haven't been able to find a better workaround. 
EXTRA_DIST = $(templates)

# Propagate any AM_*FLAGS to the per-target flags.
# We need to do this because per the Automake manual, "In compilations with per-target flags,
# the ordinary "AM_" form of the flags variable is not automatically included in the compilation
# (however, the user form of the variable is included)."
libtemplates_a_CPPFLAGS = $(AM_CPPFLAGS)
libtemplates_a_CFLAGS = $(AM_CFLAGS)
libtemplates_a_CXXFLAGS = $(AM_CXXFLAGS)

# We have to manually register the built sources for cleaning.
MOSTLYCLEANFILES = $(intermediate_cpp_files:%.cpp=$(srcdir)/%.cpp) report_boilerplate.tar.cpp

recipie_any_file_to_cpp_array = : First order of business is to remove any leftover temp files. && \
rm -f "$@.tmp" && \
: Generate the name we will use for the C++ array and length variable. && \
: The grep extracts the filename and at most one preceeding directory, while the sed converts non-C-identifier characters. && \
ARRAY_IDENTIFIER="$$(echo "$<" | grep -E -o "([^/]+/){0,$${MAX_DIRS}}([^/]+)$$" | $(SED) 's,[/ \\\.-],_,'g)" && \
: Create the file and add the boilerplate at the top. && \
:;{ \
echo "// Generated file, do not edit." && \
echo "// File generated $$(date)" ; \
} > "$@-temp" && \
: Generate the file, fixing up the identifier names in the process. && \
$(XXD) -i "$<" | \
	$(SED) -r "s,(unsigned char )([^ \[]+),extern const unsigned char $${ARRAY_IDENTIFIER}," | \
	$(SED) -r "s,(unsigned int )([^ =]+),\1$${ARRAY_IDENTIFIER}_len," | \
	$(SED) -r 's/};/, 0x00};/' >> "$@-temp" && \
mv "$@-temp" "$@"


# Rule to convert *.css files to *.css.cpp files.
.css.css.cpp:
	MAX_DIRS=1 && $(recipie_any_file_to_cpp_array)

# Rule to convert *.html files to *.html.cpp files.
.html.html.cpp:
	MAX_DIRS=0 && $(recipie_any_file_to_cpp_array)
	
# Rule to convert *.js files to *.js.cpp files.
.js.js.cpp:
	MAX_DIRS=1 && $(recipie_any_file_to_cpp_array)

# The boilerplate JavaScript and CSS files from the third_party directory.
report_boilerplate.tar.cpp: $(top_builddir)/third_party/report_boilerplate.tar
	MAX_DIRS=0 && $(recipie_any_file_to_cpp_array)

