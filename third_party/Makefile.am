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

# Some Boost lib directory variables.
BOOST_BUILD_DIR = $(abs_builddir)/boost_build_dir
BOOST_LOCAL = $(abs_builddir)/boost_local
BOOST_STAGE_DIR = $(abs_builddir)/$(BOOST_TARBALL_DIRNAME)/stage
BOOST_LOCAL_LIB = $(abs_builddir)/$(BOOST_TARBALL_DIRNAME)/stage/lib
BOOST_LOCAL_INCLUDE = $(BOOST_LOCAL)/include

# DParser directory vars.
DPARSER_PREFIX = $(abs_builddir)/dparser/local
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

# Delete all the stamps except for the untarring ones for any clean.
MOSTLYCLEANFILES = stamp_dparser_src_built stamp_dparser_src_installed \
	 stamp_boost_src_configured stamp_boost_src_built stamp_boost_src_installed
	 
# 
CLEANFILES = stamp_dparser_src_untarred stamp_boost_src_untarred $(LIBDPARSE_BUILT_HEADER_LIST)

# Untar the DParser source tarball and create a stamp file indicating that we did it.
stamp_dparser_src_untarred: $(FILE_PATH_DPARSER_SOURCE_TARBALL)
	rm -f $@-temp
	@echo "In directory $$(pwd), attempting to untar \"$(FILE_PATH_DPARSER_SOURCE_TARBALL)\""
	@: 'Untar the DParser source distro into our build directory.'
	tar -xaf "$(FILE_PATH_DPARSER_SOURCE_TARBALL)"
	echo "`date -u`" > $@-temp && mv $@-temp $@

# Build the untarred DParser, again creating a stamp file indicating that we completed it.
stamp_dparser_src_built: stamp_dparser_src_untarred
	rm -f $@-temp
	cd d && $(MAKE) $(AM_MAKEFLAGS) D_BUILD_VERSION=exported all
	echo "`date -u`" > $@-temp && mv $@-temp $@
	
stamp_dparser_src_installed: stamp_dparser_src_built
	rm -f $@-temp
	cd d && $(MAKE) $(AM_MAKEFLAGS) CFLAGS="$(make_dparser_CPPFLAGS)" D_BUILD_VERSION=exported PREFIX=$(DPARSER_PREFIX) install
	echo "`date -u`" > $@-temp && mv $@-temp $@

###
### The Automake rules for building static versions of the Boost libraries we need from an upstream source tarball.
###

# @todo Determine if this should be single or multi from configure.
COFLO_BOOST_THREADING = single
COFLO_BOOST_BJAM_JOBS = -j$(CPU_COUNT)
# @todo runtime-link option?  variant=debug|release?

stamp_boost_src_untarred: $(FILE_PATH_BOOST_SOURCE_TARBALL)
	rm -f $@-temp && echo "`date -u`" > $@-temp
	@echo "In directory $$(pwd), attempting to untar \"$(FILE_PATH_BOOST_SOURCE_TARBALL)\""
	@: 'Untar the Boost source distro into our build directory.'
	tar -xaf "$(FILE_PATH_BOOST_SOURCE_TARBALL)"
	mv $@-temp $@

stamp_boost_src_configured: stamp_boost_src_untarred
	rm -f $@-temp
	cd boost_1_48_0 && ./bootstrap.sh --prefix="$(BOOST_LOCAL)" --with-libraries="$(COFLO_BOOST_LIBS)"
	echo "`date -u`" > $@-temp && mv $@-temp $@


#		compileflags="$(libboost_static_rollup_a_CPPFLAGS) $(CPPFLAGS)" \
#		cflags="$(libboost_static_rollup_a_CXXFLAGS) $(CXXFLAGS)" \
#		cxxflags="$(libboost_static_rollup_a_CXXFLAGS) $(CXXFLAGS)" \
#		linkflags="$(libboost_static_rollup_a_LDFLAGS) $(LDFLAGS)"
# $(COFLO_BOOST_LIBS_WITH) \		#
#
	
stamp_boost_src_installed: stamp_boost_src_configured
	rm -f $@-temp
	cd boost_1_48_0 && ./b2 --build-dir=../boost_build_dir -q $(COFLO_BOOST_BJAM_JOBS) --prefix=../boost_local \
		link=static threading=$(COFLO_BOOST_THREADING)
	echo "`date -u`" > $@-temp && mv $@-temp $@

# @todo I'd still like to install this to a local directory (i.e. ./boost_local), but AFAICT it's nothing more
# than a copy, and bjam takes way too long to do it.  For now we'll use the headers and libs in situ. 	
#stamp_boost_src_installed: stamp_boost_src_built
#	rm -f $@-temp
#	cd boost_1_48_0 && ./b2 --build-dir=../boost_build_dir -q $(COFLO_BOOST_BJAM_JOBS) --prefix=../boost_local \
#		link=static threading=$(COFLO_BOOST_THREADING) install
#	echo "`date -u`" > $@-temp && mv $@-temp $@


# Hook "mostlyclean" to avoid rm'ing the boost tarball when we don't have to.
mostlyclean-local:
	@: 'Remove the Boost local installation tree.'
	test ! -d "$(BOOST_LOCAL)" || rm -rf "$(BOOST_LOCAL)"
	@: 'Remove the Boost local build tree.'
	test ! -d "$(BOOST_BUILD_DIR)" || rm -rf "$(BOOST_BUILD_DIR)"	

# We need to hook 'clean' here because we need to rm the entire untarred source trees, not just individual files.	
clean-local:
	@: 'Remove the DParser source tree.'
	test ! -d "$(builddir)/d" || rm -rf "$(builddir)/d"
	@: 'Remove the Boost source tree.'
	test ! -d "$(builddir)/boost_1_48_0" || rm -rf "$(builddir)/boost_1_48_0"
	@: 'Remove the DParser local install tree.'
	test ! -d "$(DPARSER_PREFIX)" || rm -rf "$(DPARSER_PREFIX)"
 
# Nothing in here to install or check, so no other *-local hooks.
