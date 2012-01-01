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
#    autoconf 2.68
#    automake 1.11.1
# - Cygwin:
#    autoconf 2.68
#    automake 1.11.1
# - Fedora:
#    autoconf 2.68
#    automake 1.11.1
AC_PREREQ([2.68])
# Require the newest libtool in common between all platforms we currently anticipate CoFlo will be developed on
# ("developed on", not "distributed for": libtool isn't necessary to build distributions).
# Both Cygwin and Ubuntu ship libtool 2.4 as of this writing.
LT_PREREQ([2.4])


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
AC_INIT([CoFlo], [0.0.3], [BUG-REPORT-ADDRESS],,[http://coflo.sourceforge.net/])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([Copyright (C) 2011 Gary R. Van Sickle])

# Use the auxilliary build tools (e.g. install-sh, config.sub, etc.) in ./build-aux.
AC_CONFIG_AUX_DIR([build-aux])

###
### AC_REQUIRE some helper scripts etc.  This both ensures they exist and causes them to be distributed.
###
# Require our source control version checking script. 
#AC_REQUIRE_AUX_FILE([coflo_version_control_info])
# Require our move-if-file-contents-differ script.
#AC_REQUIRE_AUX_FILE([move-if-change])
#AC_REQUIRE_AUX_FILE([config.guess])

# If/when we need to use any AC_REPLACE_FUNCS, we'll put them in ./portable. 
#AC_CONFIG_LIBOBJ_DIR([portable])

# Additional Autoconf macros are in ./m4.
AC_CONFIG_MACRO_DIR([m4])


#
# Defines for the config.h.
#
#AC_SUBST([VC_REV], [m4_esyscmd_s([build-aux/coflo_version_control_info "" .tarball-version])])
#AC_DEFINE_UNQUOTED([PACKAGE_VERSION_CONTROL_REVISION], ["$VC_REV"],
#	[Revision of source repository which was the initial origin of this distribution.])
#AM_CONDITIONAL([HAVE_TARBALL_VERSION_INFO], [test -f "$(top_srcdir)/.tarball-version"])

# Make sure we re-autoconf when the version changes.
# This adds a dependency to the configure rule in the ultimately resulting Makefile.  This
# rule reruns autoconf, so the new .version info will get picked up by both the packaging mechanisms
# and the config.h file.  
#AC_SUBST([CONFIGURE_DEPENDENCIES], ['$(top_srcdir)/.version'])

# Initialize Automake.
# See <http://www.gnu.org/software/automake/manual/automake.html#Options> for details on
# what the various settings do.
AM_INIT_AUTOMAKE([1.11.1
	check-news
    silent-rules
	subdir-objects
    tar-pax
    dist-bzip2
    color-tests
    parallel-tests
    std-options
    -Wall
    -Werror
])

# Get canonical Build and Host system types.
# This is mainly for issue reports.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
# Put the triples we found in config.h as #define'd string literals.
AC_DEFINE_UNQUOTED([SYSTEM_TRIPLE_BUILD],["$build"],[Define to the build triple.])
AC_DEFINE_UNQUOTED([SYSTEM_TRIPLE_HOST],["$host"],[Define to the host triple.])


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
# For Boost.Filesystem, set CPP flags to the Version 3 interface and to remove all deprecated interfaces.
AC_SUBST([AM_CPPFLAGS], ["-Wall -Wextra -DBOOST_FILESYSTEM_NO_DEPRECATED=1 -DBOOST_FILESYSTEM_VERSION=3"])
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
#: ${LDFLAGS="-L/usr/local/lib"}

###
### Checks for the programs we need to do a build.
###

# The DParser distribution's tarball.
AC_ARG_WITH([dparser-tarball],
	[AS_HELP_STRING([--with-dparser-tarball=@<:@PATH@:>@],
		[Path to the dparser 1.26 source distribution tarball.])], [], [with_dparser_tarball=no])

# Check if the user specified a filename.
AC_MSG_CHECKING([if DParser distribution tarball was specified])
AS_IF([test "x$with_dparser_tarball" = "xno" || test "x$with_dparser_tarball" = "xyes"],
	[AC_MSG_RESULT([no])
		AC_MSG_FAILURE([--with-dparser-tarball=@<:@PATH@:>@ must be specified])],
	[AC_MSG_RESULT([yes])])

# Check if the specified filename is a real file and that we can read it.	
AC_MSG_CHECKING([if specified DParser distribution tarball \"$with_dparser_tarball\" exists])
AS_IF([test -f "$with_dparser_tarball"],
	[AC_MSG_RESULT([yes])],
	[AC_MSG_RESULT([no])
		AC_MSG_FAILURE([file does not exist or is not a regular file: $with_dparser_tarball])])

AC_MSG_CHECKING([if specified DParser distribution tarball \"$with_dparser_tarball\" is readable])
AS_IF([test -r "$with_dparser_tarball"],
	[AC_MSG_RESULT([yes])],
	[AC_MSG_RESULT([no])
		AC_MSG_FAILURE([file is not readable: $with_dparser_tarball])])
		
# File checks out, AC_SUBST a variable for it.
AC_SUBST([FILE_PATH_DPARSER_TARBALL], [$with_dparser_tarball])
AC_MSG_NOTICE([FILE_PATH_DPARSER_TARBALL=$FILE_PATH_DPARSER_TARBALL, abs_top_srcdir=$abs_top_srcdir])

# The DParser make_dparser program.
#AX_WITH_PROG([PROG_ABSPATH_MAKE_DPARSER], [make_dparser])
#if test -z "$PROG_ABSPATH_MAKE_DPARSER"; then
#	AC_MSG_ERROR([Cannot find a usable make_dparser.])
#fi
AC_PROG_CC
# This is required instead of AC_PROG_CC_C_O by automake's "subdir-objects" option.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
# Initialize Libtool.
LT_INIT
# Create a variable for updating the libtool script if it gets out of date.
AC_SUBST([LIBTOOL_DEPS])

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
AX_WITH_PROG([PROG_ABSPATH_DOT], [dot])
if test -z "$PROG_ABSPATH_DOT"; then
	AC_MSG_WARN([Cannot find a usable Graphviz dot program.])
fi
# Put whatever we may have found in config.h.
AC_DEFINE_UNQUOTED([PROG_ABSPATH_DOT],["$PROG_ABSPATH_DOT"],[Define to the absolute path to the Graphviz dot program.])


###
### Checks for libraries
###

# Make sure we have the DParser library available.
#AC_CHECK_LIB([dparse], [dparse], [], [AC_MSG_ERROR([DParser runtime library not found.])])

# Make sure we have a Boost version we can use.
# Use the Autoconf Macro Archive macros.
AX_BOOST_BASE([1.46.1])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_REGEX
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

# Use the boost.m4 macro.
#BOOST_REQUIRE([1.46.1])
# Header-only libs
#BOOST_FOREACH
#BOOST_UTILITY
# Libs requiring linking
#BOOST_PROGRAM_OPTIONS
#BOOST_REGEX
#BOOST_SYSTEM
#BOOST_FILESYSTEM


AS_ECHO(["BOOST_LDFLAGS=$BOOST_LDFLAGS"])
AC_SUBST([BOOST_RPATH], [`echo -n "$BOOST_LDFLAGS" | sed 's/-L//'`])
AS_ECHO(["BOOST_RPATH=$BOOST_RPATH"])

###
### Checks for some library functions we need.
###
AC_CHECK_FUNC(glob)

###
### Checks for optional libraries.
###

# Allow the user to build with dmalloc.
AX_WITH_DMALLOC

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
	doc/Makefile
	src/Makefile
	src/controlflowgraph/Makefile
	src/controlflowgraph/analysis/Makefile
	src/controlflowgraph/statements/Makefile
	src/controlflowgraph/edges/Makefile
	src/controlflowgraph/visitors/Makefile
	src/debug_utils/Makefile
	src/libexttools/Makefile
	third_party/Makefile])
AC_OUTPUT

