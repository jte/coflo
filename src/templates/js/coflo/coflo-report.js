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
 * Helper functions for the generated CoFlo report.
 */

define(["dojo/_base/declare",
        "dojo/dom-geometry"],
        function(declare, domGeom){
	var CoFloReport = declare(null,
		{
			constructor: function(){
				console.debug("CoFloReport constructor called");
			}
		});
	
	CoFloReport.something = "some text";
	CoFloReport.aMethod = function(object, container_obj){
		// Get the width of the container object.		
		var width_obj_container = domGeom.getContentBox(container_obj).w;
				return "aMethod called, container width=" + width_obj_container;
				}
	return CoFloReport;
});
