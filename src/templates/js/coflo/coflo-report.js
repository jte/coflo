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
 * Helper functions for the generated CoFlo HTML report.
 */

/**
 * console.log()/.warn()/etc. stub, for running without Firebug and/or a console.
 */
if (!console) console = {
		log:function(){},
		debug:function(){},
		warn:function(){},
		info:function(){},
		error:function(){},
		group: function(){},
		groupEnd: function(){}
};

// Create and add a tab.
function setTabPaneContent(tab_pane, svg_url) 
{
	var tabTitle = $( "#tab_title" ),
	tabContent = '<object class="svg-cfg" type="image/svg+xml" data="' + svg_url + '"></object>',
	tabTemplate = "<li><a href='#{href}'>#{label}</a></li>";
	var label = tabTitle.val() || "Tab " + tabCounter,
	id = "tabs-" + tabCounter,
	li = $( tabTemplate.replace( /#\{href\}/g, "#" + id ).replace( /#\{label\}/g, label ) ),                
	tabContentHtml = tabContent;
	tab_pane.replaceWith(tabContent);
	center_pane_tabs.find( ".ui-tabs-nav" ).append( li );            
	center_pane_tabs.append( "<div id='" + id + "'>" + tabContentHtml + "</div>" );
}
