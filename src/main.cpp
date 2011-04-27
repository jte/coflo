/**
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

#include <string>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include "TranslationUnit.h"

// Define a shorter namespace alias for boost::program_options.
namespace po = boost::program_options;

/**
 * Our entry point.
 * 
 * @param argc
 * @param argv
 * @return 0 on success, non-zero on failure.
 */
int main(int argc, char* argv[])
{
	TranslationUnit *tu;
	// Declare the supported options object.
	po::options_description options("Options");
	// Declare options_description object for options which won't be shown to
	// the user in the help message.
	po::options_description hidden_options("Hidden options");
	// Declare a positional_options_description object to translate un-switched
	// options on the command line into "--input-file=<whatever>" options.
	po::positional_options_description positional_options;
	// Combined options description.
	po::options_description cmdline_options;
	// Declare a variables_map to take the command line options we're passed.
	po::variables_map vm;
	
	// Add the command-line options.
	options.add_options()
    ("help", "Produce this help message.")
    ("version,v", "Print version string.")
	("debug-parse", "Print debug info concerning the CFG parsing stage.")
	("debug-link", "Print debug info concerning the CFG linking stage.")
	;
	hidden_options.add_options()
	("input-file", po::value< std::vector<std::string> >(), "input file")
	;
	positional_options.add("input-file", -1);
	cmdline_options.add(options).add(hidden_options);

	// Parse the command line.
	po::store(po::command_line_parser(argc, argv).
		options(cmdline_options).
		positional(positional_options).run(), vm);
	/// \todo Add response file (@file) parsing, maybe parse_environment(), parse_config_file().
	po::notify(vm);    

	// See if the user is asking for help.
	if (vm.count("help")) 
	{
		std::cout << PACKAGE_STRING << std::endl;
		std::cout << std::endl;
		std::cout << options << std::endl;
		std::cout << "Report bugs to: " << PACKAGE_BUGREPORT << std::endl;
		std::cout << PACKAGE_NAME << " home page: <" << PACKAGE_URL << ">" << std::endl;
		return 0;
	}
	
	// See if user is requesting version.
	if (vm.count("version"))
	{
		// Print version info per GNU Coding Standards <http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion>.
		// PACKAGE_STRING comes from autoconf, and has the format "CoFlo X.Y".
		std::cout << PACKAGE_STRING << std::endl;
		std::cout << "Copyright (C) 2011 Gary R. Van Sickle" << std::endl;
		std::cout << "License GPLv3: GNU GPL version 3 <http://gnu.org/licenses/gpl.html>" << std::endl;
		std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
		std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
		
		return 0;
	}
	
	if(vm.count("input-file")>0)
	{
		BOOST_FOREACH(std::string input_file, vm["input-file"].as< std::vector<std::string> >())
		{
			tu = new TranslationUnit();

			// Parse this file.
			std::cout << "Parsing \"" << input_file << "\"..." << std::endl;
			bool retval = tu->ParseFile(input_file, static_cast<bool>(vm.count("debug-parse")));
			if(retval == false)
			{
				std::cerr << "ERROR: Couldn't parse \"" << input_file << "\"" << std::endl;
				return 1;
			}

			// Link the blocks in the functions in the file.
			std::cout << "Linking basic blocks..." << std::endl;
			retval = tu->LinkBasicBlocks();
			if(retval == false)
			{
				std::cerr << "ERROR: Couldn't parse \"" << input_file << "\"" << std::endl;
				return 1;
			}
	
			tu->Print();
		}
	}

	return 0;
}
