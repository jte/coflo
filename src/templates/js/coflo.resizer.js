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
	
	// Get the bounding box.
	var bbox = svgelem.getBBox();

	// Scale and return the bounding box.
	return { width: bbox.width*pts_to_pixels, height: bbox.height*pts_to_pixels };

}



/**
 * Fit all display:block objects matching the given selector to their container's width.
 */
function fitToContainer( objects, container_obj )
{
	var the_selected_objects = objects;
	//the_selected_objects.each(function()
	the_selected_objects.forEach(function(node, index, nodeList)
	{
		//var the_obj = $(this);
		//var the_obj_container = $(container_obj);
		var the_obj = node;
		var the_obj_container = container_obj;
		
		//var width_obj = the_obj.outerWidth(true);
		
		// Get the real dimensions of the svg.
		//var intrinsic_dims = getIntrinsicDimensions($(this)[0]);
		var intrinsic_dims = getIntrinsicDimensions(node);
		var width_obj = intrinsic_dims.width;
		// Get the width of the content area of the container.
		// Note that in jQuery, .width() is correct here, and not .innerWidth(), which includes the padding, somewhat
		// contrary to what one might expect, given the CSS definition of the width attribute.
		//var width_obj_container = the_obj_container.width();
		var domGeom = require("dojo/dom-geometry");
		var width_obj_container = domGeom.getContentBox(the_obj_container).w;
		
		if(width_obj_container < 10)
		{
			// Width is so small that nothing would be visible.  Don't resize.
			// We'll get 0 here on at least IE9 while resizing the browser window.
		}
		else if(width_obj > width_obj_container)
		{
			/* Object is wider than its container.  Shrink it to fit. */
			/// @todo Not sure which way is better.
			//the_obj.outerWidth( width_obj_container );
			//the_obj.css({"width": width_obj_container});
			domGeom.setContentSize(the_obj, {w:width_obj_container});
		}
		
		console.log("outerWidth: " + width_obj + ", naturalWidth: " + intrinsic_dims.width + ", container width: " + width_obj_container);
	});
}

