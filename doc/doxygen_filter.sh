#! /bin/sh

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

# INPUT_FILTER for Doxygen.

# Abort if any command fails.
set -e;

# Get the input filename off the command line.
INPUT_FILENAME="$(cygpath -u "${1}")";

# Check if we're being passed a Win32 path.  This can happen if we're running on Cygwin,
# but the Doxygen we're being called by is native Windows.
if [ "$(echo "${INPUT_FILENAME}" | grep -c -E '[[:alpha:]]:|\\')" == "1" ]; then
	# We have a Windows path on our hands.  Convert it to a Cygwin POSIX-like path before sending it
	# to sed.
	INPUT_FILENAME="$(cygpath -u "${INPUT_FILENAME}")";
fi

# Escape a number of common abbreviations so Doxygen doesn't break paragraphs because of them.
sed -r -e 's,(i\.e\.),\1\\ ,gi' "${INPUT_FILENAME}";
