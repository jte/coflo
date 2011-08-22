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
EXTRA_DIST = .version

# Rules for making and cleaning .version.
BUILT_SOURCES = $(top_srcdir)/.version
DISTCLEANFILES = $(top_srcdir)/.version
$(top_srcdir)/.version: FORCE
	echo $(VERSION) > $@-t && mv $@-t $@
FORCE:

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

# Add the doxygen-doc files to the MOSTLYCLEANFILES variable so they can be cleaned.
# Per <http://www.gnu.org/software/autoconf-archive/ax_prog_doxygen.html#ax_prog_doxygen>
MOSTLYCLEANFILES = $(DX_CLEANFILES)
