/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
window.attachEvent("onload", function () {
	external.globals("Translator").TranslateWindow("welcome", document);
	var selectionFilter = function () {return event.srcElement.tagName == "TEXTAREA" || event.srcElement.tagName == "INPUT"};
	document.attachEvent("onselectstart", selectionFilter);
	document.attachEvent("ondragstart", selectionFilter);
	document.attachEvent("oncontextmenu", selectionFilter);
	document.attachEvent("onkeydown", function () {if (event.keyCode == 27) external.wnd.close()});

	document.getElementById("show-welcome").checked = external.globals("welcomescreen") == "yes";
	document.getElementById("txt-ask-question").href = document.getElementById("txt-powered-by").href = "http://getsatisfaction.com/" + external.globals("getsatisfactioncompany");

	// TODO check if still default xmpp client (if not, show settings again)
	document.getElementById("settings").className = "visible";// TODO external.globals("welcomesettings").toString() == "true" ? "visible" : "hidden";
	document.getElementById("shortcuts").className = "hidden";// TODO external.globals("welcomesettings").toString() == "true" ? "hidden" : "visible";
	if (external.wnd.params.document.getElementById("signin-dialog").style.display == "block") {
		document.getElementById("shortcuts").disabled = true;
		document.getElementById("shortcuts").className += " disabled";
	}

	var saveSettings = function () {
		// TODO external.globals("welcomesettings") = false;

		/* Default XMPP client */
		if (document.getElementById("default-xmpp-client").checked) {
			// TODO make default xmpp client
		}
		if (document.getElementById("change-homepage").checked)
			Client.OS.Browser.SetHomepage({
				platforms: ["ie", "ff", "cr", "op"],
				url: external.globals("browserhomepage")
			});
		if (document.getElementById("change-search-engine").checked)
			Client.OS.Browser.SetSearchbox({
				name: external.globals("browsersearchboxname"),
				osd: external.globals("cwd") + "../settings/osd.xml",
				platforms: ["ie", "ff", "cr", "op"],
				url: external.globals("browsersearchboxurl")
			});
	};

	document.getElementById("btn-add-contact").attachEvent("onclick", function () {
		if (external.wnd.params.document.getElementById( 'signin-dialog' ).style.display != 'block')
			external.wnd.params.dial_adduser();
	});
	document.getElementById("btn-edit-profile").attachEvent("onclick", function () {
		if (external.wnd.params.document.getElementById( 'signin-dialog' ).style.display != 'block')
			external.wnd.params.dial_vcard_edit();
	});
	document.getElementById("btn-transports").attachEvent("onclick", function () {
		if (external.wnd.params.document.getElementById( 'signin-dialog' ).style.display != 'block')
			external.wnd.params.dial_transport_list();
	});
	document.getElementById("btn-settings").attachEvent("onclick", function () {
		if (external.wnd.params.document.getElementById( 'signin-dialog' ).style.display != 'block')
			external.wnd.params.dial_preferences("");
	});

	document.getElementById("content").attachEvent("onsubmit", function () {
		if (document.getElementById("settings").className == "visible")
			saveSettings();
		external.globals("welcomescreen") = document.getElementById("show-welcome").checked ? "yes" : "no";
		event.returnValue = false;
		external.wnd.close();
	});

	document.getElementById("settings-close").attachEvent("onclick", function () {
		saveSettings();
		document.getElementById("settings").className = "hidden";
		document.getElementById("shortcuts").className = "visible";
		event.returnValue = false;
	});

	var anchors = document.getElementsByTagName("a");
	for (var i = 0; i < anchors.length; i++)
		if (anchors[i].href.indexOf("http") != -1)
			Client.Utils.AnchorToBrowser(anchors[i]);

	Client.Utils.Searchbox({
		element: document.getElementById("search-box"),
		placeholder: external.globals("Translator").Translate("welcome", "search-topics"),
		action: function () {
			var url = external.globals("ClientPluginContainer").ParseURL(
				external.globals("welcomesearchurl"),
				{query: document.getElementById("search-box").value}
			);
			dial_webbrowser(url);
		}
	});

	document.getElementById("feed-loading").className = "visible";
	document.getElementById("feed-unavailable").className = "hidden";
	document.getElementById("feed").className = "hidden";
	Client.Utils.Ajax({
		url: external.globals("welcomefeedurl"),
		callback: function (doc) {
			var getPlainText = function (parent, tagName) {
				var node = null;
				if (node = parent.selectSingleNode(tagName + "[@type='text' or not(@type)]"))
					return node.text;
				else if (node = parent.selectSingleNode(tagName + "[@type='html']")) {
					var html = document.createElement("div");
					html.innerHtml = node.text;
					return html.innerText;
				}
				else if (node = parent.selectSingleNode(tagName + "[@type='xhtml']")) {
					return node.text;
				}
				else
					return "";
			};
			document.getElementById("feed-loading").className = "hidden";
			if (doc) {
				var feed = document.getElementById("feed");
				feed.className = "visible";
				var entries = doc.selectNodes("/feed/entry[updated][title][link[@rel='alternate' and @type='text/html' and @href]]");
				for (var i = 0; i < entries.length; ++i) {
					var listItem = document.createElement("li");
					var anchor = document.createElement("a");
					anchor.innerText = getPlainText(entries[i], "title");
					anchor.href = entries[i].selectSingleNode("link[@rel='alternate' and @type='text/html' and @href]").getAttribute("href");
					Client.Utils.AnchorToBrowser(anchor);
					listItem.appendChild(anchor);
					var timestamp = document.createElement("div");
					timestamp.className = "timestamp";
					timestamp.innerText = Client.Utils.Prettytime(Client.Utils.ISO8601(entries[i].selectSingleNode("updated").text));
					listItem.appendChild(timestamp);
					feed.appendChild(listItem);
				}
			}
			else
				document.getElementById("feed-unavailable").className = "visible";
		}
	});

	document.getElementById("introduction").focus();
	external.wnd.hide(false);
});

