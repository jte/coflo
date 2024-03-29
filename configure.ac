# configure.ac for CoFlo
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
AC_INIT([CoFlo], [0.0.5], [BUG-REPORT-ADDRESS],,[http://coflo.sourceforge.net/])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([Copyright (C) 2011, 2012 Gary R. Van Sickle])

# Use the auxilliary build tools (e.g. install-sh, config.sub, etc.) in ./build-aux.
AC_CONFIG_AUX_DIR([build-aux])

###
### AC_REQUIRE some helper scripts etc.  This both ensures they exist and causes them to be distributed.
###
# Require our source control version checking script. 
#AC_REQUIRE_AUX_FILE([coflo_version_control_info])
# Require our move-if-file-contents-differ script.
AC_REQUIRE_AUX_FILE([move-if-change])
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
# This is mainly so we have this info for issue reports.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
# Put the triples we found in config.h as #define'd string literals.
AC_DEFINE_UNQUOTED([SYSTEM_TRIPLE_BUILD],["$build"],[Define to the build triple.])
AC_DEFINE_UNQUOTED([SYSTEM_TRIPLE_HOST],["$host"],[Define to the host triple.])

# Determine how many CPUs we have on the build machine.
# We use this for building Boost in parallel with multiple jobs.
AX_COUNT_CPUS
AC_SUBST([CPU_COUNT])

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
# -Wold-style-cast
AC_SUBST([AM_CXXFLAGS], ["-ggdb3 -O0"])

# Clear the "user defaults" for compiler optimization and debug flags.  This doesn't override any configure-time or make-time settings the builder may
# specifiy for these variables, it just makes the values empty if they're not specified at configure- or make-time.
# Otherwise, Autoconf's AC_PROG_CXX et al macros assume the builder wants to compile everything "-g -O2" by default, which overrides
# any AM_C*FLAGS.
# This otherwise-frowned-upon touching of these user variables is sanctioned by the Autoconf manual for this purpose.
: ${CPPFLAGS=""}	# Use an empty default.
: ${CFLAGS=""}		# Don't default to "-g -O2".
: ${CXXFLAGS=""}	# Don't default to "-g -O2".

# Check if the C++ compiler supports some flags we'd like to set.
# Append them to AM_CXXFLAGS if it does. 
AC_LANG_PUSH([C++])
#AX_APPEND_COMPILE_FLAGS([-fno-pretty-templates],[AM_CXXFLAGS])
AC_LANG_POP([C++])


###
### Checks for the programs
###

## Checks for the programs needed to do a build.
AC_PROG_CC
# This is required instead of AC_PROG_CC_C_O by automake's "subdir-objects" option.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_SED
AC_PROG_AWK
AC_PROG_CPP
# @note The following line is to work around an arguable bug in Autoconf 1.12/Libtool.
# (q.v. http://lists.gnu.org/archive/html/automake/2012-05/msg00014.html).
# Probably remove this when/if that is fixed. 
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_EGREP
# Initialize Libtool.  This looks like it doesn't belong here, but thing go awry if it's moved
# up by AC_INIT.
LT_INIT
# Create a variable for updating the libtool script if it gets out of date.
AC_SUBST([LIBTOOL_DEPS])

## Checks for maintainer tools.

# Autotest needs autom4te, or 'missing's stub for it. 
AM_MISSING_PROG([AUTOM4TE], [autom4te])

# We need xxd to convert the templates into C arrays.
AM_MISSING_PROG([XXD], [xxd])

	
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

# The Google Test library requires POSIX threads.
AX_PTHREAD

# The Google Test library.
# See if the user specified a path to it on the configure command line.
#COFLO_ARG_WITH_DIR([GTEST_ROOT], [gtest], [root directory of the Google C++ Testing Framework source distribution])
AC_SUBST([GTEST_ROOT], ['$(top_builddir)/third_party/gtest-1.6.0/fused-src'])


# Check for a system DParser installation.
COFLO_SEARCH_PACKAGE([dparser], [dparse], [dparse], [dparse.h], [make_dparser], [/usr/local:/usr], [], [])
AS_IF([test "x$PROG_ABSPATH_MAKE_DPARSER" != xNOT_FOUND],
	[
		AC_MSG_CHECKING([if ${PROG_ABSPATH_MAKE_DPARSER} is a usable version])
		# Get the version of the make_dparser we found.
		PROG_MAKE_DPARSER_VERSION=`$PROG_ABSPATH_MAKE_DPARSER --help 2>&1 | $EGREP -o '[[0-9]]\.[[0-9]]+' | tr -d '\n'`;
		# Check if it's a usable version.
		AS_VERSION_COMPARE([${PROG_MAKE_DPARSER_VERSION}], [1.29],
			[
				AC_MSG_RESULT([no, version is ${PROG_MAKE_DPARSER_VERSION}])
				version_ok=no;
			],
			 [
			 	version_ok=yes;
			 ],
			 [
			 	version_ok=yes;
			 ])
	],
	[version_ok=false])
AM_CONDITIONAL([HAVE_SYSTEM_MAKE_DPARSER],[test "x$version_ok" = xyes])
AM_COND_IF([HAVE_SYSTEM_MAKE_DPARSER],
	[
		AC_MSG_RESULT([yes, version is ${PROG_MAKE_DPARSER_VERSION}])
	],
	[
		AC_MSG_WARN([no usable system make_dparser found.])
	]
)

# The DParser source distribution's tarball.
# To avoid requiring the user to build and install DParser on their system, we'll take the tarball
# as a configure parameter and build it with the parameters we need.
COFLO_ARG_WITH_FILE([FILE_PATH_DPARSER_SOURCE_TARBALL], [dparser-tarball], [dparser (>=1.29) source distribution tarball],
	[],[AC_MSG_WARN([No DParser source tarball specified.])])
	
AM_CONDITIONAL([HAVE_DPARSER_EXTERNAL_TARBALL],[test "x$FILE_PATH_DPARSER_SOURCE_TARBALL" != x])
	
# The Boost source distribution's tarball.
# If the user doesn't already have a suitable Boost installation, we'll do something similar to what we do
# for DParser, and build it from the source tarball and link to it statically.
# The reason here though is more complex:
#   1. Boost (at least as of 1.48.0) wants to install into the wrong /lib directory on some systems, notably some x86_64 systems.
#   2. Neither of the Autoconf macro packages for detecting Boost that I've tried works completely correctly when
#      more than one Boost installation is on a system, e.g. one in /usr/lib and one in /usr/local/lib. E.g., if an
#      old copy is in /usr/include & /usr/lib and a suitable copy is in /usr/local/include and /usr/local/lib, both
#      will by default find the correct paths, but ultimately make the program try to link with the libs in /usr/lib
#      even though it's correctly using the headers in /usr/local/include.
COFLO_ARG_WITH_FILE([FILE_PATH_BOOST_SOURCE_TARBALL], [boost-tarball], [boost source distribution tarball],
	[],[AC_MSG_WARN([No Boost source tarball specified.])])

AM_CONDITIONAL([COFLO_USE_BUILT_BOOST],[test -n "$FILE_PATH_BOOST_SOURCE_TARBALL"])

# The list of Boost libraries we need, in a form that Boost's bootstrap.sh can understand.
AC_SUBST([COFLO_BOOST_LIBS],[system,filesystem,graph,program_options,random,regex])
# Extract the "boost_1_xx_x" part of the filename.
BOOST_TARBALL_DIRNAME=$(echo "${FILE_PATH_BOOST_SOURCE_TARBALL}" | grep -o 'boost_._.._.')
BOOST_TARBALL_TARBZ2=$(basename "${FILE_PATH_BOOST_SOURCE_TARBALL}")
BOOST_TARBALL_TAR=$(basename "${BOOST_TARBALL_TARBZ2}" .bz2)
AC_SUBST([BOOST_TARBALL_DIRNAME])
AC_SUBST([BOOST_TARBALL_TARBZ2])
AC_SUBST([BOOST_TARBALL_TAR])


AM_COND_IF([COFLO_USE_BUILT_BOOST],
[
	# A path to a boost source tarball was specified.
	# Populate the necessary variables.
	AC_SUBST([BOOST_CPPFLAGS],['-I "$(top_builddir)/third_party/$(BOOST_TARBALL_DIRNAME)"'])
	AM_CPPFLAGS="$AM_CPPFLAGS $BOOST_CPPFLAGS"
	AC_SUBST([BOOST_LDFLAGS],[""])
	AC_SUBST([BOOST_LIBTOOL_FLAGS],[""])
],
[
	# Try to find a Boost version we can use.
	# Use the Autoconf Macro Archive macros.
	# Note that there is a bug in Boost.Filesystem prior to 1.47.0 which causes an exception to be thrown if the
	# current locale as specified in the LANG environment variable is anything other than "C" (see
	# <https://svn.boost.org/trac/boost/ticket/4688>).  For that reason, Boost 1.47.0 or greater is required.

	# Use the boost.m4 macro.
	BOOST_REQUIRE([1.47.0])
	boost_type="s";
	# Header-only libs
	BOOST_FOREACH([$boost_type])
	BOOST_UTILITY([$boost_type])
	# Libs requiring linking
	BOOST_GRAPH([$boost_type])
	BOOST_PROGRAM_OPTIONS([$boost_type])
	BOOST_REGEX([$boost_type])
	BOOST_SYSTEM([$boost_type])
	BOOST_FILESYSTEM([$boost_type])
	
	AC_MSG_NOTICE([The following Boost library flags will be used:])
	AC_MSG_NOTICE([BOOST_LIB_VERSION=$BOOST_LIB_VERSION])
	AC_MSG_NOTICE([BOOST_CPPFLAGS=$BOOST_CPPFLAGS])
	AC_MSG_NOTICE([BOOST_LDPATH=$BOOST_LDPATH])
	
	# Let the user know what values we'll be using for the various Boost components.
	m4_pattern_allow([BOOST_])
	for NAME in FOREACH UTILITY GRAPH PROGRAM_OPTIONS REGEX SYSTEM FILESYSTEM;
	do
		LDFLAGS_VAR=BOOST_${NAME}_LDFLAGS;
		LDPATH_VAR=BOOST_${NAME}_LDPATH;
		LIBS_VAR=BOOST_${NAME}_LIBS;
		AC_MSG_NOTICE([${LDFLAGS_VAR}=${!LDFLAGS_VAR}])
		AC_MSG_NOTICE([${LDPATH_VAR}=${!LDPATH_VAR}])
		AC_MSG_NOTICE([${LIBS_VAR}=${!LIBS_VAR}])
	done;
	m4_pattern_forbid([BOOST_])
])


###
### Checks for some library functions we need.
###
AC_CHECK_FUNC(glob)

###
### Checks for optional libraries.
###

# If we have a system make-dparser, find the library that goes with it.
AM_COND_IF([HAVE_SYSTEM_MAKE_DPARSER],
	[
		#AX_CHECK_LIBRARY([DPARSE], [dparse.h], [dparse],
		#	[AC_MSG_WARN([dparse check ok])],
		#	[AC_MSG_WARN([dparse check failed])])
		#AC_CHECK_LIB([dparse],[dparse])
	])

# Allow the user to build with dmalloc.
AX_WITH_DMALLOC

###
### Initialize the test suite.
###
# Autoconf-supported test directory.
AC_CONFIG_TESTDIR([tests])
# The files to be generated by configure.
AC_CONFIG_FILES([tests/Makefile tests/atlocal])


###
### Create the default site configuration file for the program.
###
AC_CONFIG_FILES([src/coflo.conf])

###
### Create the Makefiles for the program.
###
AC_CONFIG_FILES([Makefile
	doc/Makefile
	src/Makefile
	src/controlflowgraph/Makefile
	src/controlflowgraph/algorithms/Makefile
	src/controlflowgraph/analysis/Makefile
	src/controlflowgraph/statements/Makefile
	src/controlflowgraph/edges/Makefile
	src/controlflowgraph/visitors/Makefile
	src/parsers/Makefile
	src/debug_utils/Makefile
	src/libexttools/Makefile
	src/templates/Makefile
	third_party/Makefile])
AC_OUTPUT

