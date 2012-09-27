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

#ifndef TOOLDOT_H
#define	TOOLDOT_H

#include "ToolBase.h"

#include <string>

/**
 * Facade for the [Graphviz] dot tool.
 *
 * [Graphviz]: http://www.graphviz.org/ "Graphviz"
 */
class ToolDot : public ToolBase
{
public:
	/// @name Member types.
	///@{

	/// Enumerator for specifying the output format.
	enum OUTPUT_FORMAT_DOT { SVG, PNG };

	/// The corresponding format strings to pass to dot.
	static const char* format_strings[];

	///@}

public:
	ToolDot(const std::string &cmd);
	ToolDot(const ToolDot& orig);
	virtual ~ToolDot();
	
	/**
	 * Generates a bitmap- or vector-graphic file from the given *.dot file.
	 *
	 * @param dot_filename    Path to the input dot file.
	 * @param output_filename Path to the output file.
	 * @param output_format   The output format to be generated.
	 *
	 * @return
	 */
	bool CompileDotToPNG(const std::string &dot_filename, const std::string &output_filename,
			OUTPUT_FORMAT_DOT output_format = ToolDot::SVG) const;
	
protected:
	
	virtual std::string GetVersionOptionString() const { return "-V 2>&1"; };
	virtual std::string GetVersionExtractionRegex() const { return ".*?([\\d\\.]+).*"; };

};

#endif	/* TOOLDOT_H */

