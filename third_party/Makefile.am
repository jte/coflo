# Second-level Makefile.am for CoFlo, ./third_party directory.
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

# This is a proxy Makefile.am which dispatches the Autotools rules to the real makefiles of
# the third-party libraries/tools we may need.

# Define some file extensions we know how to handle
SUFFIXES = .tar.bz2 .tar.gz .tar

## Much of this directory is conditionalized at configure time, so we define some vars at the top
## which we'll append to later in the sections which are not conditionalized out.

# Directory trees we need to delete on a "make clean".
CLEAN_LOCAL_DIRS = 

# The one file we explicitly "build" is this stampfile.
# This stampfile depends on all files added to $(third_party_complete_deps), which will in turn be the result of
# the various sub-builds. 
BUILT_SOURCES = stampfile-all-third-party-builds-complete

# Append any stamp (or other) files which indicate that a sub-build is complete to this variable.
third_party_complete_deps =

# Extra files we'll need to distribute.
EXTRA_DIST =

# Files to clean for "make mostlyclean" and "make clean".
MOSTLYCLEANFILES =


###
### The Automake rules for building DParser from the upstream tarball.
### The tarball can be either the one distributed with CoFlo, or one specified at configure-time.
###

# Path to the bundled DParser source distribution.
BUNDLED_DPARSER_TARBALL = $(srcdir)/d-1.26-src.tar.gz
BUNDLED_DPARSER_PATCHFILE = $(srcdir)/d-1.26-to-1.29.grvs.patch

# Add the untarred DParser source directory and the local install tree to the "make clean" list. 
CLEAN_LOCAL_DIRS += $(builddir)/d $(DPARSER_PREFIX)

third_party_complete_deps += stamp_dparser_src_installed

EXTRA_DIST += $(BUNDLED_DPARSER_TARBALL) $(BUNDLED_DPARSER_PATCHFILE)

if !HAVE_DPARSER_EXTERNAL_TARBALL
# Specify the DParser tarball in the CoFlo distribution.
FILE_PATH_DPARSER_SOURCE_TARBALL_TO_USE = $(BUNDLED_DPARSER_TARBALL)
DPARSER_PATCHFILE_TO_USE = $(BUNDLED_DPARSER_PATCHFILE)
endif

# DParser directory vars.
DPARSER_PREFIX = ./dparser/local
DPARSER_INCLUDE_DIR = $(DPARSER_PREFIX)/include

# The Automake rules for building dparser's "make_dparser" and static library "libdparse.a" with headers.
# We only need these during the build, so instead of requiring the user to install DParser, we'll build them
# from the DParser source tarball.
noinst_PROGRAMS = make_dparser
make_dparser_SOURCES =
make_dparser_CPPFLAGS = -DUSE_FREELISTS -DD_MAJOR_VERSION=$(MAJOR) -DD_MINOR_VERSION=$(MINOR) -DD_BUILD_VERSION=\"$(D_BUILD_VERSION)\" $(AM_CPPFLAGS)
make_dparser_DEPENDENCIES = stamp_dparser_src_installed stamp_boost_src_installed
make_dparser_LINK = $(INSTALL_PROGRAM) $(builddir)/d/make_dparser$(EXEEXT) $(builddir)/make_dparser$(EXEEXT)

# These headers are only built in the sense that they get untarred into the ./d directory.
LIBDPARSE_BUILT_HEADER_LIST = $(DPARSER_INCLUDE_DIR)/dparse.h \
	$(DPARSER_INCLUDE_DIR)/dparse_tables.h \
	$(DPARSER_INCLUDE_DIR)/dsymtab.h \
	$(DPARSER_INCLUDE_DIR)/dparse_tree.h

MOSTLYCLEANFILES_DPARSER = stamp_dparser_src_built stamp_dparser_src_installed
CLEANFILES_DPARSER = stamp_dparser_src_untarred $(LIBDPARSE_BUILT_HEADER_LIST)

