#!/bin/sh
# 
# Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
#
# This file is part of coflo.
#
# coflo is free software: you can redistribute it and/or modify it under the
# terms of version 3 of the GNU General Public License as published by the Free
# Software Foundation.
#
# coflo is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# coflo.  If not, see <http://www.gnu.org/licenses/>.

#
# Created on Apr 3, 2011, 2:02:20 AM
#

# Run coflo on itself.
#gcc -fdump-tree-cfg-lineno "${srcdir}"/main.cpp
./coflo ../src/main.cpp.013t.cfg