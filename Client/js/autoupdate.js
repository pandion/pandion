/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
window.attachEvent("onload", function () {

	var getLatestAppcastByTrack = function (doc, track) {
		var appcastNamespaceUri = "http://pandion.im/protocol/appcast/1.0";
		var appcasts = getAppcastsFromFeed(doc);
		var topAppcastEntry = null;
		var topTimestamp = Number.NEGATIVE_INFINITY;
		for (var i = 0; i < appcasts.length; i++)
			if (appcasts[i].attributes.getQualifiedItem("track", appcastNamespaceUri).value == track) {
				var timestamp = appcasts[i].selectSingleNode("updated").text;
				var timeFromEpoch = client.data.iso8601(timestamp);
				if (timeFromEpoch > topTimestamp) {
					topTimestamp = timeFromEpoch;
					topAppcastEntry = parseAppcastFromEntry(appcasts[i]);
				}
			}
		return topAppcastEntry;
	};

	var getAppcastByVersion = function (doc, version) {
		var appcastNamespaceUri = "http://pandion.im/protocol/appcast/1.0";
		var appcasts = getAppcastsFromFeed(doc);
		for (var i = 0; i < appcasts.length; i++)
			if (appcasts[i].attributes.getQualifiedItem("version", appcastNamespaceUri).value == external.globals("softwareversion"))
				return parseAppcastFromEntry(appcasts[i]);
		return null;
	};

	var getAppcastsFromFeed = function (doc) {
		return doc ? doc.selectNodes("/feed/entry[updated][link[@rel='enclosure'][@href]]") : [];
	};

	var parseAppcastFromEntry = function (entry) {
		var getXpathTagValue = function (element, xpath) {
			var match = element.selectSingleNode(xpath);
			return match ? match.text : "";
		};
		var getNamespacedAttribute = function (element, attribute, xmlns) {
			if (!("attributes" in element))
				return "";
			var match = element.attributes.getQualifiedItem(attribute, xmlns);
			return match ? match.value : "";
		};
		var appcastNamespaceUri = "http://pandion.im/protocol/appcast/1.0";
		return {
			alternate: getXpathTagValue(entry, "link[@rel='alternate'][@type='text/html' or @type='text/plain']/@href"),
			arguments: getNamespacedAttribute(entry.selectSingleNode("link[@rel='enclosure'][@href]"), "arguments", appcastNamespaceUri),
			enclosure: entry.selectSingleNode("link[@rel='enclosure']/@href").value,
			name: getNamespacedAttribute(entry, "name", appcastNamespaceUri),
			summary: getXpathTagValue(entry, "summary"),
			track: getNamespacedAttribute(entry, "track", appcastNamespaceUri),
			version: getNamespacedAttribute(entry, "version", appcastNamespaceUri)
		};
	};

	var versionIsHigherThanCurrent = function (version) {
		var versionNumbers = version.split(/[-\. ]/);
		var currentVersion = external.globals("softwareversion").split(/[-\. ]/);
		return (parseInt(versionNumbers[0], 10) > parseInt(currentVersion[0], 10) // major
			|| (parseInt(versionNumbers[1], 10) > parseInt(currentVersion[1], 10) // ... or minor
				&& parseInt(versionNumbers[0], 10) == parseInt(currentVersion[0], 10))
			|| (parseInt(versionNumbers[2], 10) > parseInt(currentVersion[2], 10) // ... or build
				&& parseInt(versionNumbers[1], 10) == parseInt(currentVersion[1], 10)
				&& parseInt(versionNumbers[0], 10) == parseInt(currentVersion[0], 10))
		);
	};

	var downloadUpdate = function (appcastEntry) {
		gLocation = appcastEntry.alternate;
		gParameters = appcastEntry.arguments;

		document.getElementById("txt-version").innerText = external.globals("Translator").Translate("autoupdate", "title", [appcastEntry.name, appcastEntry.version]);
		document.getElementById("txt-whats-new").href = appcastEntry.alternate ? appcastEntry.alternate : external.globals("ClientPluginContainer").ParseURL(external.globals("softwareurl"));
		document.getElementById("progress-version-number").innerText = external.globals("softwareversion");
		document.getElementById("progress-version-track").innerText = external.globals("Translator").Translate("main", "track-" + external.globals("softwaretrack"));
		client.css.addClass(document.getElementById("progress-version-track"), external.globals("softwareversion"));
		document.createElement("div").innerHTML = appcastEntry.summary;

		if (appcastEntry.enclosure.length) {
			appcastEntry.enclosure = appcastEntry.enclosure.substr(appcastEntry.enclosure.indexOf("//") + 2);

			var Path = appcastEntry.enclosure;
			if (appcastEntry.enclosure.indexOf("/") != -1)
				Path = appcastEntry.enclosure.substr(appcastEntry.enclosure.indexOf("/"));
			else
				Path = "/";

			if (appcastEntry.enclosure.indexOf("?") != -1)
				appcastEntry.enclosure = appcastEntry.enclosure.substr(0, appcastEntry.enclosure.indexOf("?"));

			var Host = appcastEntry.enclosure;
			if (appcastEntry.enclosure.indexOf("/") != -1)
				Host = appcastEntry.enclosure.substr(0, appcastEntry.enclosure.indexOf("/"));

			var Port = 80;
			if (Host.indexOf(":") != -1) {
				Port = parseInt(Host.substr(Host.lastIndexOf(":") + 1));
				Host = Host.substr(0, Host.indexOf(":"));
			}
			if (isNaN(Port))
				Port = 80;

			if (appcastEntry.enclosure.lastIndexOf("/") != -1)
				gFileName = decodeURIComponent(appcastEntry.enclosure.substr(appcastEntry.enclosure.lastIndexOf("/") + 1).replace(/\\\:\*\?\"\<\>\|/, ""));
			else
				gFileName = 'noname';
			if (!gFileName.length)
				gFileName = 'noname';

			var DownloadLocation = external.globals("usersdir") + "Downloads\\";

			if (!external.Directory.Exists(DownloadLocation))
				external.Directory.Create(DownloadLocation);

			if (external.FileExists(DownloadLocation + gFileName))
				external.file(DownloadLocation + gFileName).Delete();

			external.HTTPEngine.subscribe(external.wnd);
			gSession = external.HTTPEngine.Get(DownloadLocation + gFileName, Path, 0, 0xFFFFFFFF, Host, Port);

			showPageProgress();
		}
		else {
			showPageDownload();
		}

		external.wnd.hide(false);
		document.getElementById("btn-later").focus();
	}

	external.globals("Translator").TranslateWindow("autoupdate", document);
	var selectionFilter = function () {return event.srcElement.tagName == "TEXTAREA" || event.srcElement.tagName == "INPUT"};
	document.attachEvent("onselectstart", selectionFilter);
	document.attachEvent("ondragstart", selectionFilter);
	document.attachEvent("oncontextmenu", selectionFilter);
	document.attachEvent("onkeydown", function () {
		if (event.keyCode == 27)
			external.wnd.close();
	});

	client.html.anchorToBrowser(document.getElementById("txt-whats-new"));

	document.getElementById("available-updates").attachEvent("onsubmit", function () {
		event.returnValue = false;
		external.wnd.close();
	});

	document.getElementById("download-progress").attachEvent("onsubmit", function () {
		event.returnValue = false;
		external.wnd.close();
	});

	document.getElementById("btn-update").attachEvent("onclick", function () {
		var DownloadLocation = external.globals("usersdir") + "Downloads\\";
		if (external.FileExists(DownloadLocation + "Update.txt"))
			external.File(DownloadLocation + "Update.txt").Delete();
		if (!external.FileExists(DownloadLocation + "Update.txt")) {
			external.globals.Add("autoupdatecommand", DownloadLocation + gFileName);
			external.globals.Add("autoupdateparameters", gParameters);
			external.globals.Add("autoupdatedirectory", DownloadLocation);
			window.attachEvent("onunload", function () {
				external.windows("MainWindow").close();
			});
			external.wnd.close();
		}
	});

	document.getElementById("btn-download").attachEvent("onclick", function () {
		external.wnd.hide(true);
		client.os.launchInBrowser(gLocation);
		external.wnd.close();
	});

	window.attachEvent("onunload", function () {
		external.HTTPEngine.unsubscribe(external.wnd);
		if (gSession)
			external.HTTPEngine.Abort(gSession);
		if (!external.wnd.params.selectVersion)
			external.globals("AutoUpdateTimeout") = external.wnd.params.window.setTimeout(function () {
				dial_autoupdate( false );
			}, 24 * 3600 * 1000);
	});

	if (external.wnd.params.selectVersion) {
		/* Load list of latest updates */
		document.getElementById("tracks-version-number").innerText = external.globals("softwareversion");
		document.getElementById("tracks-version-track").innerText = external.globals("Translator").Translate("main", "track-" + external.globals("softwaretrack"));
		client.css.addClass(document.getElementById("tracks-version-track"), external.globals("softwareversion"));
		client.css.hide(document.getElementById("download-progress"));
		client.css.hide(document.getElementById("txt-unavailable"));
		client.css.hide(document.getElementById("txt-no-feed"));
		client.css.hide(document.getElementById("available-list"));
		client.css.show(document.getElementById("available-preloader"));
		client.css.show(document.getElementById("available-updates"));
		document.getElementById("btn-close").focus();
		external.wnd.hide(false);
		client.io.ajax({
			url: external.globals("ClientPluginContainer").ParseURL(external.globals("autoupdate")),
			callback: function (dom, xhr) {
				var tracks = {
					stable: getLatestAppcastByTrack(dom, "stable"),
					beta: getLatestAppcastByTrack(dom, "beta"),
					development: getLatestAppcastByTrack(dom, "development")
				};
				var descriptions = {
					stable: external.globals("Translator").Translate("autoupdate", "description-stable"),
					beta: external.globals("Translator").Translate("autoupdate", "description-beta"),
					development: external.globals("Translator").Translate("autoupdate", "description-development")
				};
				var insertSimpleElement = function (containerElement, tagName, content) {
					var childElement = document.createElement(tagName);
					childElement.innerText = content;
					containerElement.appendChild(childElement);
					return childElement;
				};
				for (var name in tracks)
					if (tracks[name]) {
						var button = document.createElement("<button type='button' />");
						button.className = "update-button";
						var title = insertSimpleElement(button, "h4", "");
						client.css.addClass(insertSimpleElement(title, "span", tracks[name].name), "name");
						client.css.addClass(insertSimpleElement(title, "span", tracks[name].version), "number");
						client.css.addClass(insertSimpleElement(title, "span", external.globals("Translator").Translate("main", "track-" + name)), "track");
						if (tracks[name].version == external.globals("softwareversion")) {
							button.disabled = true;
							insertSimpleElement(button, "div", external.globals("Translator").Translate("autoupdate", "up-to-date"));
						}
						else if (name == external.globals("softwaretrack") && !versionIsHigherThanCurrent(tracks[name].version))
							insertSimpleElement(button, "div", external.globals("Translator").Translate("autoupdate", "previous-version"));
						else if (name == "stable")
							insertSimpleElement(button, "div", external.globals("Translator").Translate("autoupdate", "recommended"));
						insertSimpleElement(button, "p", descriptions[name]);
						document.getElementById("tracks-" + (name == external.globals("softwaretrack") ? "current" : "alternative")).appendChild(button);
						button.onclick = (function (name) {return function () {
							client.css.hide(document.getElementById("available-updates"));
							client.css.show(document.getElementById("download-progress"));
							external.globals("softwaretrack") = name;
							downloadUpdate(tracks[name]);
						}})(name);
					}
				client.css.hide(document.getElementById("available-preloader"));
				if (!dom.documentElement) {
					client.css.show(document.getElementById("txt-no-feed"));
				} else if (document.getElementById("tracks-current").childNodes.length + document.getElementById("tracks-alternative").childNodes.length == 0) {
					client.css.show(document.getElementById("txt-unavailable"));
				} else {
					if (document.getElementById("tracks-current").childNodes.length)
						client.css.show(document.getElementById("txt-latest"));
					else
						client.css.hide(document.getElementById("txt-latest"));
					if (document.getElementById("tracks-alternative").childNodes.length)
						client.css.show(document.getElementById("tracks-alternative"));
					else
						client.css.hide(document.getElementById("tracks-alternative"));
					client.css.show(document.getElementById("available-list"));
				}
			}
		});
	} else {
		/* Automatically download latest update */
		client.css.hide(document.getElementById("available-updates"));
		client.css.show(document.getElementById("download-progress"));
		client.io.ajax({
			url: external.globals("ClientPluginContainer").ParseURL(external.globals("autoupdate")),
			headers: {
				"Cache-Control": "no-cache",
				"Pragma": "no-cache"
			},
			callback: function (dom, xhr) {
				if (dom.documentElement) {
					var currentAppcast = getAppcastByVersion(dom, external.globals("softwareversion"));
					if (currentAppcast && currentAppcast.track.length > 0)
						external.globals("softwaretrack") = currentAppcast.track;
					external.globals("last_autoupdate") = (new Date()).getTime();
					external.wnd.params.window.SaveCommonProfile();
					var topAppcastEntry = getLatestAppcastByTrack(dom, external.globals("softwaretrack"));
					if (topAppcastEntry && versionIsHigherThanCurrent(topAppcastEntry.version) && external.globals("installupdates") == "yes")
						return downloadUpdate(topAppcastEntry);
				}
				external.wnd.close();
			}
		});
	}

});

var gFileName, gSession, gInterval, gParameters, gLocation = '';

var HTTPOnTimeout = HTTPOnSockErr = OnHTTPFileErr = HTTPOnTransferAborted = function (sessionID, error) {
	if (sessionID == gSession)
	{
		clearInterval(gInterval);
		gSession = 0;
		showPageDownload();
	}
}

function HTTPOnConnected (sessionID, remoteHost, remotePort, localPort) {
	if (sessionID != gSession)
		return;
	gInterval = setInterval(function () {
		if (sessionID != gSession)
			return;
		var total = external.HTTPEngine.GetLength(sessionID);
		var completed = external.HTTPEngine.GetProgress(sessionID);
		if (completed > total)
			completed = total;
		var percentage = total > 0 ? Math.round(1000 * (100 * completed / total)) / 1000 : 0;
		document.getElementById("progress-completed").style.width = percentage + "%";
		document.getElementById("progress-counter").innerText = Math.round(percentage) + "%";
	}, 40);
}

function HTTPOnTransferComplete (sessionID) {
	if (sessionID != gSession)
		return;
	clearInterval(gInterval);
	gSession = 0;
	showPageUpdate();

	var path = external.globals("usersdir") + "Downloads\\Update.txt";
	if (external.FileExists(path))
		external.File(path).Delete();
	var file = external.File(path);
	file.WriteLine(gFileName);
	file.WriteLine(gParameters);
	file.Close();
}

function showPageProgress () {
	client.css.show(document.getElementById("progress-total"));
	client.css.show(document.getElementById("txt-continue"));
	client.css.hide(document.getElementById("txt-restart"));
	client.css.hide(document.getElementById("txt-browser"));
	client.css.hide(document.getElementById("btn-update"));
	client.css.hide(document.getElementById("btn-download"));
}

function showPageUpdate () {
	client.css.hide(document.getElementById("progress-total"));
	client.css.hide(document.getElementById("txt-continue"));
	client.css.show(document.getElementById("txt-restart"));
	client.css.hide(document.getElementById("txt-browser"));
	client.css.show(document.getElementById("btn-update"));
	client.css.hide(document.getElementById("btn-download"));
}

function showPageDownload () {
	client.css.hide(document.getElementById("progress-total"));
	client.css.hide(document.getElementById("txt-continue"));
	client.css.hide(document.getElementById("txt-restart"));
	client.css.show(document.getElementById("txt-browser"));
	client.css.hide(document.getElementById("btn-update"));
	client.css.show(document.getElementById("btn-download"));
}
