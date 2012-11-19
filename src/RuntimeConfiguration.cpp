/*
 * Copyright 2011, 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#include "RuntimeConfiguration.h"

#include <pwd.h>
#include <sys/stat.h>
//#include <boost/filesystem.hpp>

#include "ResponseFileParser.h"


// Define a shorter namespace alias for boost::program_options.
namespace po = boost::program_options;

/**
 * Prints the versions of the various libraries we were compiled against.
 */
static void print_build_info()
{
	/// @todo Should add:
	/// - Build options
	/// - Build date
	/// - Build type (e.g. "Release", "Snapshot", etc.)
	/// - Compiler used
	/// - ???

	using std::cout;
	using std::endl;

	cout << "System triples:" << endl;
	cout << "  Build...............: " << SYSTEM_TRIPLE_BUILD << endl;
	cout << "  Host................: " << SYSTEM_TRIPLE_HOST << endl;
	cout << endl;
	cout << "Boost library info:" << endl;
	cout << "  Version.............: " << (BOOST_VERSION / 100000) << "." << ((BOOST_VERSION / 100) % 1000) << "." << (BOOST_VERSION % 100) << endl;
	cout << "  Compiler string.....: " << BOOST_COMPILER << endl;
	cout << "  Using standard lib..: " << BOOST_STDLIB << endl;
	cout << "  Detected platform...: " << BOOST_PLATFORM << endl;
	cout << endl;
	cout << "Boost.Program_Options:" << endl;
	cout << "  Version.............: " << BOOST_PROGRAM_OPTIONS_VERSION << endl;
}


