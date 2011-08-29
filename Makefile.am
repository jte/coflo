# Top-level Makefile.am for CoFlo
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

# Make sure autotools pick up the M4 macros in the ./m4 directory, and also copies
# any third-party macros (e.g. the system-wide ax_* Autoconf Macro Archive ones
# in /usr/share/aclocal) to ./m4 so they get distributed and automatically updated.
# Only useful in the top-level Makefile.am (see http://sources.redhat.com/automake/automake.html#Rebuilding).
ACLOCAL_AMFLAGS = -I m4 --install

# Include additional automake fragments for Doxygen support.
include $(top_srcdir)/aminclude_doxygen.am

# The subdirectories containing the source code and tests.
SUBDIRS = src tests

# We want to distribute .version.
#EXTRA_DIST = .version

# .version is built source.  We want it to be built before anything else
# so that it will trigger a reconfigure at the earliest possible time.
#BUILT_SOURCES = $(top_srcdir)/.version
#DISTCLEANFILES = $(top_srcdir)/.version

# The make rule for building .version.
# Note that we force this to always run.
#$(top_srcdir)/.version: FORCE
#	@# Get svn-version-gen's idea of what the right version number is.
#	@echo $$(cd $(top_srcdir) && build-aux/svn-version-gen .tarball-version) > temp.version
#	@# If .version doesn't exist, create it.
#	@if ! test -f "$@"; \
#	then \
#		echo "Creating .version file..."; \
#		mv temp.version "$@"; \
#		echo "Done."; \
#	: If temp.version differs from .version, update .version. ; \
#	elif ! cmp "$@" temp.version; then \
#		echo "Updating .version file..."; \
#		mv temp.version "$@"; \
#		echo "Done."; \
#	else \
#		echo "No need to create or update .version file."; \
#		rm temp.version; \
#	fi;
#FORCE:

# The dist-hook for building .tarball-version.
# This is invoked when we do a "make dist", and simply
# creates a file which contains the value of $(VERSION)
# which was current at that time.
#dist-hook:
#	@echo $$(cd $(top_srcdir) && build-aux/svn-version-gen .tarball-version) > $(distdir)/.tarball-version

# Add the doxygen-doc files to the MOSTLYCLEANFILES variable so they can be cleaned.
# Per <http://www.gnu.org/software/autoconf-archive/ax_prog_doxygen.html#ax_prog_doxygen>
MOSTLYCLEANFILES = $(DX_CLEANFILES)
