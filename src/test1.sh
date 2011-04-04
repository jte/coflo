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

# Run CoFlo on itself.
gcc -c -fdump-tree-cfg-lineno "${srcdir}/test/test_source_file_1.c"
./coflo ../src/test_source_file_1.c.013t.cfg
