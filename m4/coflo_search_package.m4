#
# Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

# SYNOPSIS
#
#   COFLO_SEARCH_PACKAGE([PACKAGE_NAME], [FUNCTION], [LIBRARY_NAME], [HEADER_FILE], [PROGRAM], [path = '$PATH'],
#      [ACTION_IF_FOUND], [ACTION_IF_NOT_FOUND])
#
# DESCRIPTION
#
#   Checks for the existence of a package (header, library, and optional executable) in the given path.
#   Defines the following variables:
#       PROG_ABSPATH_[PROGRAM]
#			- The absolute path to PROGRAM.
#		[PACKAGE_NAME]_CPPFLAGS
#			- Flags to add to the C/C++ preprocessor line (*_CPPFLAGS)
#		[PACKAGE_NAME]_LDADD
#			- Libraries to add to the linker line (*_LDADD)
#
# @todo version checking.
# 

#serial 1

AC_DEFUN([COFLO_SEARCH_PACKAGE],
[
	AC_PREREQ([2.68])
	
	pushdef([PACKAGE_NAME],$1)
	pushdef([FUNCTION],$2)
	pushdef([LIBRARY_NAME],$3)
	pushdef([HEADER_FILE],$4)
	pushdef([PROGRAM],$5)
	pushdef([PATH_PACKAGE_ROOT],$6)
	pushdef([ACTION_IF_FOUND],$7)
	pushdef([ACTION_IF_NOT_FOUND],[m4_default([$8],
		[
			# Default ACTION_IF_NOT_FOUND is to abort with an error message.
			AC_MSG_FAILURE([Either --with-ARG_TEXT=PATH or VARIABLE=PATH must be specified.])
		])
	])
	
	# First check for the executable.
	# @todo ... if specified.
	AS_VAR_PUSHDEF([PROG_ABSPATH], AS_TR_SH([PROG_ABSPATH_]m4_toupper(PROGRAM)))
	m4_pushdef([PATH_PACKAGE_ROOT_PLUS_BIN], m4_bpatsubsts(PATH_PACKAGE_ROOT[/bin], [[:]], [/bin:]))
	
	AX_WITH_PROG([]PROG_ABSPATH[], []PROGRAM[], [:], []PATH_PACKAGE_ROOT_PLUS_BIN[])
	AS_VAR_IF([PROG_ABSPATH], [:],
		[
			# Didn't find it.
			AC_MSG_NOTICE([didnt find it])
		],
		[
			# Found it.
			AC_MSG_NOTICE([found it])

			# Extract the package's root directory.
			AS_VAR_COPY([var], [PROG_ABSPATH])
			package_root_dir=`AS_DIRNAME([$var])`;
			package_root_dir=`AS_DIRNAME([$package_root_dir])`;			
			
			# Now look for the library components.
			# @todo Actually look for these.
			AC_SUBST(m4_toupper(PACKAGE_NAME)[_CPPFLAGS], ["-I $package_root_dir/include"])
			AC_SUBST(m4_toupper(PACKAGE_NAME)[_LDADD], ["$package_root_dir/lib/lib]LIBRARY_NAME[.a"])
		]
	)
	
	m4_popdef([PATH_PACKAGE_ROOT_PLUS_BIN])
	AS_VAR_POPDEF([PROG_ABSPATH])
	
	popdef([ACTION_IF_NOT_FOUND])
	popdef([ACTION_IF_FOUND])
	popdef([PATH_PACKAGE_ROOT])
	popdef([PROGRAM])
	popdef([HEADER_FILE])
	popdef([LIBRARY_NAME])
	popdef([FUNCTION])
	popdef([PACKAGE_NAME])
])
