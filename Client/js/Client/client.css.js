client.namespace("client.css");

client.css.hasClass = function (elem, name) {
	return (new RegExp("^|\b" + name + "\b|$")).test(elem.className);
};

client.css.show = function (elem) {
	client.css.removeClass(elem, "hidden");
	client.css.addClass(elem, "visible");
};

client.css.hide = function (elem) {
	client.css.removeClass(elem, "visible");
	client.css.addClass(elem, "hidden");
};

client.css.enable = function (elem) {
	elem.disabled = false;
	client.css.removeClass(elem, "disabled");
	client.css.addClass(elem, "enabled");
};

client.css.disable = function (elem) {
	elem.disabled = true;
	client.css.removeClass(elem, "enabled");
	client.css.addClass(elem, "disabled");
};

client.css.addClass = function (elem, name) {
	elem.className += " " + name;
};

client.css.removeClass = function (elem, name) {
	var classes = elem.className.split(/\s/);
	var offset = 0;
	for (var i in classes)
		if (classes[i - offset] == name) {
			classes.splice(i - offset, 1);
			offset++;
		}
	elem.className = classes.join(" ");
};