/**
 * Additional command-line parser for '\@file' response files.
 *
 * @param s String to check for a prefix of '@', indicating that this is a response file name.
 * @return If this isn't a '\@filename' paramter, a std::pair<> of two empty strings.
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


RuntimeConfiguration::RuntimeConfiguration(int argc, char *argv[])
{
	m_was_help_or_version = false;
	ParseAllOptionSources(argc, argv);
}

RuntimeConfiguration::~RuntimeConfiguration()
{
	// TODO Auto-generated destructor stub
}

void RuntimeConfiguration::ParseAllOptionSources(int argc, char* argv[])
{
	//// Declare the objects that will describe the supported options.

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
	// Options which are only allowed in a configuration file.
	po::options_description conf_file_options("~/.coflo.conf options");
	// Declare a positional_options_description object to translate un-switched
	// options on the command line into "--input-file=<whatever>" options.
	po::positional_options_description positional_options;
	// Almost all the command-line options, except for the hidden ones (for printing usage).
	po::options_description non_hidden_cmdline_options;

	// All command-line options descriptions.
	po::options_description cmdline_options;

	// All configuration file options valid in the PREFIX/etc/coflo.conf.
	po::options_description sysconf_config_file_options;

	// All configuration file options valid in ~/.coflo.conf.
	po::options_description user_config_file_options;

	// Add the command-line options.
	general_options.add_options()
	(CLP_HELP, "Produce this help message.")
	(CLP_VERSION",v", "Display the version number and copyright information.")
	(CLP_BUILD_INFO, "Print information about library versions and options used to build this program.")
	(CLP_RESPONSE_FILE, po::value<std::string>(&response_filename), "Read command line options from file. Can also be specified with '@name'.")
	(CLP_TEMPS_DIR, po::value< std::string >(), "The directory in which to put intermediate files during the analysis.")
	(CLP_OUTPUT_DIR",O", po::value< std::string >(), "Put HTML report output in the given directory.")
	;
	preproc_options.add_options()
	(CLP_DEFINE",D", po::value< std::vector<std::string> >(), "Define a preprocessing macro")
	(CLP_INCLUDE_DIR",I", po::value< std::vector<std::string> >(), "Add an include directory")
	;
	subprogram_options.add_options()
	(CLP_USE_FILTER, po::value< std::string >(), "Pass all source through this filter prior to preprocessing and compiling.")
	(CLP_USE_GCC, po::value< std::string >()->default_value("gcc"), "GCC to invoke.")
	(CLP_USE_DOT, po::value< std::string >()->default_value("dot"), "GraphViz dot program to use for drawing graphs.")
	;
	analysis_options.add_options()
	(CLP_CONSTRAINT, po::value< std::vector<std::string> >(), "\"f1() -x f2()\" : Warn if f1 can reach f2.")
	;
	cfg_options.add_options()
	(CLP_PRINT_FUNCTION_CFG, po::value< std::string >(), "Print the control flow graph of the given function to standard output.")
	(CLP_CFG_FMT, po::value< std::string >()->default_value("txt"), "Specifies the control flow graph output format.\n"
			"Valid values are:\n"
			"  txt: \tPrints a textual representation of the CFG to stdout.\n"
			"  dot: \tGenerates a dot file which can be used as input to the Graphviz dot program.\n"
			"  img: \tGenerates a graphical representation of the CFG using the Graphviz dot program.  Image file format is PNG."
			)
	(CLP_CFG_OUTPUT_FILENAME",o", po::value<std::string>()->default_value("output_file"), "Output filename.")
	(CLP_CFG_VERBOSE, po::bool_switch()->default_value(false),
			"Output all statements and nodes CoFlo finds in the control flow graph.  Default is to limit output to function calls and flow control constructs only.")
	(CLP_CFG_VERTEX_IDS, po::bool_switch()->default_value(false), "Output numeric IDs of the control flow graph vertices.  Can help when comparing graphical and textual representations.")
	;
	debugging_options.add_options()
	(CLP_DEBUG_PARSE, po::bool_switch()->default_value(false), "Print debug info concerning the CFG parsing stage.")
	(CLP_DEBUG_LINK, po::bool_switch()->default_value(false), "Print debug info concerning the CFG linking stage.")
	(CLP_DEBUG_CFG, po::bool_switch()->default_value(false), "Print debug info concerning the CFG fix-up stages.")
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

	conf_file_options.add_options()
			(CLP_CONF_FILE_FORMAT_VERSION, po::value<std::string>()->required(), "Conf file format version.")
			;
	user_config_file_options.add(conf_file_options);

	/**
	 * Setup is done.  Now parse the runtime configurations sources in the following order:
	 * - Command line
	 * - User's ~/.coflo.conf file, if it exists
	 * - System config file in /etc/coflo.conf
	 * Settings from later sources are overridden by settings in earlier sources.
	 */

	/// Parse the command line.
	po::store(po::command_line_parser(argc, argv).
		extra_parser(at_option_parser).
		options(cmdline_options).
		positional(positional_options).run(), m_vm);

	/// Parse the user's .coflo.conf file, if any.
	// Find the user's .coflo.conf file, if any.
	const char *user_home_dir = getenv("HOME");
	if(user_home_dir == NULL)
	{
		// No HOME environment variable.  Check /etc/passwd.
		struct passwd *pw = getpwuid(getuid());
		user_home_dir = pw->pw_dir;
	}
	if(user_home_dir != NULL)
	{
		struct stat s;
		int retval;

		std::string user_conf_file = std::string(user_home_dir) + "/.coflo.conf";
		retval = stat(user_conf_file.c_str(), &s);

		if(S_ISREG(s.st_mode) /** @todo && is readable */)
		{
			// Parse the user's ~/.coflo.conf file.
			po::store(po::parse_config_file<char>(user_conf_file.c_str(), user_config_file_options), m_vm);
		}
	}

	/// @todo Parse the system config.


	// Handle the parameters parsed so far, in particular CLP_RESPONSE_FILE, so that the response_filename variable
	// gets populated, so the check for response files below works properly.
	m_vm.notify();

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
		po::store(po::command_line_parser(args).options(cmdline_options).positional(positional_options).run(), m_vm);
	}

	// Call the notify() functions for any options.
	po::notify(m_vm);

	// See if the user is asking for help, or didn't pass any parameters at all.
	if (m_vm.count(CLP_HELP) || argc < 2)
	{
		/// @todo Commenting out PACKAGE_VERSION_CONTROL_REVISION until I can get it to work properly in configure/make distcheck.
		std::cout << PACKAGE_STRING /*<< PACKAGE_VERSION_CONTROL_REVISION*/ << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: coflo [options] file..." << std::endl;
		std::cout << non_hidden_cmdline_options << std::endl;
		std::cout << "Report bugs to: " << PACKAGE_BUGREPORT << std::endl;
		std::cout << PACKAGE_NAME << " home page: <" << PACKAGE_URL << ">" << std::endl;
		m_was_help_or_version = true;
		return;
	}

	// See if user is requesting version.
	if (m_vm.count(CLP_VERSION))
	{
		// Print version info per GNU Coding Standards <http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion>.
		// PACKAGE_STRING comes from autoconf, and has the format "CoFlo X.Y.Z".
		/// @todo Commenting out PACKAGE_VERSION_CONTROL_REVISION until I can get it to work properly in configure/make distcheck.
		std::cout << PACKAGE_STRING /*<< PACKAGE_VERSION_CONTROL_REVISION*/ << std::endl;
		std::cout << "Copyright (C) 2011, 2012 Gary R. Van Sickle" << std::endl;
		std::cout << "License GPLv3: GNU GPL version 3 <http://gnu.org/licenses/gpl.html>" << std::endl;
		std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
		std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
		m_was_help_or_version = true;
		return;
	}

	// See of the user is requesting build info.
	if(m_vm.count(CLP_BUILD_INFO))
	{
		/// @todo Commenting out PACKAGE_VERSION_CONTROL_REVISION until I can get it to work properly in configure/make distcheck.
		std::cout << PACKAGE_STRING /*<< PACKAGE_VERSION_CONTROL_REVISION*/ << std::endl;
		std::cout << "Copyright (C) 2011, 2012 Gary R. Van Sickle" << std::endl;
		std::cout << std::endl;
		std::cout << "Build info:" << std::endl << std::endl;
		print_build_info();
		m_was_help_or_version = true;
		return;
	}

}

void RuntimeConfiguration::Setup()
{
}
