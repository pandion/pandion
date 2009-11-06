client.namespace("client.utils");

client.utils.searchBox = function (arg) {
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

client.utils.ajax = function (arg) {
	var xml = new ActiveXObject("MSXML2.DOMDocument");
	xml.async = true;
	xml.resolveExternals = false;
	xml.onreadystatechange = function () {
		if (xml.readyState == 4)
			arg.callback((xml.parseError.errorCode == 0 && xml.documentElement) ? xml.documentElement : null);
	};
	xml.load(arg.url);
};

client.utils.prettytime = function (date) {
	if (date instanceof Date)
		date = date.getTime();
	var timeAgo = (new Date()).getTime() - date;
	timeAgo /= 1000;
	var map = [
		[0, "now"],
		[1, "second"],
		[2, "seconds", 2],
		[60, "minute"],
		[2*60, "minutes", 2],
		[60*60, "hour"],
		[2*60*60, "hours", 2],
		[24*60*60, "day"],
		[2*24*60*60, "days", 2],
		[7*24*60*60, "week"],
		[2*7*24*60*60, "weeks", 2],
		[30*24*60*60, "month"],
		[2*30*24*60*60, "months", 2],
		[365*24*60*60, "year"],
		[2*365*24*60*60, "years", 2]
	];
	for (var i = 1; i < map.length && timeAgo >= map[i][0]; ++i);
	i--;
	if (map[i].length == 3)
		timeAgo *= map[i][2];
	return external.globals("Translator").Translate(
		"main",
		"time-" + map[i][1],
		[parseInt(timeAgo / map[i][0])]
	);
};

client.utils.iso8601 = function (timestamp) {
	                           // 1 BCAD 2 Y          3 M         4 D            5 h         6 m         7 s         8 ms                              9 o   10 oh       11 om
	var match = timestamp.match(/^([-+]?)(\d{4,})(?:-?(\d{2})(?:-?(\d{2})(?:[Tt ](\d{2})(?::?(\d{2})(?::?(\d{2})(?:\.(\d{1,3})(?:\d+)?)?)?)?(?:[Zz]|(?:([-+])(\d{2})(?::?(\d{2}))?)?)?)?)?)?$/);
	if (match) {
		for (var ints = [2, 3, 4, 5, 6, 7, 8, 10, 11], i = ints.length - 1; i >= 0; --i)
			match[ints[i]] = (typeof match[ints[i]] != "undefined" &&
				match[ints[i]].length > 0) ? parseInt(match[ints[i]], 10) : 0;
		if (match[1] == '-') // BC/AD
			match[2] *= -1;
		var ms = Date.UTC(
			match[2], // Y
			match[3] - 1, // M
			match[4], // D
			match[5], // h
			match[6], // m
			match[7], // s
			match[8] // ms
		);
		if (typeof match[9] != "undefined" && match[9].length > 0) // offset
			ms += (match[9] == '+' ? -1 : 1) * (match[10]*3600*1000 + match[11]*60*1000); // oh om
		if (match[2] >= 0 && match[2] <= 99) // 1-99 AD
			ms -= 59958144000000;
		return ms;
	}
	else
		return Number.NEGATIVE_INFINITY;
};

client.utils.anchorToBrowser = function (anchor) {
	anchor.attachEvent("onclick", function () {
		var address = event.srcElement.href;
		event.returnValue = false;
		client.utils.launchInBrowser(address);
	});
};

client.utils.launchInBrowser = function (address) {
	if (address.substr(0, 7) == "http://" || address.substr(0, 8) == "https://") {
		try {
			var HTTPHandler = client.os.registry.read("HKEY_CLASSES_ROOT", "HTTP\\shell\\open\\command", "");
			if (HTTPHandler.indexOf("iexplore.exe") != -1)
				return window.open(address, "_blank");
		}
		catch (e) {
		}
	}
	external.shellExec("open", address, "", "", 0);
}
