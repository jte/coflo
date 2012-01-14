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

#include <string.h>
#include <stdio.h>

/**
 * This is the volatile variable which decides whether the function is threadsafe or not.
 */
volatile int external_forces;

void SometimesSafePrint(char *string, int integer)
{
	if(external_forces)
	{
		// We can print safely by copying the params and calling the (assumed threadsafe) printf().
		char *copy_of_string = strdup(string);
		int copy_of_int = integer;

		printf(copy_of_string);
		printf("%d\n", copy_of_int);		
	}
	else
	{
		RarelySafePrint(string, integer);
	}
}
