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

#SUFFIXES = .html .css template_html.cpp template_css.cpp

# The template source files.  These will be converted to C++ arrays and compiled into the binary.
templates = index.template.html css/index.template.css

intermediate_cpp_files = index.template.html.cpp css/index.template.css.cpp
BUILT_SOURCES = $(intermediate_cpp_files)
EXTRA_DIST = $(templates)

noinst_LIBRARIES = libtemplates.a
# The .cpp files below are all built sources.
libtemplates_a_SOURCES = templates.h FileTemplate.h FileTemplate.cpp \
	 $(intermediate_cpp_files)

# Propagate any AM_*FLAGS to the per-target flags.
# We need to do this because per the Automake manual, "In compilations with per-target flags,
# the ordinary "AM_" form of the flags variable is not automatically included in the compilation
# (however, the user form of the variable is included)."
libtemplates_a_CPPFLAGS = $(AM_CPPFLAGS)
libtemplates_a_CFLAGS = $(AM_CFLAGS)
libtemplates_a_CXXFLAGS = $(AM_CXXFLAGS)

# We have to manually register the built sources for cleaning.
MOSTLYCLEANFILES = $(intermediate_cpp_files)

# The template files.
#nobase_dist_pkgdata_DATA = css/index.css
#
#css/index.css: $(srcdir)/templates/css/index.template.css Makefile
#	rm -f $@ $@.tmp
#	$(MKDIR_P) templates/css
#	srcdir=''; \
#		test -f ./templates/css/index.template.css || srcdir=$(srcdir)/; \
#       cp $${srcdir}templates/css/index.template.css $@.tmp
#	mv $@.tmp $@

# Makefile action to turn any input file into a file containing a '0'-terminated C++ const char array.
recipie_any_file_to_cpp_array = rm -f "$@.tmp" && \
	RELATIVE_SOURCE_NAME=$$(realpath --relative-to=$(srcdir) "$<") && \
	:;{ \
	echo "// Generated file, do not edit." && \
	echo "// File generated $$(date)" && \
	echo "extern const char "$$(echo $${RELATIVE_SOURCE_NAME} | sed 's,[/ \\\.],_,'g)"[] = {" ; \
	} > "$@.tmp" && \
	$(XXD) -i < "$<" >> "$@.tmp" && \
	echo ', 0 };' >> "$@.tmp" && \
	mv "$@.tmp" "$${RELATIVE_SOURCE_NAME}.cpp"

	
index.template.html.cpp: $(srcdir)/index.template.html
	$(recipie_any_file_to_cpp_array)
	
css/index.template.css.cpp: $(srcdir)/css/index.template.css
	$(recipie_any_file_to_cpp_array)