var Client = {};
Client.Utils = {};

Client.Utils.Searchbox = function (arg) {
	arg.element.value = arg.placeholder;
	arg.element.attachEvent("onkeydown", function () {
		if (event.keyCode == 13) {
			var retval = arg.action();
			event.returnValue = retval === undefined ? false : retval;
		}
	});
	arg.element.attachEvent("onfocus", function () {
		arg.element.className = "active";
		if (arg.element.value === arg.placeholder)
			arg.element.value = "";
	});
	arg.element.attachEvent("onblur", function () {
		if (arg.element.value.length == 0)
		{
			arg.element.className = "";
			arg.element.value = arg.placeholder;
		}
	});
};

Client.Utils.Ajax = function (arg) {
	var xml = new ActiveXObject("MSXML2.DOMDocument");
	xml.async = true;
	xml.resolveExternals = false;
	xml.onreadystatechange = function () {
		if (xml.readyState == 4)
			arg.callback((xml.parseError.errorCode == 0 && xml.documentElement) ? xml.documentElement : null);
	};
	xml.load(arg.url);
};

Client.Utils.Prettytime = function (date) {
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

Client.Utils.ISO8601 = function (timestamp) {
	                           // 1 BCAD 2 Y          3 M         4 D            5 h         6 m         7 s         8 ms                              9 o   10 oh       11 om
	var match = timestamp.match(/^([-+]?)(\d{4,})(?:-?(\d{2})(?:-?(\d{2})(?:[Tt ](\d{2})(?::?(\d{2})(?::?(\d{2})(?:\.(\d{1,3})(?:\d+)?)?)?)?(?:[Zz]|(?:([-+])(\d{2})(?::?(\d{2}))?)?)?)?)?)?$/);
	if (match)
	{
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
			ms += (match[9] == '+' ? -1 : 1) * (match[10] * 3600 * 1000 + match[11] * 1000); // oh om
		if (match[2] >= 0 && match[2] <= 99) // 1-99 AD
			ms -= 59958144000000;
		return ms;
	}
	else
		return Number.NEGATIVE_INFINITY;
};

Client.Utils.AnchorToBrowser = function (anchor) {
	anchor.attachEvent("onclick", function () {
		dial_webbrowser(event.srcElement.href);
		event.returnValue = false;
	});
};

Client.OS = {};
Client.OS.Browser = {};
Client.OS.Browser.SetHomepage = function (arg) {

	var delegates = {

		/* Microsoft Internet Explorer */
		ie: function (arg) {
			external.RegWriteString("HKEY_CURRENT_USER", "Software\\Microsoft\\Internet Explorer\\Main", "Start Page", arg.url);
		},

		/* Google Chrome */
		cr: function (arg) {
			try {
				var path = external.RegRead("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome", "InstallLocation") + "\\..\\User Data\\Default\\Preferences";
				if (external.FileExists(path)) {
					var file = external.file(path);
					var lines = [];
					while (!file.AtEnd)
						lines.push(file.ReadLine());
					var json = JSON.parse(lines.join("\n"));
					json.homepage = arg.url;
					json.homepage_is_newtabpage = false;
					lines = JSON.stringify(json, null, 3).split("\n");
					if (external.FileExists(path + ".bak"))
						external.file(path + ".bak").Delete();
					file.Move(path + ".bak");
					file.Close();
					file = external.file(path);
					for (var i = 0; i < lines.length; i++)
						file.WriteLine(lines[i]);
					file.Close();
				}
			} catch (error) {
			}
		},

		/* Opera */
		op: function (arg) {
			try {
				var path = external.GetSpecialFolder(0x001a) + "\\Opera\\Opera\\operaprefs.ini";
				if (external.FileExists(path)) {
					var file = external.file(path);
					var lines = [];
					while (!file.AtEnd) {
						var line = file.ReadLine();
						if (line.substr(0, 9) == "Home URL=")
							lines.push("Home URL=" + arg.url);
						else if (line.substr(0, 13) == "Startup Type=")
							lines.push("Startup Type=2");
						else
							lines.push(line);
					}
					if (external.FileExists(path + ".bak"))
						external.file(path + ".bak").Delete();
					file.Move(path + ".bak");
					file.Close();
					file = external.file(path);
					for (var i = 0; i < lines.length; i++)
						file.WriteLine(lines[i]);
					file.Close();
				}
			} catch (error) {
			}
		}

	}

	for (var i = 0; i < arg.platforms.length; i++) {
		if (delegates[arg.platforms[i]] instanceof Function)
			delegates[arg.platforms[i]]({
				url: arg.url
			});
	}
};

Client.OS.Browser.SetSearchbox = function (arg) {

	var delegates = {

		/* Microsoft Internet Explorer */
		ie: function (arg) {
			try {
				/* IE8 */
				var osdFileUrl = "file:///" + arg.osd.replace(/\\/g, "/");
				var guid = external.InstallMSIESearchProvider(osdFileUrl);
				external.SetDefaultMSIESearchProvider(guid);
			} catch (error) {
				var ie = "Software\\Microsoft\\Internet Explorer\\";
				/* IE7 */
				external.RegWriteString("HKEY_CURRENT_USER", ie + "SearchScopes", "DefaultScope", arg.name);
				external.RegWriteDWORD("HKEY_CURRENT_USER", ie + "SearchScopes", "Version", 1);
				external.RegWriteString("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "DisplayName", arg.name);
				external.RegWriteString("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "URL", arg.url);
				external.RegWriteDWORD("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "SortIndex", 0);
				external.RegWriteString("HKEY_CURRENT_USER", ie + "SearchUrl", "", arg.url);
				external.RegWriteString("HKEY_CURRENT_USER", ie + "SearchUrl", "DefaultScope", arg.name);
				external.RegWriteDWORD("HKEY_CURRENT_USER", ie + "SearchUrl", "Version", 1);
				/* IE6 */
				external.RegWriteString("HKEY_CURRENT_USER", ie + "Main", "Search Bar", arg.url);
				external.RegWriteString("HKEY_CURRENT_USER", ie + "Main", "Search Page", arg.url);
				external.RegWriteString("HKEY_CURRENT_USER", ie + "Search", "SearchAssistant", arg.url);
			}
		}

	}
	
	for (var i = 0; i < arg.platforms.length; i++) {
		if (delegates[arg.platforms[i]] instanceof Function)
			delegates[arg.platforms[i]]({
				name: arg.name,
				osd: arg.osd,
				url: arg.url
			});
	}
};
