client.namespace("client.io");

client.io.ajax = function (arg) {
	var xml = new ActiveXObject("MSXML2.DOMDocument");
	xml.async = true;
	xml.resolveExternals = false;
	xml.onreadystatechange = function () {
		if (xml.readyState == 4)
			arg.callback((xml.parseError.errorCode == 0 && xml.documentElement) ? xml.documentElement : null);
	};
	xml.load(arg.url);
};
