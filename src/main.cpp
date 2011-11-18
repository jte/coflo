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

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/version.hpp>
#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

// Include the config.h file generated by configure.
#include "../config.h"

#include "debug_utils/debug_utils.hpp"

#include "ResponseFileParser.h"

#include "Program.h"
#include "libexttools/ToolCompiler.h"
#include "libexttools/ToolDot.h"
#include "Analyzer.h"

// Define a shorter namespace alias for boost::program_options.
namespace po = boost::program_options;

/// @name Command line parameter strings.
/// Using defines for these because Boost.ProgramOptions uses them (generally) twice.
//@{
#define CLP_HELP	"help"
#define CLP_VERSION	"version"
#define CLP_BUILD_INFO	"build-info"
#define CLP_RESPONSE_FILE "response-file"
#define CLP_DEBUG_PARSE "debug-parse"
#define CLP_DEBUG_LINK  "debug-link"
#define CLP_DEBUG_CFG	"debug-cfg"
#define CLP_TEMPS_DIR	"temps-dir"
#define CLP_OUTPUT_DIR	"output-dir"

#define CLP_DEFINE	"define"
#define CLP_INCLUDE_DIR	"include-dir"

#define CLP_USE_GCC "use-gcc"
#define CLP_USE_DOT "use-dot"
#define CLP_USE_FILTER "use-filter"

#define CLP_PRINT_FUNCTION_CFG "cfg"
#define CLP_CFG_VERBOSE "cfg-verbose"
#define CLP_CFG_VERTEX_IDS "cfg-vertex-ids"

#define CLP_CONSTRAINT "constraint"

#define CLP_INPUT_FILE "input-file"
//@}

/**
 * Additional command-line parser for '@file' response files.
 * 
 * @param s String to check for a prefix of '@', indicating that this is a response file name.
 * @return If this isn't a '@filename' paramter, a std::pair<> of two empty strings.
 *         If it is, the pair <"response-file", filename>.
 */
std::pair<std::string, std::string> at_option_parser(const std::string &s)
{
    if ('@' == s[0])
	{
        return std::make_pair(std::string(CLP_RESPONSE_FILE), s.substr(1));
	}
    else
	{
        return std::pair<std::string, std::string>();
	}
}

/**
 * Prints the versions of the various libraries we were compiled against.
 */
static void print_lib_versions();

/**
 * CoFlo entry point.
 * 
 * @param argc Number of command line arguments.
 * @param argv Pointers to the command line strings.
 * @return 0 on success, non-zero on failure.
 */
