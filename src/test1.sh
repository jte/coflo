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

#
# Created on Apr 3, 2011, 2:02:20 AM
#

# Run CoFlo on some test code.
# Note that the "-blocks" option is required to make both gcc 4.3.4 and 4.4.1 emit
# the same BLOCK/PRED/SUCC notations in the .cfg file (4.3.4 does it without -blocks).
TEST_SOURCE=test_source_file_1.c
#TEST_SOURCE=bzip2.c
gcc -c -fdump-tree-cfg-lineno-blocks "${srcdir}/../test/${TEST_SOURCE}"
./coflo ../src/${TEST_SOURCE}.013t.cfg
