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
 * console.log()/.warn()/etc. stub, for running without Firebug and/or a console.
 */
if (typeof console === "undefined") console = {
		log:function(){},
		debug:function(){},
		warn:function(){},
		info:function(){},
		error:function(){},
		group: function(){},
		groupEnd: function(){}
};

/**
 * IE 9 (at least) doesn't support console.group/groupEnd.
 */
if (typeof console.group === "undefined") console.group = function(params){
	console.info("GROUP>>>"+params);
};
if (typeof console.groupEnd === "undefined") console.groupEnd = function(){
	console.info("GROUP<<<");
};

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
		console.warn("null svgdoc")
		return { width: 100, height: 100 };
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
 * @param object The jQuery object to potentially resize.
 * @param container_obj The jQuery UI container to fit @a object to.
 */
function fitToContainer( object, container_obj )
{
	// Validate the input.
	if(object.length == 0)
	{
		console.warn("no object passed");
		return;
	}
	var the_obj_container = container_obj;
	
	// Get the intrinsic dimensions of the svg.
	// We need to get this from its DOM element, not the jQuery object.
	var intrinsic_dims = getIntrinsicDimensions(object[0]);
	var width_obj = intrinsic_dims.width;

	// Get the width of the content area of the container.
	// Note that .width() is correct here, and not .innerWidth(), which includes the padding, somewhat
	// contrary to what one might expect, given the CSS definition of the width attribute.
	var width_obj_container = the_obj_container.width();
	
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

	// Finally we set the size of object.
	/// @todo Not sure which way is better.
	//object.outerWidth( width_new );
	object.css({"width": width_new});
	
	console.log("outerWidth: " + width_obj + ", naturalWidth: " + intrinsic_dims.width + ", container width: " + width_obj_container);
}

