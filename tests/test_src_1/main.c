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

/**
 * @file Sample program for testing CoFlo.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "Thread1.h"
#include "Thread2.h"

int main()
{
	int retval1, retval2;
	pthread_t thread1, thread2;

	if( (retval1=pthread_create( &thread1, NULL, &ThreadBody1, NULL)) )
	{
		printf("Thread creation failed: %d\n", retval1);
	}

	if( (retval2=pthread_create( &thread2, NULL, &ThreadBody2, NULL)) )
	{
		printf("Thread creation failed: %d\n", retval2);
	}
	
	/* Wait for the threads to terminate. */
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);
	
	return 0;
}

