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

/** @file */
#if 0
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#endif
 #include <cppunit/extensions/TestFactoryRegistry.h>
 #include <cppunit/CompilerOutputter.h>
 #include <cppunit/TestResult.h>
 #include <cppunit/TestResultCollector.h>
 #include <cppunit/TestRunner.h>
 //#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>


extern int PullInMyLibrary();

int main(int argc, char* argv[])
{
	int val = PullInMyLibrary();

	// Create the event manager and test controller
	CppUnit::TestResult controller;
	// Add a listener that colllects test result
	   CppUnit::TestResultCollector result;
	   controller.addListener( &result );
	   // Add a listener that print dots as test run.
	      //CppUnit::TextTestProgressListener progress;
	      //controller.addListener( &progress );
	      CppUnit::BriefTestProgressListener brief_progress;
	      controller.addListener( &brief_progress );


	// Create the test runner.
	CppUnit::TestRunner runner;

	// Get a reference to the test registry.
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

	// Get the top level suite from the registry
	CppUnit::Test *suite = registry.makeTest();

	// Adds the test to the list of test to run
	runner.addTest( suite );

	try
	{
		std::cout << "Running tests" << std::endl;
		runner.run(controller,"");

	// Change the default outputter to a compiler error format outputter
	CppUnit::CompilerOutputter outputter( &result, std::cerr );
	outputter.write();
	}
	catch (...)
	{
	     std::cerr  <<  std::endl
	                <<  "ERROR: "  /*<<  e.what()*/
	                << std::endl;
	     return 0;
	}

	// Return error code 1 if the one of test failed.
	return result.wasSuccessful() ? 0 : 1;
}


