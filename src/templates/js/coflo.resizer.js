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
 * Fit all display:block objects matching the given selector to their container's width.
 */
function fitToContainer( selector )
{
	var the_selected_objects = $( selector );
	the_selected_objects.each(function()
	{
		var the_obj = $(this);
		var the_obj_container = $(this).parent();
		
		var width_obj = the_obj.outerWidth();
		var width_obj_container = the_obj_container.width();
		
		if(width_obj > width_obj_container)
		{
			/* Object is wider than its container.  Shrink it to fit. */
			the_obj.outerWidth( width_obj_container );
		}
		
		
		//$("#cfdebugout").text("svg2 outerWidth: " + the_obj.outerWidth() + ", svg2 container width: " + the_obj_container.innerWidth());
		
		
		/*return true;*/
	});
}