# Untar the DParser source tarball and create a stamp file indicating that we did it.
stamp_dparser_src_untarred: $(FILE_PATH_DPARSER_SOURCE_TARBALL_TO_USE)
	rm -f $@-temp
	@echo "In directory $$(pwd), attempting to untar \"$(FILE_PATH_DPARSER_SOURCE_TARBALL_TO_USE)\""
	@: 'Untar the DParser source distro into our build directory.'
	tar -xaf "$(FILE_PATH_DPARSER_SOURCE_TARBALL_TO_USE)"
	@: 'Apply the patch file.'
	patch -p1 -d ./d -i ../$(DPARSER_PATCHFILE_TO_USE)
	echo "`date -u`" > $@-temp && mv $@-temp $@

# Build the untarred DParser, again creating a stamp file indicating that we completed it.
stamp_dparser_src_built: stamp_dparser_src_untarred
	rm -f $@-temp
	cd d && $(MAKE) $(AM_MAKEFLAGS) EXEEXT=$(EXEEXT) D_BUILD_VERSION=exported all
	echo "`date -u`" > $@-temp && mv $@-temp $@
	
stamp_dparser_src_installed: stamp_dparser_src_built
	rm -f $@-temp
	cd d && $(MAKE) $(AM_MAKEFLAGS) CFLAGS="$(make_dparser_CPPFLAGS)" EXEEXT=$(EXEEXT) D_BUILD_VERSION=exported PREFIX=../$(DPARSER_PREFIX) install
	echo "`date -u`" > $@-temp && mv $@-temp $@


if COFLO_USE_BUILT_BOOST
###
### The Automake rules for building static versions of the Boost libraries we need from an upstream source tarball.
###

# The Boost build-from-tarball process is untarred->configured->built->installed.

# Some Boost lib directory variables.
BOOST_BUILD_DIR = $(abs_builddir)/boost_build_dir
BOOST_LOCAL = ./boost_local
BOOST_STAGE_DIR = $(abs_builddir)/$(BOOST_TARBALL_DIRNAME)/stage
BOOST_LOCAL_LIB = $(abs_builddir)/$(BOOST_TARBALL_DIRNAME)/stage/lib
BOOST_LOCAL_INCLUDE = $(BOOST_LOCAL)/include

BUILTSOURCES_BOOST = stamp_boost_src_installed
MOSTLYCLEANFILES_BOOST = stamp_boost_src_built stamp_boost_src_installed
CLEANFILES_BOOST = stamp_boost_src_configured stamp_boost_src_untarred

# Remove the untarred Boost source tree on "make clean". 
CLEAN_LOCAL_DIRS += $(builddir)/$(BOOST_TARBALL_DIRNAME)

# @todo Determine if this should be single or multi from configure.
COFLO_BOOST_THREADING = single
COFLO_BOOST_BJAM_JOBS = -j$(CPU_COUNT)
COFLO_BOOST_RUNTIME_LINK = shared
COFLO_BOOST_VARIANT = debug

# Split the bunzipping and the untarring into separate rules so we can take better advantage of parallel builds. 
#$(BOOST_TARBALL_TAR): $(FILE_PATH_BOOST_SOURCE_TARBALL)
#	bunzip2 --keep --force "$<"

stamp_boost_src_untarred: $(FILE_PATH_BOOST_SOURCE_TARBALL)
	rm -f $@-temp
	@echo "In directory $$(pwd), attempting to untar \"$<\""
	@: 'Untar the Boost source distro into our build directory.'
	tar -xaf "$<"
	echo "`date -u`" > $@-temp && mv $@-temp $@

stamp_boost_src_configured: stamp_boost_src_untarred
	rm -f $@-temp
	@echo "Building b2 and configuring Boost..."; 
	cd $(BOOST_TARBALL_DIRNAME) && ./bootstrap.sh --prefix="../$(BOOST_LOCAL)" --with-libraries="$(COFLO_BOOST_LIBS)"
	echo "`date -u`" > $@-temp && mv $@-temp $@

