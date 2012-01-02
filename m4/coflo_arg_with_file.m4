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
#
# SYNOPSIS
#
#   COFLO_ARG_WITH_FILE([VARIABLE], [with-option-suffix], [description of the file])
#
# DESCRIPTION
#
#   Adds options to the generated configure which requre a file to be specified with either:
#       "configure --with-with-option-suffix=/path/to/file"
#   or:
#       "configure VARIABLE=/path/to/file"
#   Specified file is check for existence, readability, and that it's a regular file.  Configuration
#   aborts if the file is not specified or if any of the checks fail.
#   The given path is stored in the precious variable VARIABLE.
# 

#serial 1

AC_DEFUN([COFLO_ARG_WITH_FILE],
[
	AC_PREREQ([2.68])
	
	pushdef([VARIABLE],$1)
	pushdef([ARG_TEXT],$2)
	pushdef([VAR_AND_OPT_DESCRIPTION],$3)
	
	dnl Add the precious variable and associated help text.
	AC_ARG_VAR(VARIABLE,Absolute path to VAR_AND_OPT_DESCRIPTION.)
	
	dnl Check if the associated variable is already defined.
	AS_IF([test -z "$VARIABLE"],
	[
		dnl It isn't.
		AC_MSG_CHECKING([if VAR_AND_OPT_DESCRIPTION was specified])
		AC_ARG_WITH(ARG_TEXT,AS_HELP_STRING([--with-ARG_TEXT=PATH],[Absolute path to VAR_AND_OPT_DESCRIPTION.]),
		[
			dnl The option was passed, but check if the user didn't provide a value for PATH,
			dnl or gave a "yes" or "no" instead of a path. 
			AS_IF([test "$withval" != yes && test "$withval" != no],
			[
				dnl The provided path was something other than empty, "yes", or "no".
				VARIABLE="$withval"
				AC_MSG_RESULT(yes: \"$VARIABLE\")
			],
			[
				dnl Nothing that might be a path was provided.
				VARIABLE=""
				AC_MSG_RESULT([no])
			])
		])
	])
	
	AS_IF([test -z "$VARIABLE"],
	[
		dnl If VARIABLE is empty at this point, we didn't get it from either AC_ARG_VAR or AC_ARG_WITH,
		dnl so we have to abort.
		AC_MSG_FAILURE([Either --with-ARG_TEXT=PATH or VARIABLE=PATH must be specified.])
	],[])
	
	dnl If we haven't failed yet, make sure the path we return is an absolute path.
	VARIABLE=`readlink -f "$VARIABLE"`		
	
	dnl Check if the specified file exists and is a regular file.
	AC_MSG_CHECKING([if specified VAR_AND_OPT_DESCRIPTION \"$VARIABLE\" exists and is a regular file])
	AS_IF([test -f "$VARIABLE"],
	[
		AC_MSG_RESULT([yes])
	],
	[
		AC_MSG_RESULT([no])
		AC_MSG_FAILURE([file does not exist or is not a regular file: \"$VARIABLE\"])
	])
	
	dnl Check if the specified file is readable by us.
	AC_MSG_CHECKING([if specified VAR_AND_OPT_DESCRIPTION \"$VARIABLE\" is readable])
	AS_IF([test -r "$VARIABLE"],
	[
		AC_MSG_RESULT([yes])
	],
	[
		AC_MSG_RESULT([no])
		AC_MSG_FAILURE([file is not readable: \"$VARIABLE\"])
	])
	
	popdef([VAR_AND_OPT_DESCRIPTION])
	popdef([ARG_TEXT])
	popdef([VARIABLE])
])# COFLO_ARG_WITH_FILE

