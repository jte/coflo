# Makefile.am for CoFlo's Test directory.
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

## Process this file with automake to produce Makefile.in.

#AUTOMAKE_OPTIONS = parallel-tests
VERBOSE = 1

BUILT_SOURCES = test_source_file_1.cpp test_source_file_2.cpp

# List of scripts that will be use during "make check" and are not built.  Note that scripts are not
# distributed by default, hence the "dist_" prefix.
dist_check_SCRIPTS = test1.sh test_use_compiler_i686-pc-mingw32-gcc-4.5.2.sh

# List of scripts that will be use during "make check" and are built.  Note that scripts are not
# distributed by default.
check_SCRIPTS = test_cpp1.sh

test_source_file_1.cpp: test_source_file_1.c
	@# Simply copy the C files to make C++ versions.
	cp $< $@;

test_source_file_2.cpp: test_source_file_2.c
	@# Simply copy the C files to make C++ versions.
	cp $< $@;

test_cpp1.sh: test_template.sh Makefile
	cat $< | sed -e 's,[@]gcc[@],gcc,g' \
	    -e 's,[@]test_source[@],\"test_source_file_1.cpp test_source_file_2.cpp\",g' \
	> $@


#EXTRA_PROGRAMS = test1.sh
#EXTRA_DIST = test1.sh test_use_compiler_i686-pc-mingw32-gcc-4.5.2.sh

# Tests to run for "make check".
#TESTS_ENVIRONMENT = export "TEST_CPPFLAGS=$(CPPFLAGS)" && export "TEST_CXXFLAGS=$(CXXFLAGS)" && $(SHELL) -x
TESTS = test1.sh test_use_compiler_i686-pc-mingw32-gcc-4.5.2.sh test_cpp1.sh
TEST_EXTENSIONS = .sh

SH_LOG_COMPILER = $(SHELL)
AM_SH_LOG_FLAGS =
