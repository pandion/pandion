client.namespace("client.html");

client.html.anchorToBrowser = function (anchor) {
	anchor.attachEvent("onclick", function () {
		var address = event.srcElement.href;
		event.returnValue = false;
		client.os.launchInBrowser(address);
	});
};
