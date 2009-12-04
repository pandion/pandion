/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client");

client.css = function () {};

client.css.prototype = {

	/* Helpers to manipulate the class attribute safely */
	hasClass: function (elem, name) {
		return (new RegExp("(^|\\s)" + name + "(\\s|$)")).test(elem.className);
	},
	addClass: function (elem, name) {
		elem.className += " " + name;
	},
	removeClass: function (elem, name) {
		var classes = elem.className.split(/\s+/);
		for (var i = 0; i < classes.length; i++)
			if (classes[i] == name)
				classes.splice(i--, 1);
		elem.className = classes.join(" ");
	},

	/* Set the visibility of an element through CSS classes.
	   Note that the style properties are not touched directly.
	   It is up to the CSS theme to implement these CSS classes. */
	show: function (elem) {
		this.removeClass(elem, "hidden");
		this.addClass(elem, "visible");
	},
	hide: function (elem) {
		this.removeClass(elem, "visible");
		this.addClass(elem, "hidden");
	},
	visible: function (elem) {
		return this.hasClass(elem, "visible");
	},
	hidden: function (elem) {
		return this.hasClass(elem, "hidden");
	},
	toggle: function (elem) {
		this[this.visible(elem) ? "hide" : "show"](elem);
	},

	/* Set the interativity of an element through CSS classes.
	   Note that the style properties are not touched directly.
	   It is up to the CSS theme to implement these CSS classes. */
	enable: function (elem) {
		elem.disabled = false;
		this.removeClass(elem, "disabled");
		this.addClass(elem, "enabled");
	},
	disable: function (elem) {
		elem.disabled = true;
		this.removeClass(elem, "enabled");
		this.addClass(elem, "disabled");
	},
	enabled: function (elem) {
		return this.hasClass(elem, "enabled");
	},
	disabled: function (elem) {
		return this.hasClass(elem, "disabled");
	},
	flip: function (elem) {
		this[this.enabled(elem) ? "disable" : "enable"](elem);
	}

};

client.css = new client.css();
