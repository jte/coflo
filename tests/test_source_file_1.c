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
 * @file
 * This is a do-nothing sample C program used to test CoFlo.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef double (*T_FUNCTION_PTR)(void);

static pthread_mutex_t f_mutex_protecting_thread_unsafe_function = PTHREAD_MUTEX_INITIALIZER;

int some_global_variable = 1;

extern volatile int some_variable_set_by_an_isr;
extern volatile int g_external_variable;

double function_that_we_will_call_through_a_ptr(void)
{
    return (some_variable_set_by_an_isr + g_external_variable);
}


/**
 * External function declarations.
 */
extern void do_this();
extern void do_that();
extern long last_function_called_by_main(long);

int another_level_deep() { return 1; };
int function_a() { another_level_deep(); return 1; };
int function_b() {return 2;};
int function_c() {return 3;};
static int function_w() {return 4;};
int function_y() {return 5;};
int function_z() {return some_global_variable;};




long threadsafe_printf_wrapper(int which_thread)
{
    pthread_mutex_lock(&f_mutex_protecting_thread_unsafe_function);
    printf("Now printing from thread %d\n", which_thread);
    pthread_mutex_unlock(&f_mutex_protecting_thread_unsafe_function);
}

extern int calculate(int x);

int test_func(void)
{
	return 1;
}

int __attribute__ ((warn_unused_result)) predicate_1(void) 
{
	return some_variable_set_by_an_isr;
}

void _Pragma("is_an_isr") /*is_an_isr*/ ISR1(void)
{
    // ERROR: This ISR shouldn't be calling any functions which try to lock mutexes
    // or call printf().
    threadsafe_printf_wrapper(9);
}

int one_if(int val)
{
	int x = val;

	if(x > val)
	{
		printf("Yes\n");

		if(x > 6)
		{
			printf("t");
		}
	}
	else
	{
		printf("No\n");
		return 4;
	}

	printf("h");
	exit(2);

	return x;
}

int main()
{
	int retval;
	double a_double_val;
	T_FUNCTION_PTR a_function_ptr = function_that_we_will_call_through_a_ptr;
	
	if(test_func()+function_a()+calculate(function_w()))
	{
		retval = function_a();
		retval = function_b();
		retval = function_c();
	}
	else
	{
		while(predicate_1())
		{
			retval = function_w();
			retval = function_y();
			retval = function_z();
		}

		if(g_external_variable)
		{
			do_this();
		}
		else
		{
			do_that();
		}

		if(g_external_variable)
		{
			// This if() has no else.
			printf("if with no else\n");
		}
	}
	
Label1:

	switch(1)
	{
		case 1:
		{
			function_a();
			break;
		}
		case 2:
		{
			function_b();
			exit(0);
		}
		case 3:
		{
			function_c();
			break;
		}
		default:
		{
			// Call a function through a pointer.
			printf("Double value: %f", a_function_ptr());
			break;
		}
	}
	
	if(g_external_variable && predicate_1())
	{
		printf("OR Test\n");
	}

	last_function_called_by_main(88);

	return retval;
}
