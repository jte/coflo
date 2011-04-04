/**
 * Copyright 2011 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of coflo.
 *
 * coflo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * coflo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * coflo.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * This is a do-nothing sample C program used to test CoFlo.
 */

int x = 1;

int a() {return 1;};
int b() {return 2;};
int c() {return 3;};
int w() {return 4;};
int y() {return 5;};
int z() {return 6;};

extern int calculate(int x);

int test_func(void)
{
	return 1;
}

int main()
{
	int retval;
	
	if(test_func()+a()+calculate(w()))
	{
		retval = a();
		retval = b();
		retval = c();
	}
	else
	{
		retval = w();
		retval = y();
		retval = z();
	}
	
	switch(1)
	{
		case 1:
		{
			a();
			break;
		}
		case 2:
		{
			b();
			break;
		}
		case 3:
		{
			c();
			break;
		}
		default:
		{
			break;
		}
	}
	
	return retval;
}
