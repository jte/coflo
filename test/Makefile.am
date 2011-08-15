# Second-level Makefile.am for CoFlo's ./test directory.
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

# Test at most the first 16 GCCs that we find installed on the system.
MAX_GCCS_TO_TEST=16

# Add scripts which will be generated and used during "make check" to check_SCRIPTS.
# Note that scripts are not distributed by default, so we don't need a "nodist_" prefix here.
check_SCRIPTS = 
EXTRA_DIST = testsuite.at $(srcdir)/package.m4 $(TESTSUITE) atlocal.in $(C_TEST_SOURCE)
CLEANFILES = $(CPP_TEST_SOURCE)
TESTSUITE = $(srcdir)/testsuite

###
### Rules to hook the AutoTest functionality into Automake.
###
check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)
     
installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(bindir)' \
	$(TESTSUITEFLAGS)
     
clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean
     
AUTOM4TE = $(SHELL) $(top_srcdir)/build-aux/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest

# The `:;' works around a Bash 3.2 bug when the output is not writeable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	:;{ \
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
	echo 'm4_define([AT_PACKAGE_NAME],' && \
	echo '  [$(PACKAGE_NAME)])' && \
	echo 'm4_define([AT_PACKAGE_TARNAME],' && \
	echo '  [$(PACKAGE_TARNAME)])' && \
	echo 'm4_define([AT_PACKAGE_VERSION],' && \
	echo '  [$(PACKAGE_VERSION)])' && \
	echo 'm4_define([AT_PACKAGE_STRING],' && \
	echo '  [$(PACKAGE_STRING)])' && \
	echo 'm4_define([AT_PACKAGE_BUGREPORT],' && \
	echo '  [$(PACKAGE_BUGREPORT)])'; \
	echo 'm4_define([AT_PACKAGE_URL],' && \
	echo '  [$(PACKAGE_URL)])'; \
	} > '$(srcdir)/package.m4'

# List testsuite.at's dependencies (i.e. the files it includes)
$(srcdir)/testsuite.at:

###
### The rule which makes the "testsuite" shell script.
###
$(TESTSUITE): $(srcdir)/testsuite.at $(srcdir)/package.m4 $(CPP_TEST_SOURCE) $(C_TEST_SOURCE)
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@


###
### Some other files that we need for the tests.
###

# The C test files.
C_TEST_SOURCE = $(srcdir)/test_source_file_1.c $(srcdir)/test_source_file_2.c
# The built C++ test files.
CPP_TEST_SOURCE = test_source_file_1.cpp test_source_file_2.cpp
CLEANFILES += $(CPP_TEST_SOURCE)

test_source_file_1.cpp: $(srcdir)/test_source_file_1.c
	@# Simply copy the C files to make C++ versions.
	cp $(srcdir)/test_source_file_1.c $@;

test_source_file_2.cpp: $(srcdir)/test_source_file_2.c
	@# Simply copy the C files to make C++ versions.
	cp $(srcdir)/test_source_file_2.c $@;
