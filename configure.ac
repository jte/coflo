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

AC_PREREQ([2.68])
AC_INIT([CoFlo], m4_esyscmd([echo "0.0" | tr -d '\n']), [BUG-REPORT-ADDRESS],,[http://coflo.sourceforge.net/])
AC_CONFIG_SRCDIR([src/main.cpp])
# AC_CONFIG_HEADER([config.h])
AC_COPYRIGHT([[Copyright (C) 2011 Gary R. Van Sickle]])

# Use the auxilliary build tools (e.g. install-sh, config.sub, etc.) in ./build-aux.
AC_CONFIG_AUX_DIR([./build-aux])
AC_REQUIRE_AUX_FILE([install-sh])
AC_REQUIRE_AUX_FILE([missing])
AC_REQUIRE_AUX_FILE([depcomp])
# Don't think we need config.guess or config.sub, not using AC_CANONICAL_BUILD/HOST/TARGET

# Additional Autoconf macros are in ./m4.
AC_CONFIG_MACRO_DIR([./m4])

# Autoconf-supported test directory.
AC_CONFIG_TESTDIR([./test])

# Initialize Automake.
AM_INIT_AUTOMAKE([check-news
    color-tests
    dist-bzip2
    parallel-tests
    1.11.1
    -Wall
])

# Enable the configure remake rules.
AM_MAINTAINER_MODE([enable])

# Set up the maintainer compiler flags.
AC_SUBST([AM_CPPFLAGS], ["-Wall -Wextra"])
AC_SUBST([AM_CXXFLAGS], ["-ggdb3 -O0"])

# Check for the various compilers and other tools we need to do a build.
: ${CPPFLAGS=""}	# Don't default to "-g -O2"
: ${CXXFLAGS=""}	# Don't default to "-g -O2"
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL

# Some other tools we'll need.
AC_PROG_SED

# Look for Doxygen.
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
DX_INIT_DOXYGEN([CoFlo], [src/Doxyfile])

# Make sure we have a usable Boost version.
AX_BOOST_BASE([1.42.0])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_REGEX
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

# Check for come functions we need.
AC_CHECK_FUNC(glob)

AC_CONFIG_FILES([Makefile src/Makefile src/controlflowgraph/Makefile src/libexttools/Makefile src/statements/Makefile test/Makefile	test/atlocal])
AC_OUTPUT

