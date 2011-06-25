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

# Include additional automake fragments.
include $(top_srcdir)/amlocal.am

# Get s list of all the GCC's installed in the system.
GCC_LIST := $(wildcard /usr/bin/*-*-*-gcc$(EXEEXT)) $(wildcard /usr/bin/*-*-*-gcc-*$(EXEEXT))
# Strip off directory and any extension.
#GCC_LIST_BASENAMES := $(basename $(notdir $(GCC_LIST)))
#GCC_LIST_BASENAMES_2 := $(addprefix test_,$(GCC_LIST_BASENAMES))
#GCC_TEST_SCRIPT_TARGETS := $(addsuffix .sh,$(GCC_LIST_BASENAMES_2))
###GCC_TEST_SCRIPT_TARGETS := $(shell echo "$(GCC_LIST_BASENAMES)" | sed -r 's/([^ ]+)/test_\1\.sh/g')
GCC_TEST_SCRIPT_TARGETS := test_i686-pc-cygwin-gcc-3.4.4.sh \
    test_i686-pc-cygwin-gcc-4.3.4.sh \
    test_i686-pc-cygwin-gcc-4.sh \
    test_i686-pc-mingw32-gcc-4.5.2.sh \
    test_i686-w64-mingw32-gcc-4.5.3.sh \
    test_x86_64-w64-mingw32-gcc-4.5.3.sh

$(info The list $(GCC_TEST_SCRIPT_TARGETS))

# The C test file.
C_TEST_SOURCE = $(srcdir)/test_source_file_1.c $(srcdir)/test_source_file_2.c
# The built C++ test files.
CPP_TEST_SOURCE = test_source_file_1.cpp test_source_file_2.cpp
CLEANFILES = $(CPP_TEST_SOURCE)

# List of scripts that will be use during "make check" and are not built.  Note that scripts are not
# distributed by default, hence the "dist_" prefix.
dist_check_SCRIPTS = test_template.sh

# List of scripts that will be use during "make check" and are built.  Note that scripts are not
# distributed by default.
check_SCRIPTS := test_cpp1.sh
CLEANFILES += $(check_SCRIPTS)

test_source_file_1.cpp: test_source_file_1.c
	@# Simply copy the C files to make C++ versions.
	cp $(srcdir)/test_source_file_1.c $@;

test_source_file_2.cpp: test_source_file_2.c
	@# Simply copy the C files to make C++ versions.
	cp $(srcdir)/test_source_file_2.c $@;

test_cpp1.sh: test_template.sh Makefile $(CPP_TEST_SOURCE)
	cat $(srcdir)/test_template.sh | sed -e 's,[@]gcc[@],gcc,g' \
	    -e 's,[@]test_source[@],\"${CPP_TEST_SOURCE}\",g' \
	> $@

# Static pattern rule for generating test scripts for testing coflo with all
# gcc versions found on the build system.
$(GCC_TEST_SCRIPT_TARGETS): test_%.sh: test_template.sh Makefile
	cat $(srcdir)/test_template.sh | "$(SED)" -e 's,[@]gcc[@],$*,g' \
	    -e 's,[@]test_source[@],\"${C_TEST_SOURCE}\",g' \
	> $@

# Tests to run for "make check".
#TESTS_ENVIRONMENT = export "TEST_CPPFLAGS=$(CPPFLAGS)" && export "TEST_CXXFLAGS=$(CXXFLAGS)" && $(SHELL) -x
TESTS = $(check_SCRIPTS) $(GCC_TEST_SCRIPT_TARGETS)
TEST_EXTENSIONS = .sh

SH_LOG_COMPILER = $(SHELL)
AM_SH_LOG_FLAGS =
