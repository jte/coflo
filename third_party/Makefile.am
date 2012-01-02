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

# The Automake rules for building dparser's "make_dparser" and static library "libdparse.a" with headers.
# We only need these during the build, so instead of requiring the user to install DParser, we'll build it
# 
noinst_PROGRAMS = make_dparser
make_dparser_SOURCES =
make_dparser_DEPENDENCIES = stamp_dparser_ran_make
make_dparser_LINK = $(INSTALL_PROGRAM) $(builddir)/d/make_dparser$(EXEEXT) $(builddir)/make_dparser$(EXEEXT)

noinst_LIBRARIES = libdparse.a
libdparse_a_SOURCES =
libdparse_a_DEPENDENCIES = stamp_dparser_ran_make
libdparse_a_AR = cp $(builddir)/d/libdparse.a \
	$(builddir)/d/dparse.h \
	$(builddir)/d/dparse_tables.h \
	$(builddir)/d/dsymtab.h \
	$(builddir)/d/dparse_tree.h ./ ; :

CLEANFILES = stamp_dparser_src_untarred stamp_dparser_ran_make

#make_dparser$(EXEEXT): stamp_dparser_ran_make
#	@: '"Install" the make_dparser program into this directory.'
#	$(INSTALL_PROGRAM) $(builddir)/d/make_dparser $(builddir)/make_dparser
	
#libdparse.a: stamp_dparser_ran_make
#	@: 'Copy the library and its headers out of the .d/ directory into this directory.'
#	cp $(builddir)/d/libdparse.a $(builddir)/libdparse.a
#	cp $(builddir)/d/dparse.h ./
#	cp $(builddir)/d/dparse_tables.h ./
#	cp $(builddir)/d/dsymtab.h ./
#	cp $(builddir)/d/dparse_tree.h ./

stamp_dparser_src_untarred: $(FILE_PATH_DPARSER_SOURCE_TARBALL)
	@echo "In directory $$(pwd), attempting to untar \"$(FILE_PATH_DPARSER_SOURCE_TARBALL)\""
	@: 'Untar the DParser source distro into our build directory.'
	tar -xaf $(FILE_PATH_DPARSER_SOURCE_TARBALL)
	echo "`date -u`" > stamp_dparser_src_untarred

stamp_dparser_ran_make: stamp_dparser_src_untarred
	cd d && $(MAKE) $(AM_MAKEFLAGS) D_BUILD_VERSION=exported all

# We need to hook clean here because we need to rm the entire untarred directory.	
clean-local:
	test ! -d "$(builddir)/d" || rm -rf "$(builddir)/d"
	cd $(builddir) && rm dparse.h dparse_tables.h dsymtab.h dparse_tree.h
 
# Nothing in here to install or check.
#check-local:
#	@: 'Nothing to do here for a "make check"'
#install-data-local:
#	@: 'Nothing to do here for a "make install"'
#install-exec-local:
#	@: 'Nothing to do here for a "make install"'
#uninstall-local:
#	@: 'Nothing to do here for a "make uninstall"'
#
