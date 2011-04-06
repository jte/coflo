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
#include <iostream>

#include "TranslationUnit.h"

int main(int argc, const char* argv[])
{
	TranslationUnit *tu;
	
	if(argc > 0)
	{
		tu = new TranslationUnit();

		// Parse this file.
		std::cout << "Parsing \"" << argv[1] << "\"..." << std::endl;
		bool retval = tu->ParseFile(argv[1]);
		if(retval == false)
		{
			std::cerr << "ERROR: Couldn't parse \"" << argv[1] << "\"" << std::endl;
			return 1;
		}

		// Link the blocks in the functions in the file.
		std::cout << "Linking function blocks..." << std::endl;
		retval = tu->LinkFunctionBlocks();
		if(retval == false)
		{
			std::cerr << "ERROR: Couldn't parse \"" << argv[1] << "\"" << std::endl;
			return 1;
		}
	}

	tu->Print();

	return 0;
}
