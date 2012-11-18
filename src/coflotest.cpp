/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of CoFlo.
 *
 * CoFlo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * CoFlo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * CoFlo.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 * main() entry point for the coflotest unit test executable.
 */

#include "gtest/gtest.h"

/// @name Dummy functions pulled in solely to trick the linker into not optimizing away the test libraries.
/// If you get link errors when building via "make check", this is probably the problem.
///@{
extern int PullInMyLibrary();
extern int GetMeToo();
///@}

int main(int argc, char* argv[])
{
	int val = PullInMyLibrary();
	int val2 = GetMeToo();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


