# configure.ac for CoFlo
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

# Process this file with autoconf to produce a configure script.

# Require the newest autoconf which is commonly deployed on our targeted platforms.
# Current defaults are:
# - Ubuntu:
#    autoconf 2.67
# - Cygwin:
#    autoconf 2.68
#    automake 1.11.1
# - Fedora:
#    autoconf 2.68
#    automake 1.11.1
AC_PREREQ([2.67])

# Define an M4 macro which will get the working copy's current version info via "svnversion".
#m4_include([m4/coflo_source_control_version.m4])

# Initialize automake.
# The only thing unusual here is the version number handling.  This is handled much the same way
# as it is in the autoconf package itself, bison, and other packages.  The overall effect of the m4_esyscmd_s() call is:
#  If we're building from a source controlled working copy:
#	- Append the working copy's revision number to the version.
#  If we're building from a distributed tarball:
#   - Append the revision number of the packager's working copy.
# The package is reconfigured as necessary, in particular when what's in .version doesn't match what is reported by
# svnversion.
# There is considerable mechanism required to make this work properly.  See the "build-aux/coflo_version_control_info" script,
# the "AC_SUBST([CONFIG_STATUS_DEPENDENCIES]..." line below, and the top-level Makefile.am for the other components
# of this mechanism.
#AC_INIT([CoFlo], , [BUG-REPORT-ADDRESS],,[http://coflo.sourceforge.net/])
AC_INIT([CoFlo], [0.0.1], [BUG-REPORT-ADDRESS],,[http://coflo.sourceforge.net/])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([Copyright (C) 2011 Gary R. Van Sickle])

# Use the auxilliary build tools (e.g. install-sh, config.sub, etc.) in ./build-aux.
AC_CONFIG_AUX_DIR([build-aux])

# Require our source control version checking script. 
AC_REQUIRE_AUX_FILE([coflo_version_control_info])

# Additional Autoconf macros are in m4/.
AC_CONFIG_MACRO_DIR([m4])


#
# Defines for the config.h.
#
AC_SUBST([VC_REV], [m4_esyscmd_s([build-aux/coflo_version_control_info "" .tarball-version])])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_CONTROL_REVISION], ["$VC_REV"],
	[Revision of source repository which was the initial origin of this distribution.])

# Make sure we re-autoconf when the version changes.
# This adds a dependency to the configure rule in the ultimately resulting Makefile.  This
# rule reruns autoconf, so the new .version info will get picked up by both the packaging mechanisms
# and the config.h file.  
AC_SUBST([CONFIGURE_DEPENDENCIES], ['$(top_srcdir)/.version'])

# Initialize Automake.
# See <http://www.gnu.org/software/automake/manual/automake.html#Options> for details on
# what the various settings do.
AM_INIT_AUTOMAKE([std-options
	subdir-objects
    color-tests
    tar-pax
    dist-bzip2
    parallel-tests
    1.11.1
    -Wall
    -Werror
])

# Programs and scripts to exempt from the "installcheck" rule introduced by "std-options".
# List programs and scripts which don't support --help or --version here.
# Add the "$(EXEEXT)" suffix to any programs.
# @TODO This is causing autoconf to fail for some reason.
#AM_INSTALLCHECK_STD_OPTIONS_EXEMPT=

###
### Manual configure options.
###

# Set up the maintainer compiler flags.
# Increase the default GCC warning level.
AC_SUBST([AM_CPPFLAGS], ["-Wall -Wextra"])
# By default, compile C and C++ with the maximum possible debugging info and least optimization for maximum debugability.
AC_SUBST([AM_CFLAGS], ["-ggdb3 -O0"])
AC_SUBST([AM_CXXFLAGS], ["-ggdb3 -O0 -Wold-style-cast -fno-pretty-templates"])

# Clear the "user defaults" for compiler optimization and debug flags.  This doesn't override any configure-time or make-time settings the builder may
# specifiy for these variables, it just makes the values empty if they're not specified at configure- or make-time.
# Otherwise, Autoconf assumes the builder wants to compile everything "-g -O2" by default, which overrides
# any AM_C*FLAGS.
: ${CPPFLAGS=""}	# Use an empty default.
: ${CFLAGS=""}		# Don't default to "-g -O2".
: ${CXXFLAGS=""}	# Don't default to "-g -O2".
# Similar thing for LDFLAGS.  This is to get AC_CHECK_LIB to also look in /usr/local/lib for libdparser.a. 
: ${LDFLAGS="-L/usr/local/lib"}

###
### Checks for the programs we need to do a build.
###
# The DParser make_dparser program.
AX_WITH_PROG([PROG_MAKE_DPARSER_ABSPATH], [make_dparser])
if test -z "$PROG_MAKE_DPARSER_ABSPATH"; then
	AC_MSG_ERROR([Cannot find a usable make_dparser.])
fi 
AC_PROG_CC
# This is required instead of AC_PROG_CC_C_O by automake's "subdir-objects" option.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL

# Some other tools we'll need.
AC_PROG_SED

# Look for Doxygen and configure it if it does exist.
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$PACKAGE_NAME], [src/Doxyfile])

# Check for Graphviz dot at configure-time. 
AX_WITH_PROG([PROG_DOT_ABSPATH], [dot])
if test -z "$PROG_DOT_ABSPATH"; then
	AC_MSG_WARN([Cannot find a usable Graphviz dot program.])
fi
# Put whatever we may hav found in config.h.
AC_DEFINE_UNQUOTED([PROG_DOT_ABSPATH],["$PROG_DOT_ABSPATH"],[Define to the absolute path to the Graphviz dot program.])


###
### Checks for libraries
###

# Make sure we have the DParser library available.
AC_CHECK_LIB([dparse], [dparse], [], [AC_MSG_ERROR([DParser runtime library not found.])])

# Make sure we have a usable Boost version.
AX_BOOST_BASE([1.42.0])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_REGEX
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

###
### Checks for some library functions we need.
###
AC_CHECK_FUNC(glob)

###
### Checks for optional libraries.
###


###
### Initialize the test suite.
###
# Autoconf-supported test directory.
AC_CONFIG_TESTDIR([tests])
# The files to be generated by configure.
AC_CONFIG_FILES([tests/Makefile tests/atlocal])
# Autotest needs autom4te, or 'missing's stub for it. 
AM_MISSING_PROG([AUTOM4TE], [autom4te])

###
### Create the Makefiles for the program.
###
AC_CONFIG_FILES([Makefile
	src/Makefile
	src/controlflowgraph/Makefile
	src/controlflowgraph/analysis/Makefile
	src/controlflowgraph/statements/Makefile
	src/controlflowgraph/edges/Makefile
	src/controlflowgraph/visitors/Makefile
	src/debug_utils/Makefile
	src/libexttools/Makefile
	doc/Makefile])
AC_OUTPUT