stamp_boost_src_installed: stamp_boost_src_configured
	rm -f $@-temp
	@echo "Building Boost with b2...";
	cd $(BOOST_TARBALL_DIRNAME) && ./b2 -d 1 --build-dir=../boost_build_dir -q $(COFLO_BOOST_BJAM_JOBS) --prefix=../boost_local \
		variant=$(COFLO_BOOST_VARIANT) link=static runtime-link=$(COFLO_BOOST_RUNTIME_LINK) threading=$(COFLO_BOOST_THREADING)
	echo "`date -u`" > $@-temp && mv $@-temp $@

# @todo I'd still like to install this to a local directory (i.e. ./boost_local), but AFAICT it's nothing more
# than a copy, and bjam takes way too long to do it.  For now we'll use the headers and libs in situ. 	
#stamp_boost_src_installed: stamp_boost_src_built
#	rm -f $@-temp
#	cd $(BOOST_TARBALL_DIRNAME) && ./b2 --build-dir=../boost_build_dir -q $(COFLO_BOOST_BJAM_JOBS) --prefix=../boost_local \
#		link=static threading=$(COFLO_BOOST_THREADING) install
#	echo "`date -u`" > $@-temp && mv $@-temp $@

endif

###
### Rules to extract the googletest distribution.
###
stamp-gtest-extracted: gtest-1.6.0.tar.bz2
	tar -xaf "$<"
	echo "$$(date)" > "$@-temp" && mv "$@-temp" "$@"
	
third_party_complete_deps += stamp-gtest-extracted
CLEAN_LOCAL_DIRS += gtest-1.6.0
EXTRA_DIST += gtest-1.6.0.tar.bz2
MOSTLYCLEANFILES += $(third_party_complete_deps)

###
### Automake rules for creating archives of the JavaScript and CSS files
### used in the report.
###

css_dark_hive_src_root_dir = $(srcdir)/jquery-ui-1.9.1.custom

css_dark_hive_files = css/dark-hive/images/ui-bg_flat_30_cccccc_40x100.png \
	css/dark-hive/images/ui-bg_flat_50_5c5c5c_40x100.png \
	css/dark-hive/images/ui-bg_glass_40_ffc73d_1x400.png \
	css/dark-hive/images/ui-bg_highlight-hard_20_0972a5_1x100.png \
	css/dark-hive/images/ui-bg_highlight-soft_33_003147_1x100.png \
	css/dark-hive/images/ui-bg_highlight-soft_35_222222_1x100.png \
	css/dark-hive/images/ui-bg_highlight-soft_44_444444_1x100.png \
	css/dark-hive/images/ui-bg_highlight-soft_80_eeeeee_1x100.png \
	css/dark-hive/images/ui-bg_loop_25_000000_21x21.png \
	css/dark-hive/images/ui-icons_222222_256x240.png \
	css/dark-hive/images/ui-icons_4b8e0b_256x240.png \
	css/dark-hive/images/ui-icons_a83300_256x240.png \
	css/dark-hive/images/ui-icons_cccccc_256x240.png \
	css/dark-hive/images/ui-icons_ffffff_256x240.png \
	css/dark-hive/jquery-ui-1.9.1.custom.css \
	css/dark-hive/jquery-ui-1.9.1.custom.min.css

css_dark_hive_dirs = css/dark-hive/images \
	css/dark-hive
	
# The jQuery UI Layout files.

jquery_ui_layout_root = $(srcdir)/jquery-ui-layout-plugin

jquery_ui_layout_files = css/layout-default-latest.css \
	js/jquery.layout-latest.js
	
# The jQuery, jQuery UI, and jsTree JavaScript files which need to go into reports.
jquery_ui_report_js_files = jquery-ui-1.9.1.custom/js/jquery-ui-1.9.1.custom.js \
	jquery/jquery-1.8.2.js \
	jstree_pre1.0_fix_1/jquery.jstree.js

# Other files and directory trees we need to distribute.
other_dist_files = jstree_pre1.0_fix_1 jquery-ui-1.9.1.custom jquery
	
# The jQuery UI Layout intermediate tar file.
jquery_ui_layout.tar: $(jquery_ui_layout_files:%=$(jquery_ui_layout_root)/%)
	tar --format=posix --create --file="$@-temp" --directory=$(jquery_ui_layout_root) $(jquery_ui_layout_files)
	mv "$@-temp" "$@"

