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

/**
 * @file Image resizing functions. 
 */

/**
 * Determine the intrinsic (aka "natural") dimensions of an svg file embedded as a <object> element.
 * 
 * @return .width and .height in pixels.
 */
function getIntrinsicDimensions(DOMElement)
{
	var obj = DOMElement;
	// reference to the SVG document
	var svgdoc = obj.contentDocument; 
	 // Get the SVG element
	var svgelem = svgdoc.documentElement;
	/// @todo This is what dot assumes.  I'm not sure if we should assume this or determine it if possible.
	var current_dpi = 96.0;
	var pts_per_in = 72.0;
	// The conversion factor.
	var pts_to_pixels = current_dpi/pts_per_in;

	// Scale and return the bounding box.
	return { width: svgelem.getBBox().width*pts_to_pixels, height: svgelem.getBBox().height*pts_to_pixels };

}

/**
 * Fit all display:block objects matching the given selector to their container's width.
 */
function fitToContainer( objects, container_obj )
{
	var the_selected_objects = objects;
	the_selected_objects.each(function()
	{
		var the_obj = $(this);
		var the_obj_container = $(container_obj);
		
		var width_obj = the_obj.outerWidth(true);
		
		// Get the real dimensions of the svg.
		var intrinsic_dims = getIntrinsicDimensions($(this)[0]);
		var width_obj_container = the_obj_container.width();
		
		if(width_obj > width_obj_container)
		{
			/* Object is wider than its container.  Shrink it to fit. */
			the_obj.outerWidth( width_obj_container );
			
			/// @todo Or is it better to set the width through css?
			//the_obj.css({"width": width_obj_container});
		}
		
		
		$("#cfdebugout").text("outerWidth: " + width_obj + ", naturalWidth: " + intrinsic_dims.width + ", container width: " + width_obj_container);
	});
}
