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
