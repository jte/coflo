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

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>


#include "Program.h"
#include "libexttools/ToolCompiler.h"
#include "Analyzer.h"

// Define a shorter namespace alias for boost::program_options.
namespace po = boost::program_options;

/**
 * Additional command-line parser for the '@file' response file specification.
 * 
 * @param s
 * @return 
 */
std::pair<std::string, std::string> at_option_parser(const std::string &s)
{
    if ('@' == s[0])
	{
        return std::make_pair(std::string("response-file"), s.substr(1));
	}
    else
	{
        return std::pair<std::string, std::string>();
	}
}


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
	std::string the_filter;
	std::string the_gcc;
	std::string the_dot;
	std::string the_ctags;
	
	// Declare the objects that will describe the supported options.
	// Declare general options.
	po::options_description general_options("General Options");
	// Declare preprocessing-related options.
	po::options_description preproc_options("Preprocessing Options");
	// Declare subprograms to use.
	po::options_description subprogram_options("Subprogram Options");
	// Rules to check.
	po::options_description rule_options("Analysis Options");
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
	general_options.add_options()
    ("help", "Produce this help message.")
    ("version,v", "Print version string.")
	("response-file", po::value<std::string>(), "Read command line options from file. Can also be specified with '@name'.")
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
	rule_options.add_options()
	("print-function-cfg", po::value< std::string >(), "Print the control flow graph of the given function to standard output.")
	("constraint", po::value< std::vector<std::string> >(), "\"f1() -x f2()\" : Warn if f1 can reach f2.")
	;
	hidden_options.add_options()
	("input-file", po::value< std::vector<std::string> >(), "input file")
	;
	positional_options.add("input-file", -1);
	non_hidden_cmdline_options.add(general_options).add(preproc_options).add(subprogram_options).add(rule_options);
	cmdline_options.add(non_hidden_cmdline_options).add(hidden_options);

	// Parse the command line.
	po::store(po::command_line_parser(argc, argv).
		options(cmdline_options).
		extra_parser(at_option_parser).
		positional(positional_options).run(), vm);
	
	// Parse any response files.
	if (vm.count("response-file"))
	{
		 // Load the file and tokenize it
		 std::ifstream ifs(vm["response-file"].as<std::string>().c_str());
		 if (!ifs)
		 {
			 std::cout << "Could not open the response file\n";
			 return 1;
		 }
		 // Read the whole file into a string
		 /// @todo Possibly lock the file as FILE_SHARE_READ while doing this.
		 std::stringstream ss;
		 ss << ifs.rdbuf();
		 // Split the file content
		 boost::char_separator<char> sep(" \n\r");
		 std::string ResponsefileContents( ss.str() );
		 boost::tokenizer<boost::char_separator<char> > tok(ResponsefileContents, sep);
		 std::vector<std::string> args;
		 copy(tok.begin(), tok.end(), back_inserter(args));
		 // Parse the file and store the options
		 po::store(po::command_line_parser(args).options(cmdline_options).positional(positional_options).run(), vm);     
	}
	
	// Call the notify() functions for any options.
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
	
	the_program = new Program();
	the_analyzer = new Analyzer();
	
	if(vm.count("input-file")>0)
	{
		const std::vector<std::string> *defines, *includes;
		if(vm.count("define")>0)
		{
			defines = &(vm["define"].as< std::vector<std::string> >());
		}
		else
		{
			defines = new std::vector<std::string>();
		}
		if(vm.count("include-dir")>0)
		{
			includes = &(vm["include-dir"].as< std::vector<std::string> >());
		}
		else
		{
			includes = new std::vector<std::string>();
		}
		
		the_program->SetTheCtags(the_ctags);
		the_program->SetTheDot(the_dot);
		the_program->SetTheFilter(the_filter);
		ToolCompiler *tool_compiler = new ToolCompiler(the_gcc);
		std::cout << "GCC version: " << tool_compiler->GetVersion() << std::endl;
		the_program->SetTheGcc(tool_compiler);
		the_program->AddSourceFiles(vm["input-file"].as< std::vector<std::string> >());
		if(!the_program->Parse(
			*defines,
			*includes,
			static_cast<bool>(vm.count("debug-parse"))))
		{
			// Parse failed.
			return 1;
		}
	}
	
	if(vm.count("print-function-cfg"))
	{
		// User wants a control flow graph.
		if(!the_program->PrintFunctionCFG(vm["print-function-cfg"].as<std::string>()))
		{
			// Something went wrong.
			return 1;
		}
	}
	
	the_analyzer->AttachToProgram(the_program);
	
	if(vm.count("constraint") > 0)
	{
		// Add the given constraints to the analysis.
		the_analyzer->AddConstraints(vm["constraint"].as< std::vector<std::string> >());
	}
	
	// Perform the analysis.
	the_analyzer->Analyze();
	
	if(vm.count("output-dir"))
	{
		the_program->Print(the_dot, vm["output-dir"].as<std::string>());
	}
	
	return 0;
}
