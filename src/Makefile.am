# Second-level Makefile.am for coflo
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

bin_PROGRAMS = coflo
coflo_SOURCES = main.cpp \
	Function.cpp Function.h \
	Block.cpp Block.h

coflo_CPPFLAGS = $(BOOST_CPPFLAGS)
coflo_LDFLAGS = $(BOOST_LDFLAGS) -Wl,--enable-auto-import
coflo_LDADD = $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB)
