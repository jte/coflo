/** \mainpage
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
namespace bf = boost::filesystem;


/**
 * CoFlo entry point.
 * 
 * @param argc Number of command line arguments.
 * @param argv Pointers to the command line strings.
 * @return 0 on success, non-zero on failure.
 */
int main(int argc, char* argv[])
{
	TranslationUnit *tu;
	std::string the_filter;
	std::string the_gcc;
	std::string the_dot;
	std::string the_ctags;
	
	// Declare the supported options object.
	po::options_description options("Options");
	// Declare preprocessing-related options.
	po::options_description preproc_options("Preprocessing Options");
	// Declare subprograms to use.
	po::options_description subprogram_options("Subprogram Options");
	// Declare options_description object for options which won't be shown to
	// the user in the help message.
	po::options_description hidden_options("Hidden options");
	// Declare a positional_options_description object to translate un-switched
	// options on the command line into "--input-file=<whatever>" options.
	po::positional_options_description positional_options;
	// Almost all the command-line options, except for the hidden ones (for printing usage).
	po::options_description non_hidden_cmdline_options;
	// All command-line options descriptions.
	po::options_description cmdline_options;
	// Declare a variables_map to take the command line options we're passed.
	po::variables_map vm;
	
	// Add the command-line options.
	options.add_options()
    ("help", "Produce this help message.")
    ("version,v", "Print version string.")
	("debug-parse", "Print debug info concerning the CFG parsing stage.")
	("debug-link", "Print debug info concerning the CFG linking stage.")
	("output-dir", po::value< std::string >(), "Put output in the given directory.")
	;
	preproc_options.add_options()
	("define,D", po::value< std::vector<std::string> >(), "Define a preprocessing macro")
	("include-dir,I", po::value< std::vector<std::string> >(), "Add an include directory")
	;
	subprogram_options.add_options()
	("use-filter", po::value< std::string >(&the_filter), "Pass all source through this filter prior to preprocessing and compiling.")
	("use-gcc", po::value< std::string >(&the_gcc)->default_value("gcc"), "GCC to invoke.")
	("use-ctags", po::value< std::string >(&the_ctags)->default_value("ctags"), "The ctags program to invoke.")
	("use-dot", po::value< std::string >(&the_dot)->default_value("dot"), "GraphViz dot program to use for drawing graphs.")
	;
	hidden_options.add_options()
	("input-file", po::value< std::vector<std::string> >(), "input file")
	;
	positional_options.add("input-file", -1);
	non_hidden_cmdline_options.add(options).add(preproc_options).add(subprogram_options);
	cmdline_options.add(non_hidden_cmdline_options).add(hidden_options);

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
		std::cout << "Usage: coflo [options] file..." << std::endl;
		std::cout << non_hidden_cmdline_options << std::endl;
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
			T_ID_TO_FUNCTION_PTR_MAP function_map;

			// Parse this file.
			std::cout << "Parsing \"" << input_file << "\"..." << std::endl;
			bool retval = tu->ParseFile(input_file, &function_map,
									 the_filter, the_gcc, the_ctags, static_cast<bool>(vm.count("debug-parse")));
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
			
			// Link the function calls.
			//std::cout << "Linking function calls..." << std::endl;
			//tu->Link(function_map);
			
			// Create the control-flow graphs.
			std::cout << "Creating function control-flow graphs..." << std::endl;
			tu->CreateControlFlowGraphs();
			
			// Link the function calls.
			/// @todo Link doesn't work after CreateControlFlowGraphs() for some reason.
			std::cout << "Linking function calls..." << std::endl;
			tu->Link(function_map);	
			
			if(vm.count("output-dir"))
			{
				tu->Print(the_dot, vm["output-dir"].as<std::string>());
			}
		}
	}

	return 0;
}
