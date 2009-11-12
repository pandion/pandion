client.namespace("client.html");

client.html.searchBox = function (arg) {
	arg.element.value = arg.placeholder;
	arg.element.attachEvent("onkeydown", function () {
		if (event.keyCode == 13) {
			var retval = arg.action();
			event.returnValue = retval === undefined ? false : retval;
		}
	});
	arg.element.attachEvent("onfocus", function () {
		client.css.addClass(arg.element, "active");
		if (arg.element.value === arg.placeholder)
			arg.element.value = "";
	});
	arg.element.attachEvent("onblur", function () {
		if (arg.element.value.length == 0) {
			client.css.removeClass(arg.element, "active");
			arg.element.value = arg.placeholder;
		}
	});
};
