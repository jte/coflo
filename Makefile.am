# Top-level Makefile.am for CoFlo
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

# Make sure autotools pick up the M4 macros in the ./m4 directory, and also copy
# any third-party macros (e.g. the system-wide ax_* Autoconf Macro Archive ones
# in /usr/share/aclocal) to ./m4 so they get distributed and automatically updated.
# Only useful in the top-level Makefile.am (see http://sources.redhat.com/automake/automake.html#Rebuilding).
ACLOCAL_AMFLAGS = -I m4 --install

# Include additional automake fragments for Doxygen support.
include $(top_srcdir)/aminclude_doxygen.am

# The subdirectories containing the source code, docs, and tests.
# Note: The code in src depends on the libraries in third_party, so make sure the latter is always listed before the former.
# Note that we can't list these other libraries in e.g. coflo_DEPENDENCIES because that replaces all Automake-generated dependencies.
SUBDIRS = doc third_party src tests

# We want to distribute .version.
#EXTRA_DIST = $(top_srcdir)/.version

# .version is built source.  We want it to be built before anything else
# so that it will trigger a reconfigure at the earliest possible time.
#BUILT_SOURCES = $(top_srcdir)/.version
##DISTCLEANFILES = $(top_srcdir)/.version

#if HAVE_TARBALL_VERSION_INFO
#VC_REV = "1.2.3"
#else
#VC_REV = "4.5.6"
#endif

# The rule for building .version.
# We only update it based on content, not file time.
#$(top_srcdir)/.version:	FORCE
#	@# Get coflo_version_control_info's idea of what the right version number is.
#	@echo $$(cd $(top_srcdir) && build-aux/coflo_version_control_info "" .tarball-version) > .version-temp
#	@# If the contents differ or the target file doesn't exist, move .version-temp to its final name (.version).
#	@# If the contents are the same, do a "pretend move", and simply delete the source file,
#	@# leaving the destination file's timestamp untouched.  This avoids spurious rebuilds. 
#	$(top_srcdir)/build-aux/move-if-change ".version-temp" "$@"

FORCE:

# The dist-hook for building .tarball-version.
# This is invoked when we do a "make dist", and simply
# creates a file which contains the value of $(VERSION)
# which was current at that time.
#dist-hook:
#	@# Create the .tarball-version file.
#	@echo $(VC_REV) > .tarball-version-temp
#	$(top_srcdir)/build-aux/move-if-change ".tarball-version-temp" "$(distdir)/.tarball-version"
#	@#echo $$(cd $(top_srcdir) && build-aux/coflo_version_control_info "" .tarball-version) > .tarball-version

## This exists solely to get the version control version into the NEWS file so that distcheck
## doesn't fail.  Probably more trouble than its worth.
#.stamp-pre_distcheck:
#	: # Put the current version control revision into the NEWS file.
#	@VCVER=$$(echo $(PACKAGE_VERSION) | sed -r -n 's/.+(\.r[0-9]+)/\1/p') &&\
#	cat $(top_srcdir)/NEWS.in | sed -r 's/[@]VC_VERSION[@]/$${VCVER}/' > $(top_srcdir)/NEWS-temp
#	mv $(top_srcdir)/NEWS-temp $(top_srcdir)/NEWS
#	echo timestamp > .stamp-pre_distcheck	

# Add the doxygen-doc files to the MOSTLYCLEANFILES variable so they can be cleaned.
# Per <http://www.gnu.org/software/autoconf-archive/ax_prog_doxygen.html#ax_prog_doxygen>
MOSTLYCLEANFILES = $(DX_CLEANFILES)


# Rule to remake the libtool script if it gets out of date.
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

