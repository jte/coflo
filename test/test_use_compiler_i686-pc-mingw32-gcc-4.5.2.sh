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

# Abort if there are any errors.
set -e

#USE_GCC=i686-w64-mingw32-gcc.exe
USE_GCC=i686-pc-mingw32-gcc-4.5.2.exe

# Run CoFlo on some test code.
TEST_SOURCE="test_source_file_1.c test_source_file_2.c"
#TEST_SOURCE=bzip2.c
#TEST_SOURCE=../src/main.cpp
cd ../test/ && ../src/coflo --use-gcc=${USE_GCC} ${TEST_SOURCE} --output-dir=../test_html/
