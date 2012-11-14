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

SUFFIXES = .js .js.cpp .html .html.cpp .css .css.cpp .cpio .cpio.cpp

jquery_ui_layout_dir = $(top_srcdir)/third_party/jquery-ui-layout-plugin

# The template source files.  These will be converted to C++ arrays and compiled into the binary.
templates = index.template.html \
	css/index.template.css \
	js/coflo.resizer.js \
	js/jquery.layout-latest.js \
	js/jquery.jstree.js \
	js/jquery-1.8.2.js \
	js/jquery-ui.js

#intermediate_cpp_files_1 = $(templates:%.html=%.html.cpp)
#intermediate_cpp_files_2 = $(intermediate_cpp_files_1:%.css=%.css.cpp)
#intermediate_cpp_files = $(intermediate_cpp_files_2:%.js=%.js.cpp)

intermediate_cpp_files = index.template.html.cpp \
	css/index.template.css.cpp \
	js/coflo.resizer.js.cpp \
	js/jquery.layout-latest.js.cpp \
	js/jquery.jstree.js.cpp \
	js/jquery-1.8.2.js.cpp \
	js/jquery-ui.js.cpp
	
# Files that we'll need to tell Automake to clean.
intermediate_clean_files =  css_dark_hive.cpio  css_dark_hive.cpio.cpp jquery_ui_layout.cpio jquery_ui_layout.cpio.cpp

test1:
	echo $(intermediate_cpp_files)
	echo "2:"
	echo $(intermediate_cpp_files)
	echo "3"
	echo $(intermediate_cpp_files:%.cpp=$(srcdir)/%.cpp)

noinst_LIBRARIES = libtemplates.a
# The .cpp files below are all built sources.
libtemplates_a_SOURCES = templates.h FileTemplate.h FileTemplate.cpp $(intermediate_cpp_files)
#EXTRA_libtemplates_a_SOURCES = $(top_builddir)/third_party/css_dark_hive.cpio $(top_builddir)/third_party/jquery_ui_layout.cpio
libtemplates_a_LIBADD = $(builddir)/css_dark_hive.cpio.$(OBJEXT) $(builddir)/jquery_ui_layout.cpio.$(OBJEXT)

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
## @todo This should really be maintainer clean.
MOSTLYCLEANFILES = $(intermediate_cpp_files:%.cpp=$(srcdir)/%.cpp) $(intermediate_clean_files)

define test555 =
echo "Hello: $${TARGET_NAME} $@"
endef

test56:
	TARGET_NAME="gjhg" && $(test555)

recipie_any_file_to_cpp_array = \
	@: First order of business is to remove any leftover temp files. && \
	rm -f "$@.tmp" && \
	RELATIVE_SOURCE_NAME="$$(realpath --relative-to=$(srcdir) "$<")" && \
	: Generate the name we will use for the C++ array and length variable. && \
	: The grep extracts the filename and at most one preceeding directory, while the sed converts non-C-identifier characters. && \
	ARRAY_IDENTIFIER="$$(echo $${RELATIVE_SOURCE_NAME} | grep -E -o '([^/]+/)?([^/]+)$$' | sed 's,[/ \\\.-],_,'g)" && \
	: Create the file and add the boilerplate at the top. && \
	:;{ \
	echo "// Generated file, do not edit." && \
	echo "// File generated $$(date)" ; \
	} > "$@.tmp" && \
	: Generate the file, fixing up the identifier names in the process. && \
	$(XXD) -i "$<" | \
		sed -r "s,(unsigned char )([^ \[]+),extern const unsigned char $${ARRAY_IDENTIFIER}," | \
		sed -r "s,(unsigned int )([^ =]+),extern \1$${ARRAY_IDENTIFIER}_len," | \
		sed -r 's/};/, 0x00};/' >> "$@.tmp" && \
	mv "$@.tmp" "$(srcdir)/$${RELATIVE_SOURCE_NAME}.cpp"

# Rule to convert *.css files to *.css.cpp files.
.css.css.cpp:
	$(recipie_any_file_to_cpp_array)

# Rule to convert *.html files to *.html.cpp files.
.html.html.cpp:
	$(recipie_any_file_to_cpp_array)
	
# Rule to convert *.js files to *.js.cpp files.
.js.js.cpp:
	$(recipie_any_file_to_cpp_array)
	
.cpio.cpio.cpp:
	$(recipie_any_file_to_cpp_array)

css_dark_hive.cpio.cpp: css_dark_hive.cpio
	$(recipie_any_file_to_cpp_array)
	
jquery_ui_layout.cpio.cpp: jquery_ui_layout.cpio
	$(recipie_any_file_to_cpp_array)

# Some of the sources we need are in the third_party subdirectory.
# We'll copy them to source directories under this $(srcdir) if needed so we can use that same build
# mechanisms for all of the template files.

# Makefile action for copying a file only if it is different from the target.
# Leverages "move-if-change". 
recipie_copy_if_change = TEMPFILE_NAME="$$(mktemp)" && \
	cp -p "$<" "$${TEMPFILE_NAME}" && \
	"$(top_srcdir)/build-aux/move-if-change" "$${TEMPFILE_NAME}" "$@"

#$(builddir)/css/layout-default-latest.css: $(top_srcdir)/third_party/jquery-ui-layout-plugin/layout-default-latest.css
#	$(recipie_copy_if_change)

$(srcdir)/js/jquery.jstree.js: $(top_srcdir)/third_party/jstree_pre1.0_fix_1/jquery.jstree.js
	$(recipie_copy_if_change)

$(srcdir)/js/jquery.layout-latest.js: $(top_srcdir)/third_party/jquery-ui-layout-plugin/jquery.layout-latest.js
	$(recipie_copy_if_change)

$(srcdir)/js/jquery-1.8.2.js: $(top_srcdir)/third_party/jquery/jquery-1.8.2.js
	$(recipie_copy_if_change)

$(srcdir)/js/jquery-ui.js: $(top_srcdir)/third_party/jquery-ui-1.9.1/ui/jquery-ui.js
	$(recipie_copy_if_change)

css_dark_hive.cpio: $(top_builddir)/third_party/css_dark_hive.cpio
	$(recipie_copy_if_change)

jquery_ui_layout.cpio: $(top_builddir)/third_party/jquery_ui_layout.cpio
	$(recipie_copy_if_change)
