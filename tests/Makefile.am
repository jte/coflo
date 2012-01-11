# Second-level Makefile.am for CoFlo's ./tests directory.
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

## Process this file with automake to produce Makefile.in.

# Test at most the first 16 GCCs that we find installed on the system.
MAX_GCCS_TO_TEST=16

EXTRA_DIST = $(TESTSUITE_AT) $(TESTSUITE) $(srcdir)/package.m4 atlocal.in $(C_TEST_SOURCE) \
	test_src_1/main.c \
	test_src_1/Layer1.c test_src_1/Layer1.h \
	test_src_1/Thread1.c \
	test_src_1/Thread1.h \
	test_src_1/Thread2.c \
	test_src_1/Thread2.h \
	test_src_1/RarelySafePrint.c test_src_1/RarelySafePrint.h \
	test_src_1/ThreadUnsafeFunctions.c test_src_1/ThreadUnsafeFunctions.h
DISTCLEANFILES = atconfig
MAINTAINERCLEANFILES = Makefile.in $(TESTSUITE)
CLEANFILES = $(CPP_TEST_SOURCE)
     
# This line appears in the Autoconf/Autotest manual, but see configure.ac.
# The "AM_MISSING_PROG([AUTOM4TE], [autom4te])" line takes care of this,
# though the "AM_MISSING_PROG" macro is not documented.
#AUTOM4TE = $(SHELL) $(top_srcdir)/build-aux/missing --run autom4te


# The `:;' works around a Bash 3.2 bug when the output is not writeable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	@ :;{ \
	echo '# Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).' && \
	echo '#' && \
	echo '# This file is part of CoFlo.' && \
	echo '#' && \
	echo '# CoFlo is free software: you can redistribute it and/or modify it under the' && \
	echo '# terms of version 3 of the GNU General Public License as published by the Free' && \
	echo '# Software Foundation.' && \
	echo '#' && \
	echo '# CoFlo is distributed in the hope that it will be useful, but WITHOUT ANY' && \
	echo '# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A' && \
	echo '# PARTICULAR PURPOSE.  See the GNU General Public License for more details.' && \
	echo '#' && \
	echo '# You should have received a copy of the GNU General Public License along with' && \
	echo '# CoFlo.  If not, see <http://www.gnu.org/licenses/>.' &&\
	echo '#' && \
	echo '# THIS FILE GENERATED, DO NOT EDIT.' && \
	echo '# Signature of the current package.' && \
	echo 'm4_define([AT_PACKAGE_NAME],      [$(PACKAGE_NAME)])' && \
	echo 'm4_define([AT_PACKAGE_TARNAME],   [$(PACKAGE_TARNAME)])' && \
	echo 'm4_define([AT_PACKAGE_VERSION],   [$(PACKAGE_VERSION)$(VC_REV)])' && \
	echo 'm4_define([AT_PACKAGE_STRING],    [$(PACKAGE_STRING)$(VC_REV)])' && \
	echo 'm4_define([AT_PACKAGE_BUGREPORT], [$(PACKAGE_BUGREPORT)])' && \
	echo 'm4_define([AT_PACKAGE_URL],       [$(PACKAGE_URL)])' ; \
	} > '$(srcdir)/package.m4'

# The .at files which make up the test suite.
TESTSUITE_AT = \
	connectedness.at \
	constraints.at \
	structuring_compound_conditions.at \
	testsuite.at \
	version.at

# The actual shell script which is the testsuite.
TESTSUITE = $(srcdir)/testsuite

# Our AutoTest compiler is really Autom4te with "--language=autotest".
AUTOTEST = $(AUTOM4TE) --language=autotest

###
### The rule which makes the "testsuite" shell script.
### Note that this script is distributed, and ordinarily anyone using a distribution
### shouldn't need to or want to rebuild it.  "make distcheck" will fail in cryptic
### ways if it is for some reason rebuilt:
###   - "$@.tmp" points to a file in $(srcdir)/tests/, which during "make distcheck" is read-only, so autom4te fails.
###   - Similarly, "$(srcdir)/tests/testsuite" can't be overwritten, so the mv will fail.
### To avoid this, don't make $(TESTSUITE) depend on non-distributed built files.
### See <http://sources.redhat.com/automake/automake.html#distcleancheck> for more info.
###
$(TESTSUITE): $(srcdir)/package.m4 $(TESTSUITE_AT)
	$(AUTOTEST) -I '$(srcdir)' $(srcdir)/testsuite.at -o $@.tmp
	mv $@.tmp $@
	
# It appears that at least up to autoconf 2.68, atconfig doesn't get a dependency
# and rebuild rule generated for it.  Do that manually here.
atconfig: $(top_builddir)/config.status
	cd $(top_builddir) && ./config.status tests/$@

###
### Rules to hook the AutoTest functionality into Automake.
###
     
clean-local:
	test ! -f '$(TESTSUITE)' ||	$(SHELL) '$(TESTSUITE)' --clean
	
check-local: atconfig atlocal $(TESTSUITE) $(CPP_TEST_SOURCE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)
     
installcheck-local: atconfig atlocal $(TESTSUITE) $(CPP_TEST_SOURCE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) AUTOTEST_PATH='$(bindir)'


###
### Some other files that we need for the tests.
###

# The C test files.
C_TEST_SOURCE = $(srcdir)/test_source_file_1.c $(srcdir)/test_source_file_2.c $(srcdir)/loop_test_1.c \
	compound_condition_1.c \
	compound_condition_2.c \
	compound_condition_3.c \
	compound_condition_4.c \
	infinite_loop_simple.c
	
# The built C++ test files.
CPP_TEST_SOURCE = test_source_file_1.cpp test_source_file_2.cpp

test_source_file_1.cpp: $(srcdir)/test_source_file_1.c
	@# Simply copy the C files to make C++ versions.
	cp $(srcdir)/test_source_file_1.c $@;

test_source_file_2.cpp: $(srcdir)/test_source_file_2.c
	@# Simply copy the C files to make C++ versions.
	cp $(srcdir)/test_source_file_2.c $@;
