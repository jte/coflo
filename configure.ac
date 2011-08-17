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
m4_include([m4/coflo_source_control_version.m4])

AC_INIT([CoFlo], [0.1.SVN_VERSION], [BUG-REPORT-ADDRESS],,[http://coflo.sourceforge.net/])
#AC_SUBST([SVN_VERSION])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([[Copyright (C) 2011 Gary R. Van Sickle]])

# Use the auxilliary build tools (e.g. install-sh, config.sub, etc.) in ./build-aux.
AC_CONFIG_AUX_DIR([build-aux])
AC_REQUIRE_AUX_FILE([install-sh])
AC_REQUIRE_AUX_FILE([missing])
AC_REQUIRE_AUX_FILE([depcomp])
AC_REQUIRE_AUX_FILE([svn-version-gen])
# Don't think we need config.guess or config.sub, not using AC_CANONICAL_BUILD/HOST/TARGET

# Additional Autoconf macros are in ./m4.
AC_CONFIG_MACRO_DIR([m4])

# Make sure we re-configure when the version changes.
AC_SUBST([CONFIG_STATUS_DEPENDENCIES], ['$(top_srcdir)/build-aux/svn-version-gen' '$(top_srcdir)/.version'])

# Initialize Automake.
# See <http://www.gnu.org/software/automake/manual/automake.html#Options> for details on
# what the various settings do.
AM_INIT_AUTOMAKE([check-news
	std-options
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

# Enable the configure remake rules.
#AM_MAINTAINER_MODE([enable])

# Set up the maintainer compiler flags.
# Increase the default GCC warning level.
AC_SUBST([AM_CPPFLAGS], ["-Wall -Wextra"])
# By default, compile C and C++ with the maximum possible debugging info and least optimization for maximum debugability.
AC_SUBST([AM_CFLAGS], ["-ggdb3 -O0"])
AC_SUBST([AM_CXXFLAGS], ["-ggdb3 -O0"])

# Clear the "user defaults".  This doesn't override any configure-time or make-time settings the builder may
# specifiy for these variables, it just makes the values empty if they're not specified.
: ${CPPFLAGS=""}	# Use an empty default.
: ${CFLAGS=""}		# Don't default to "-g -O2".
: ${CXXFLAGS=""}	# Don't default to "-g -O2".

###
### Checks for the programs we need to do a build.
###
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

###
### Checks for libraries
###

# Make sure we have a usable Boost version.
AX_BOOST_BASE([1.45.0])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_REGEX
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

###
### Checks for some library functions we need.
###
AC_CHECK_FUNC(glob)

###
### Initialize the test suite.
# Autoconf-supported test directory.
AC_CONFIG_TESTDIR([test])
AC_CONFIG_FILES([test/Makefile test/atlocal])
AM_MISSING_PROG([AUTOM4TE], [autom4te])

AC_CONFIG_FILES([Makefile src/Makefile src/controlflowgraph/Makefile src/debug/Makefile src/libexttools/Makefile src/controlflowgraph/statements/Makefile])
AC_OUTPUT

