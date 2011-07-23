#!/bin/sh
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

# Abort if there are any errors.
#set -e

#USE_GCC=@gcc@

# Run CoFlo on some test code.
#TEST_SOURCE=@test_source@

# ${srcdir} is set to the source "test/" directory.
#echo "SRCDIR = "${srcdir}
#echo "PWD = "$(pwd)

# Run CoFlo on the code.
#cd ../test/ && ../src/coflo --use-gcc=${USE_GCC} ${TEST_SOURCE} --output-dir=../test_html/

oneTimeSetUp()
{
	echo "Setting up test suite...";
	echo "Using ShUnit2 version ${SHUNIT_VERSION}";
	# Get list of gcc's installed on this system.
	GCC_LIST=$(find /usr/bin -regextype posix-extended -iregex '.*-.*-.*-gcc-.*(\.exe)?$' -or -iregex '.*/gcc(\.exe)?$');
	echo "GCCs=${GCC_LIST}";

	echo "# Created by CoFlo test script." > generated_tests.sh
	for GCCPATH in ${GCC_LIST}; do
		echo "Creating test pairing CoFlo with ${GCCPATH}...";
		cat >> generated_tests.sh <<END-OF-HERE-DOC
CHECK_CoFlow_$(echo "$GCCPATH" | tr '[:punct:]' '_')_C()
{
	echo "Testing CoFlow with ${GCCPATH}...";
	false && ../build/src/coflo && RETVAL=$?; #--use-gcc=${USE_GCC} ${TEST_SOURCE} --print-function-cfg=main); 
	echo $RETVAL;
	assertTrue "[ $RETVAL -eq 0 ]"
}
END-OF-HERE-DOC
	done;

	. ./generated_tests.sh
}

oneTimeTearDown()
{
	echo "Tearing down test suite...";
}

testEquality()
{
  assertEquals 1 1
}

testPartyLikeItIs1999()
{
  year=`date '+%Y'`
  assertEquals "It's not 1999 :-(" \
      '1999' "${year}"
}

dynamicallyGeneratedTest1()
{
	USE_GCC=@gcc@

	# Run CoFlo on some test code.
	TEST_SOURCE=@test_source@

	# ${srcdir} is set to the source "test/" directory.
	echo "SRCDIR = ${srcdir}"
	echo "PWD = $(pwd)"

	# Run CoFlo on the code.
	#cd ../test/ && ../src/coflo --use-gcc=${USE_GCC} ${TEST_SOURCE} --output-dir=../test_html/
}

dynamicallyGeneratedTest2()
{
	USE_GCC=@gcc@;

	# Run CoFlo on some test code.
	TEST_SOURCE=@test_source@;

	# ${srcdir} is set to the source "test/" directory.
	echo "SRCDIR = ${srcdir}";
	echo "PWD = $(pwd)";

	# Run CoFlo on the code.
	#cd ../test/ && ../src/coflo --use-gcc=${USE_GCC} ${TEST_SOURCE} --output-dir=../test_html/
}



# Define the dynamic test suite.
# The number of tests are determined at "make check" time.
suite()
{
	for GCCPATH in ${GCC_LIST}; do
		suite_addTest "CHECK_CoFlow_$(echo "$GCCPATH" | tr '[:punct:]' '_')_C";
	done;
}

# shunit2 needs this for Zsh compatibility.
if [ "x${ZSH_VERSION}" != "x" ]; then
	set -o shwordsplit;
fi;

# load shunit2 to run the tests.
. ./shunit2-2.1.6/src/shunit2