jquery_ui_js.tar: $(jquery_ui_report_js_files)
	@: Create an empty tar archive.  Note that we must "trick" tar into doing this.
	tar --format=posix --create --file="$@-temp" --files-from=/dev/null
	for NAME in $^; do tar --append --file="$@-temp" \
		--directory=$$(dirname $${NAME}) --add-file=$$(basename $${NAME}) \
		 --transform='s,(.+\.js),js/\1,x;s,(.+\.css),css/\1,x'; done;
	mv "$@-temp" "$@"

jquery_ui_css.tar: $(css_dark_hive_files:%=$(css_dark_hive_src_root_dir)/%)
	tar --format=posix --create --file="$@-temp" --directory=$(css_dark_hive_src_root_dir) $(css_dark_hive_files)
	mv "$@-temp" "$@"
	
jquery_ui.tar: jquery_ui_js.tar jquery_ui_css.tar
	: For some reason, creating an empty tar file and then --concatenating into it results in a corrupt tarfile.
	: So instead, we will copy the first file and concat into that. 
	cp "$<" "$@-temp"
	tar --concatenate --file="$@-temp" jquery_ui_css.tar
	mv "$@-temp" "$@"
	
report_boilerplate.tar: jquery_ui.tar jquery_ui_layout.tar
	cp "$<" "$@-temp"
	tar --concatenate --file="$@-temp" jquery_ui_layout.tar
	mv "$@-temp" "$@"

CLEANFILES_REPORT_BOILERPLATE = jquery_ui_layout.tar jquery_ui.tar jquery_ui_js.tar jquery_ui_css.tar report_boilerplate.tar

third_party_complete_deps += $(BUILTSOURCES_BOOST) report_boilerplate.tar

# The stamp file which will depend on all active third-party builds.
# We do this this way so as to maximize parallel build potential.
stampfile-all-third-party-builds-complete: $(third_party_complete_deps)
	echo "All third-party builds complete, $$(date)" > "$@-temp"
	mv "$@-temp" "$@"

EXTRA_DIST += $(css_dark_hive_files:%=$(css_dark_hive_src_root_dir)/%) \
	$(jquery_ui_layout_files:%=$(jquery_ui_layout_root)/%) \
	$(other_dist_files)

# Delete all the stamps except for the untarring ones for any clean.
# Also delete the generated .tar files, they're cheap to rebuild.
MOSTLYCLEANFILES +=  $(MOSTLYCLEANFILES_DPARSER) $(MOSTLYCLEANFILES_BOOST) \
	$(CLEANFILES_REPORT_BOILERPLATE) stampfile-all-third-party-builds-complete
	 
# Additional files to clean during a "make clean".
CLEANFILES = $(CLEANFILES_DPARSER) $(CLEANFILES_BOOST) 

# Hook "mostlyclean" to avoid rm'ing the untarred boost tarball when we don't have to.
mostlyclean-local:
	@: 'Remove the Boost local installation tree.'
	test ! -d "$(BOOST_LOCAL)" || rm -rf "$(BOOST_LOCAL)"
	@: 'Remove the Boost local build tree.'
	test ! -d "$(BOOST_BUILD_DIR)" || rm -rf "$(BOOST_BUILD_DIR)"	

# We need to hook 'clean' here because we need to rm the entire untarred source trees, not just individual files.	
clean-local:
	@:; # Strip any spaces off the ends of the directory list.
	DIRLIST="$$(echo "$(CLEAN_LOCAL_DIRS)" | $(SED) -r 's/[ ]*(.*)[ ]*/\1\n/')"; \
	if [ -n "$${DIRLIST}" ]; then \
		for DIRNAME in $${DIRLIST}; \
		do \
			echo "Removing directory $${DIRNAME}...";\
			test ! -d "$${DIRNAME}" || rm -rf "$${DIRNAME}"; \
		done; \
	fi;

# Nothing in here to install or check, so no other *-local hooks.
