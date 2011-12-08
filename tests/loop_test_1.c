/*
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
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
 * A simple program to test CoFlo's loop handling.
 *
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{
	int i = 6;

	printf("Start.\n");

	while(argc > 6)
	{
		printf("i = %d\n", i);
	}

	if(argc > 1)
	{
		if(i>0)
		{
			i--;
			printf("i = %d\n", i);

			if(i+argc > 7)
			{
				printf("here\n");
			}
		}
	}

	printf("Finished.\n");
	
	dummy(1,3,4,5,"hello");

	return 0;
}


