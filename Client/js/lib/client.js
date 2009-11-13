/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
if (!client) {
	var client = {
		namespace: function (ns) {
			var parent = window;
			var children = ns.split(".");
			for (i in children)
				parent = (children[i] in parent) ? parent[children[i]] : (parent[children[i]] = {});
			return parent;
		}
	};
}
