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
	if (external.globals("welcomesettings").toString() == "true") {
		client.css.hide(document.getElementById("settings"));
		client.css.show(document.getElementById("shortcuts"));
	} else {
		client.css.show(document.getElementById("settings"));
		client.css.hide(document.getElementById("shortcuts"));
	}
	if (external.wnd.params.document.getElementById("signin-dialog").style.display == "block")
		client.css.disable(document.getElementById("shortcuts"));

	var saveSettings = function () {
		external.globals("welcomesettings") = false;

		if (document.getElementById("default-xmpp-client").checked) {
			var logo = external.globals("cwd") + "..\\images\\brand\\default.ico";
			var open = "\"" + external.globals("cwd") + "..\\" + external.globals("softwarenamesafe") + ".exe\"";
			client.os.registerDefaultPrograms({
				description: external.globals("Translator").Translate("main", "program-description"),
				icon: logo,
				name: external.globals("softwarenamesafe"),
				client: {
					category: "IM",
					show: open + " /show",
					hide: open + " /hide",
					reinstall: open + " /reinstall"
				},
				associations: {
					"JISP": {
						command: open + " \"%1\"",
						icon: logo,
						name: external.globals("Translator").Translate("main", "extension-jisp"),
						extension: [".jisp"],
						mime: ["application/vnd.jisp"]
					},
					"PDN": {
						command: open + " \"%1\"",
						icon: logo,
						name: external.globals("Translator").Translate("main", "extension-pdn"),
						extension: [".pdn"],
						mime: ["application/x-pdn+xml"]
					},
					"XMPP": {
						command: open + " \"%1\"",
						icon: logo,
						name: external.globals("Translator").Translate("main", "extension-xmpp"),
						extension: [".xmpp"],
						mime: ["application/jabber+xml"],
						shortcut: true
					},
					"xmpp": {
						command: open + " \"%1\"",
						icon: logo,
						name: external.globals("Translator").Translate("main", "uri-xmpp"),
						protocol: ["xmpp"],
						shortcut: true
					}
				}
			});
		}

		if (document.getElementById("change-homepage").checked)
			external.wnd.params.client.os.browser.setHomepage(["ie", "ff", "cr", "op"], {
				homepage: external.globals("browserhomepage"),
				xpi: {
					path: external.globals("cwd") + "..\\search\\xpi\\",
					id: external.globals("browsersearchboxxpi")
				}
			});

		if (document.getElementById("change-search-engine").checked)
			external.wnd.params.client.os.browser.setSearchbox(["ie", "ff", "cr", "op"], {
				icon: external.globals("browsersearchboxicon"),
				keyword: external.globals("browsersearchboxkeyword"),
				name: external.globals("browsersearchboxosdname"),
				osd: external.globals("browsersearchboxosd"),
				replace: external.globals("browsersearchboxreplace"),
				searchpage: external.globals("browsersearchboxsearchpage"),
				xpi: {
					path: external.globals("cwd") + "..\\search\\xpi\\",
					id: external.globals("browsersearchboxxpi")
				}
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
		if (client.css.hasClass(document.getElementById("settings"), "visible"))
			saveSettings();
		external.globals("welcomescreen") = document.getElementById("show-welcome").checked ? "yes" : "no";
		event.returnValue = false;
		external.wnd.close();
	});

	document.getElementById("settings-close").attachEvent("onclick", function () {
		saveSettings();
		client.css.hide(document.getElementById("settings"));
		client.css.show(document.getElementById("shortcuts"));
		event.returnValue = false;
	});

	var anchors = document.getElementsByTagName("a");
	for (var i = 0; i < anchors.length; i++)
		if (anchors[i].href.indexOf("http") != -1)
			client.html.anchorToBrowser(anchors[i]);

	client.html.searchBox({
		element: document.getElementById("search-box"),
		placeholder: external.globals("Translator").Translate("welcome", "search-topics"),
		action: function () {
			var url = external.globals("ClientPluginContainer").ParseURL(
				external.globals("welcomesearchurl"),
				{query: document.getElementById("search-box").value}
			);
			client.os.launchInBrowser(url);
		}
	});

	client.css.show(document.getElementById("feed-loading"));
	client.css.hide(document.getElementById("feed-unavailable"));
	client.css.hide(document.getElementById("feed"));
	client.io.ajax({
		url: external.globals("ClientPluginContainer").ParseURL(external.globals("welcomefeedurl")),
		callback: function (dom, xhr) {
			var getPlainText = function (parent, tagName) {
				var node = null;
				if (node = parent.selectSingleNode(tagName + "[@type='text' or not(@type)]"))
					return node.text;
				else if (node = parent.selectSingleNode(tagName + "[@type='html']")) {
					var html = document.createElement("div");
					html.innerHtml = node.text;
					return html.innerText;
				}
				else if (node = parent.selectSingleNode(tagName + "[@type='xhtml']"))
					return node.text;
				else
					return "";
			};
			var feed = document.getElementById("feed");
			var entries = dom ? dom.selectNodes("/feed/entry[updated][title][link[@rel='alternate' and @type='text/html' and @href]]") : [];
			for (var i = 0; i < entries.length; ++i) {
				var listItem = document.createElement("li");
				var anchor = document.createElement("a");
				anchor.innerText = getPlainText(entries[i], "title");
				anchor.href = entries[i].selectSingleNode("link[@rel='alternate' and @type='text/html' and @href]").getAttribute("href");
				client.html.anchorToBrowser(anchor);
				listItem.appendChild(anchor);
				var timestamp = document.createElement("div");
				client.css.addClass(timestamp, "timestamp");
				timestamp.innerText = client.data.prettytime(client.data.iso8601(entries[i].selectSingleNode("updated").text));
				listItem.appendChild(timestamp);
				feed.appendChild(listItem);
			}
			client.css.hide(document.getElementById("feed-loading"));
			client.css.show(entries.length > 0 ? feed : document.getElementById("feed-unavailable"));
		}
	});

	document.getElementById("introduction").focus();
	external.wnd.hide(false);
});
