/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.html");

client.html.anchorToBrowser = function (anchor) {
	anchor.attachEvent("onclick", function () {
		var address = event.srcElement.href;
		event.returnValue = false;
		client.os.launchInBrowser(address);
	});
};
