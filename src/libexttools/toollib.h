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

#ifndef TOOLLIB_H
#define TOOLLIB_H

#include <string>

std::string regex_replace(const std::string &input_string,
		const std::string &regex_to_match,
		const std::string &replacement);

std::string regex_append_after(const std::string &input_string,
		const std::string &regex_to_match,
		const std::string &replacement);

void fcopy(const std::string &from, const std::string &to);

#endif // TOOLLIB_H

