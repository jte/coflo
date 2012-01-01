# Second-level Makefile.am for CoFlo, ./third_party directory.
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

# This is a proxy Makefile.am which dispatches the Autotools rules to the real makefiles of
# the third-party libraries/tools we may need.

# The Automake rules for building dparser's "make_dparser" and static library "libdparse.a" with headers.
# We only need these during the build, so instead of requiring the user to install DParser, we'll build it
# 
#noinst_PROGRAMS = make_dparser
#make_dparser_SOURCES = 
noinst_LIBRARIES = libdparse.a
libdparse_a_SOURCES =

all-local:
	@: 'Get the absolute path of FILE_PATH_DPARSER_TARBALL'
	@: TODO
	@: 'Untar the DParser source distro into our build directory.'
	@echo "In directory $$(pwd), attempting to untar \"$(FILE_PATH_DPARSER_TARBALL)\""
	tar -xaf $(FILE_PATH_DPARSER_TARBALL)
	cd d && $(MAKE) $(AM_MAKEFLAGS) D_BUILD_VERSION=exported all
	@: 'Copy the library and its headers out of the .d/ directory into this directory.'
	cp $(builddir)/d/libdparse.a $(builddir)/libdparse.a
	cp $(builddir)/d/dparse.h ./
	cp $(builddir)/d/dparse_tables.h ./
	cp $(builddir)/d/dsymtab.h ./
	cp $(builddir)/d/dparse_tree.h ./
	@: '"Install" the make_dparser program into this directory.'
	$(INSTALL_PROGRAM) $(builddir)/d/make_dparser $(builddir)/make_dparser
	
check-local:
	@: 'Nothing to do here for a "make check"'
	
clean-local:
	test ! -d "$(builddir)/d" || rm -rf "$(builddir)/d"
	rm $(builddir)/libdparse.a
	rm $(builddir)/make_dparser
	cd $(builddir) && rm dparse.h dparse_tables.h dsymtab.h dparse_tree.h
 
# Nothing in here gets installed.
install-data-local:
	@: 'Nothing to do here for a "make install"'
install-exec-local:
	@: 'Nothing to do here for a "make install"'
uninstall-local:
	@: 'Nothing to do here for a "make uninstall"'