int main(int argc, char* argv[])
{
	Program *the_program;
	Analyzer *the_analyzer;

	// Subprograms we'll need.
	std::string the_filter;
	std::string the_gcc;
	std::string the_dot;

	// Name of the response file if we get one on the command line.
	std::string response_filename;

	// The HTML report output directory.
	std::string report_output_directory;

	// Debug settings.
	bool debug_parse = false;
	bool debug_link = false;
	bool debug_cfg = false;
	
	// Control flow graph option flags.
	// Whether to limit display to only function calls, or to everything CoFlo
	// recognizes.
	bool cfg_verbose = false;
	// Enable or disable outputting vertex IDs.
	bool cfg_vertex_ids = false;

	// Declare a variables_map to take the command line options we're passed.
	po::variables_map vm;

	// Catch any exceptions thrown by the command-line parser.
	// Usually this will be an unrecognized parameter.
	try
	{
		// Declare the objects that will describe the supported options.
		// Declare general options.
		po::options_description general_options("General Options");
		// Declare preprocessing-related options.
		po::options_description preproc_options("Preprocessing Options");
		// Declare subprograms to use.
		po::options_description subprogram_options("Subprogram Options");
		// Rules to check.
		po::options_description analysis_options("Analysis Options");
		// Control Flow Graph options.
		po::options_description cfg_options("Control Flow Graph Options");
		// Options for debugging CoFlo itself and/or the program being analyzed.
		po::options_description debugging_options("Debugging Options");
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


		// Add the command-line options.
		general_options.add_options()
		(CLP_HELP, "Produce this help message.")
		(CLP_VERSION",v", "Display the version number and copyright information.")
		(CLP_BUILD_INFO, "Print information about library versions and options used to build this program.")
		(CLP_RESPONSE_FILE, po::value<std::string>(&response_filename), "Read command line options from file. Can also be specified with '@name'.")
		(CLP_TEMPS_DIR, po::value< std::string >(), "The directory in which to put intermediate files during the analysis.")
		(CLP_OUTPUT_DIR",O", po::value< std::string >(&report_output_directory), "Put HTML report output in the given directory.")
		;
		preproc_options.add_options()
		(CLP_DEFINE",D", po::value< std::vector<std::string> >(), "Define a preprocessing macro")
		(CLP_INCLUDE_DIR",I", po::value< std::vector<std::string> >(), "Add an include directory")
		;
		subprogram_options.add_options()
		(CLP_USE_FILTER, po::value< std::string >(&the_filter), "Pass all source through this filter prior to preprocessing and compiling.")
		(CLP_USE_GCC, po::value< std::string >(&the_gcc)->default_value("gcc"), "GCC to invoke.")
		(CLP_USE_DOT, po::value< std::string >(&the_dot)->default_value("dot"), "GraphViz dot program to use for drawing graphs.")
		;
		analysis_options.add_options()
		(CLP_CONSTRAINT, po::value< std::vector<std::string> >(), "\"f1() -x f2()\" : Warn if f1 can reach f2.")
		;
		cfg_options.add_options()
		(CLP_PRINT_FUNCTION_CFG, po::value< std::string >(), "Print the control flow graph of the given function to standard output.")
		(CLP_CFG_VERBOSE, po::bool_switch(&cfg_verbose),
				"Output all statements and nodes CoFlo finds in the control flow graph.  Default is to limit output to function calls and flow control constructs only.")
		(CLP_CFG_VERTEX_IDS, po::bool_switch(&cfg_vertex_ids), "Output numeric IDs of the control flow graph vertices.  Can help when comparing graphical and textual representations.")
		;
		debugging_options.add_options()
		(CLP_DEBUG_PARSE, po::bool_switch(&debug_parse), "Print debug info concerning the CFG parsing stage.")
		(CLP_DEBUG_LINK, po::bool_switch(&debug_link), "Print debug info concerning the CFG linking stage.")
		(CLP_DEBUG_CFG, po::bool_switch(&debug_cfg), "Print debug info concerning the CFG fix-up stages.")
		;
		hidden_options.add_options()
		(CLP_INPUT_FILE, po::value< std::vector<std::string> >(), "input file")
		;
		positional_options.add("input-file", -1);
		non_hidden_cmdline_options.add(general_options)
				.add(preproc_options)
				.add(subprogram_options)
				.add(analysis_options)
				.add(cfg_options)
				.add(debugging_options);
		cmdline_options.add(non_hidden_cmdline_options).add(hidden_options);
	
		// Parse the command line.
		po::store(po::command_line_parser(argc, argv).
			extra_parser(at_option_parser).
			options(cmdline_options).
			positional(positional_options).run(), vm);
	
		// Handle the parameters parsed so far, in particular CLP_RESPONSE_FILE, so that the response_filename variable
		// gets populated, so the check for response files below works properly.
		vm.notify();

		// Parse any response files.
		if(!response_filename.empty())
		{
			ResponseFileParser rfp;
			std::vector<std::string> args;

			// Parse the response file.
			rfp.Parse(response_filename, &args);

			// Store the options we found.
			// Note that since this store is called after the store of the parameters actually
			// presented on the command line, response-file params will be overridden by any command-line params.
			po::store(po::command_line_parser(args).options(cmdline_options).positional(positional_options).run(), vm);
		}

		// Call the notify() functions for any options.
		po::notify(vm);

		// See if the user is asking for help, or didn't pass any parameters at all.
		if (vm.count(CLP_HELP) || argc < 2)
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
		if (vm.count(CLP_VERSION))
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

		// See of the user is requesting build info.
		if(vm.count(CLP_BUILD_INFO))
		{
			std::cout << PACKAGE_STRING << std::endl;
			std::cout << "Copyright (C) 2011 Gary R. Van Sickle" << std::endl;
			std::cout << std::endl;
			std::cout << "Build info:" << std::endl;
			print_lib_versions();
			/// @todo Should add:
			/// - Build options
			/// - Build date
			/// - Build type (e.g. "Release", "Snapshot", etc.)
			/// - ???
			return 0;
		}

	}
	catch(std::exception &e)
	{
		// Something went wrong while trying to parse the command line.
		// Print an error message and exit.
		std::cout << "ERROR: Couldn't parse command line: " << e.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cout << "ERROR: Unknown exception" << std::endl;
		return 1;
	}

	if(vm.count(CLP_INPUT_FILE)>0)
	{
		try
		{
			// Enable/disable debug output.
			/// @todo Add debug_link control.
			dlog_block.enable(debug_parse);
			dlog_function.enable(debug_parse);
			dlog_cfg.enable(debug_cfg);

			the_program = new Program();
			the_analyzer = new Analyzer();

			const std::vector<std::string> *defines, *includes;
			if(vm.count(CLP_DEFINE)>0)
			{
				defines = &(vm[CLP_DEFINE].as< std::vector<std::string> >());
			}
			else
			{
				defines = new std::vector<std::string>();
			}
			if(vm.count(CLP_INCLUDE_DIR)>0)
			{
				includes = &(vm[CLP_INCLUDE_DIR].as< std::vector<std::string> >());
			}
			else
			{
				includes = new std::vector<std::string>();
			}

			the_program->SetTheFilter(the_filter);
			ToolCompiler *tool_compiler = new ToolCompiler(the_gcc);
			std::cout << "Using GCC version: " << tool_compiler->GetVersion() << std::endl;

			// Check if this version of GCC is going to work.
			std::string gcc_version_check_string;
			bool gcc_ver_ok;
			boost::tie(gcc_version_check_string, gcc_ver_ok) = tool_compiler->CheckIfVersionIsUsable();
			if(gcc_ver_ok == false)
			{
				std::cerr << "ERROR: " << gcc_version_check_string << std::endl;
				return 1;
			}
			the_program->SetTheGcc(tool_compiler);
			the_program->AddSourceFiles(vm[CLP_INPUT_FILE].as< std::vector<std::string> >());

			// Parse the program.
			T_ID_TO_FUNCTION_CALL_UNRESOLVED_MAP unresolved_function_calls;
			if(!the_program->Parse(
				*defines,
				*includes,
				&unresolved_function_calls,
				debug_parse))
			{
				// Parse failed.
				return 1;
			}

			the_program->PrintUnresolvedFunctionCalls(&unresolved_function_calls);
		}
		catch( boost::exception & e )
		{
			std::cerr << "ERROR: EXCEPTION CAUGHT: " << boost::diagnostic_information(e) << std::endl;
			return 1;
		}
		catch(...)
		{
			std::cerr << "ERROR: Unknown exception: " << std::endl;
			return 1;
		}
	}


	if(vm.count(CLP_PRINT_FUNCTION_CFG))
	{
		// User wants a control flow graph.
		if(!the_program->PrintFunctionCFG(vm[CLP_PRINT_FUNCTION_CFG].as<std::string>(), cfg_verbose, cfg_vertex_ids))
		{
			// Something went wrong.
			return 1;
		}
	}

	the_analyzer->AttachToProgram(the_program);

	if(vm.count(CLP_CONSTRAINT) > 0)
	{
		// Add the given constraints to the analysis.
		the_analyzer->AddConstraints(vm[CLP_CONSTRAINT].as< std::vector<std::string> >());
	}
	
	// Perform the analysis.
	the_analyzer->Analyze();
	
	if(!report_output_directory.empty())
	{
		// User wants HTML output.
		ToolDot *tool_dot = new ToolDot(the_dot);
		the_program->SetTheDot(tool_dot);
		std::cout << "Using Dot version: " << tool_dot->GetVersion() << std::endl;
		the_program->Print(report_output_directory);
	}

	return 0;
}

static void print_lib_versions()
{
	using std::cout;
	using std::endl;

	cout << "Boost library info:" << endl;
	cout << "  Version.............: " << (BOOST_VERSION / 100000) << "." << ((BOOST_VERSION / 100) % 1000) << "." << (BOOST_VERSION % 100) << endl;
	cout << "  Compiler string.....: " << BOOST_COMPILER << endl;
	cout << "  Using standard lib..: " << BOOST_STDLIB << endl;
	cout << "  Detected platform...: " << BOOST_PLATFORM << endl;
	cout << endl;
	cout << "Boost.Program_Options:" << endl;
	cout << "  Version.............: " << BOOST_PROGRAM_OPTIONS_VERSION << endl;
}
