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
 
#include <stdio.h>
#include "Layer1.h"
 
void* ThreadBody2(void* arg)
{
	int i;
	for(i=0; i<100; i++)
	{
		if(arg != NULL)
		{
			printf("Thread 2: %d\n", i);
		}
		else
		{
			SometimesSafePrint("Thread 2: ", i);
		}
	}
	
	return NULL;
}
