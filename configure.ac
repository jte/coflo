# configure.ac for coflo
#
# Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
#
# This file is part of coflo.
#
# coflo is free software: you can redistribute it and/or modify it under the
# terms of version 3 of the GNU General Public License as published by the Free
# Software Foundation.
#
# coflo is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# coflo.  If not, see <http://www.gnu.org/licenses/>.

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT(coflo, 0.1, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([src/main.cpp])
# AC_CONFIG_HEADER([config.h])
AC_COPYRIGHT([Copyright (C) 2011 Gary R. Van Sickle])

# Use the auxilliary build tools (e.g. install-sh, config.sub, etc.) in ./build-aux.
AC_CONFIG_AUX_DIR([./build-aux])

# Additional Autoconf macros are in ./m4.
AC_CONFIG_MACRO_DIR([./m4])

AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL

# Make sure we have at least Boost version 1.38.0.
AX_BOOST_BASE([1.38.0])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_REGEX

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
