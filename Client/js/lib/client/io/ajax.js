/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.io");

client.io.ajax = function (request) {
	request.async = request.async || true;
	request.data = request.data || null;
	request.headers = request.headers || {};
	request.method = request.method || ((request.data === null) ? "GET" : "POST");
	request.password = request.password || null;
	request.url = request.url || "";
	request.user = request.user || null;

	if (request.method === "POST" && request.data !== null && !("Content-Type" in request.headers))
		request.headers["Content-Type"] = "application/x-www-form-urlencoded";

	if (!("User-Agent" in request.headers))
		request.headers["User-Agent"] = external.globals("softwarenamesafe")
			+ "/" + external.globals("softwareversion")
			+ " (" + external.globals("language") + ")";

	var xhrFactories = [
		//function () {return new XMLHttpRequest()}, /* ie7/ie8 -- causes fatal crash on some systems (see issue 74) */
		//function () {return new ActiveXObject("Msxml2.XMLHTTP.6.0")}, /* xpsp3/vista/7 -- causes fatal crash on some systems */
		//function () {return new ActiveXObject("Msxml2.XMLHTTP.3.0")}, /* 2ksp4 -- causes fatal crash on some systems */
		//function () {return new ActiveXObject("Msxml2.XMLHTTP")}, /* ie6 -- as used by Dojo */
		function () {return new ActiveXObject("Microsoft.XMLHTTP")} /* ie5/ie5.5/ie6 -- as used by jQuery, YUI, etc. */
	];

	var getXhr = function () {
		for (var i = 0; i < xhrFactories.length; i++) {
			try {
				return xhrFactories[i]();
			} catch (exception) {
				continue;
			}
		}
	};

	var xhr = getXhr();
	xhr.open(request.method, request.url, request.async, request.user, request.password);
	for (var fieldName in request.headers)
		xhr.setRequestHeader(fieldName, request.headers[fieldName]);
	xhr.onreadystatechange = function () {
		if (xhr.readyState == 4) {
			if (xhr.responseText.length > 0 && xhr.responseXML !== null && xhr.responseXML.documentElement === null && xhr.responseXML.parseError.errorCode == 0)
				xhr.responseXML.loadXML(xhr.responseText);
			var dom = new ActiveXObject("Msxml2.DOMDocument");
			dom.loadXML(xhr.responseText);
			request.callback(dom, xhr);
		}
	};
	xhr.send(request.data);
};
