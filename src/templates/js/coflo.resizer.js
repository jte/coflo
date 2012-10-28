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
 * @param DOMElement The HTML DOM element from which to obtain the intrinsic width and height.
 *  
 * @return .width and .height of @a DOMElement in pixels.
 */
function getIntrinsicDimensions( DOMElement )
{
	var obj = DOMElement;
	// reference to the SVG document
	var svgdoc = obj.contentDocument;
	
	// Make sure the svg is loaded.
	if(svgdoc == null)
	{
		return { width: 10, height: 10 };
	}
	
	// Get the SVG element
	var svgelem = svgdoc.documentElement;
	/// @todo This is what dot assumes by default.  I'm not sure if we should assume this or determine it if possible.
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
 * Fit the passed @a object to the width of @a container_obj, maintaining aspect ratio.
 * Does not change the size of @a object if its intrinsic width is already less than the width of
 * @a container_obj.
 * 
 * @param object The object to potentially resize.
 * @param container_obj The container to fit @a object to.
 */
function fitToContainer( object, container_obj )
{
	var the_obj_container = container_obj;
	
	// Get the intrinsic dimensions of the svg.
	var intrinsic_dims = getIntrinsicDimensions(object);
	var width_obj = intrinsic_dims.width;

	// Get the width of the content area of the container.
	var domGeom = require("dojo/dom-geometry");
	var width_obj_container = domGeom.getContentBox(the_obj_container).w;
	
	var width_new = width_obj;
	
	if(width_obj_container < 10)
	{
		// Width is so small that nothing would be visible.  Don't resize.
		// We'll get 0 here on at least IE9 while resizing the browser window.
		width_new = 10;
	}
	else if(width_obj > width_obj_container)
	{
		/* Object is wider than its container.  Shrink it to fit. */
		width_new = width_obj_container;

	}
	/// @todo Not sure which way is better.
	//the_obj.outerWidth( width_obj_container );
	//the_obj.css({"width": width_obj_container});
	domGeom.setContentSize(object, {w:width_new});
	
	console.log("outerWidth: " + width_obj + ", naturalWidth: " + intrinsic_dims.width + ", container width: " + width_obj_container);
}

