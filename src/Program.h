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

#ifndef PROGRAM_H
#define	PROGRAM_H

#include <vector>

class TranslationUnit;
class Function;

/**
 * Encapsulates the concept of an entire program, consisting of one or more
 * translation units.
 */
class Program
{
public:
	Program();
	Program(const Program& orig);
	virtual ~Program();
private:

	// The TranslationUnits which make up this Program.
	std::vector< TranslationUnit* > m_translation_units;
};

#endif	/* PROGRAM_H */

