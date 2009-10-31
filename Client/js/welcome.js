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
	/* TODO
	if (external.globals("welcomesettings").toString() == "true") {
		Client.CSS.Hide(document.getElementById("settings"));
		Client.CSS.Show(document.getElementById("shortcuts"));
	} else {
		Client.CSS.Show(document.getElementById("settings"));
		Client.CSS.Hide(document.getElementById("shortcuts"));
	}
	*/
	Client.CSS.Show(document.getElementById("settings")); // TODO
	Client.CSS.Hide(document.getElementById("shortcuts")); // TODO
	if (external.wnd.params.document.getElementById("signin-dialog").style.display == "block")
		Client.CSS.Disable(document.getElementById("shortcuts"));

	var saveSettings = function () {
		// TODO external.globals("welcomesettings") = false;

		if (document.getElementById("default-xmpp-client").checked) {
			var logo = external.globals("cwd") + "..\\images\\brand\\default.ico";
			var open = "\"" + external.globals("cwd") + "..\\" + external.globals("softwarenamesafe") + ".exe\"";
			Client.OS.Registry.RegisterDefaultPrograms({
				description: external.globals("Translator").Translate("main", "program-description"),
				icon: logo,
				name: external.globals("softwarenamesafe"),
				client: {
					category: "IM",
					show: open + " /registerdefaultprogram show",
					hide: open + " /registerdefaultprogram hide",
					reinstall: open + " /registerdefaultprogram reinstall",
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
						mime: ["application/jabber+xml"]
					},
					"xmpp": {
						command: open + " \"%1\"",
						icon: logo,
						name: external.globals("Translator").Translate("main", "uri-xmpp"),
						protocol: ["xmpp"]
					}
				}
			});
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
		if (Client.CSS.HasClass(document.getElementById("settings"), "visible"))
			saveSettings();
		external.globals("welcomescreen") = document.getElementById("show-welcome").checked ? "yes" : "no";
		event.returnValue = false;
		external.wnd.close();
	});

	document.getElementById("settings-close").attachEvent("onclick", function () {
		saveSettings();
		Client.CSS.Hide(document.getElementById("settings"));
		Client.CSS.Show(document.getElementById("shortcuts"));
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

	Client.CSS.Show(document.getElementById("feed-loading"));
	Client.CSS.Hide(document.getElementById("feed-unavailable"));
	Client.CSS.Hide(document.getElementById("feed"));
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
			Client.CSS.Hide(document.getElementById("feed-loading"));
			if (doc) {
				var feed = document.getElementById("feed");
				Client.CSS.Show(feed);
				var entries = doc.selectNodes("/feed/entry[updated][title][link[@rel='alternate' and @type='text/html' and @href]]");
				for (var i = 0; i < entries.length; ++i) {
					var listItem = document.createElement("li");
					var anchor = document.createElement("a");
					anchor.innerText = getPlainText(entries[i], "title");
					anchor.href = entries[i].selectSingleNode("link[@rel='alternate' and @type='text/html' and @href]").getAttribute("href");
					Client.Utils.AnchorToBrowser(anchor);
					listItem.appendChild(anchor);
					var timestamp = document.createElement("div");
					Client.CSS.AddClass(timestamp, "timestamp");
					timestamp.innerText = Client.Utils.Prettytime(Client.Utils.ISO8601(entries[i].selectSingleNode("updated").text));
					listItem.appendChild(timestamp);
					feed.appendChild(listItem);
				}
			}
			else
				Client.CSS.Show(document.getElementById("feed-unavailable"));
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
		Client.CSS.AddClass(arg.element, "active");
		if (arg.element.value === arg.placeholder)
			arg.element.value = "";
	});
	arg.element.attachEvent("onblur", function () {
		if (arg.element.value.length == 0)
		{
			Client.CSS.RemoveClass(arg.element, "active");
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

Client.OS.Registry = {};

Client.OS.Registry.Read = function (hive, key, value) {
	try {
		return external.RegRead(hive, key, value);
	} catch (error) {
		return null;
	}
};

Client.OS.Registry.Write = function (hive, key, value, data) {
	try {
		if ((data instanceof String) || (typeof data == 'string'))
			external.RegWriteString(hive, key, value, data);
		else if ((data instanceof Number) || (typeof data == 'number'))
			external.RegWriteDWORD(hive, key, value, data);
		else
			return false;
		return true;
	} catch (error) {
		return false;
	}
};

Client.OS.Registry.Delete = function (hive, key, value) {
	try {
		if (value == undefined)
			external.RegDeleteKey(hive, key);
		else
			external.RegDeleteValue(hive, key, value);
		return true;
	} catch (error) {
		return false;
	}
};

Client.OS.Registry.RegisterDefaultPrograms = function (arg) {
	var registryWriteHklmOrHkcu = function (key, value, data) {
		if (Client.OS.Registry.Write(hklm, key, value, data))
			Client.OS.Registry.Delete(hkcu, key, value);
		else
			Client.OS.Registry.Write(hkcu, key, value, data);
	};
	var hkcu = "HKEY_CURRENT_USER";
	var hklm = "HKEY_LOCAL_MACHINE";
	var keyClientCategory = "Software\\Clients\\" + arg.client.category;
	var keyClient = keyClientCategory + "\\" + arg.name;
	var keyCapabilities = keyClient + "\\Capabilities";

	/* Registration as default program for specific service (IM, email, web browser, ...) */
	Client.OS.Registry.Write(hklm, keyClientCategory, "", arg.name);
	Client.OS.Registry.Write(hklm, keyClient, "", arg.name);
	Client.OS.Registry.Write(hklm, keyClient + "\\InstallInfo", "IconsVisible", 1);
	Client.OS.Registry.Write(hklm, keyClient + "\\InstallInfo", "ShowIconsCommand", arg.client.show);
	Client.OS.Registry.Write(hklm, keyClient + "\\InstallInfo", "HideIconsCommand", arg.client.hide);
	Client.OS.Registry.Write(hklm, keyClient + "\\InstallInfo", "ReinstallCommand", arg.client.reinstall);

	/* Registration as default handler of file extension, MIME type, URI protocol */
	Client.OS.Registry.Write(hklm, "Software\\RegisteredApplications", arg.name, keyCapabilities);
	Client.OS.Registry.Write(hklm, keyCapabilities, "ApplicationIcon", arg.icon);
	Client.OS.Registry.Write(hklm, keyCapabilities, "ApplicationName", arg.name);
	Client.OS.Registry.Write(hklm, keyCapabilities, "ApplicationDescription", arg.description);

	/* Documentation: http://msdn.microsoft.com/en-us/library/cc144154%28VS.85%29.aspx */
	for (var type in arg.associations) {
		var progid = arg.name + ("protocol" in arg.associations[type] ? ".Url." : ".File.") + type;
		var keyProgid = "Software\\Classes\\" + progid;
		var keyApplication = "Software\\Classes\\Applications\\" + arg.name + ".exe";

		/* ProgID */
		if ("extension" in arg.associations[type] || "protocol" in arg.associations[type]) {
			registryWriteHklmOrHkcu(keyProgid, "", arg.associations[type].name);
			registryWriteHklmOrHkcu(keyProgid + "\\DefaultIcon", "", arg.associations[type].icon);
			registryWriteHklmOrHkcu(keyProgid + "\\shell\\open\\command", "", arg.associations[type].command);
		}

		if ("extension" in arg.associations[type]) {
			/* Open automatically in web browser */
			registryWriteHklmOrHkcu(keyProgid, "BrowserFlags", 0x00000008);
			registryWriteHklmOrHkcu(keyProgid, "EditFlags", 0x00010000);

			/* Appearance in Open With... */
			registryWriteHklmOrHkcu(keyApplication, "NoOpenWith", "");
			registryWriteHklmOrHkcu(keyApplication + "\\DefaultIcon", "", arg.associations[type].icon);
			registryWriteHklmOrHkcu(keyApplication + "\\shell\\open\\command", "", arg.associations[type].command);

			for (var i in arg.associations[type].extension) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\" + arg.associations[type].extension[i] + "\\UserChoice";
				var keyExtension = "Software\\Classes\\" + arg.associations[type].extension[i];
				/* Register the file extension and link to the ProgID */
				registryWriteHklmOrHkcu(keyExtension, "", progid);
				/* Always show up for this extension in Open With... */
				registryWriteHklmOrHkcu(keyExtension + "\\OpenWithList\\" + arg.name + ".exe", "", "");
				registryWriteHklmOrHkcu(keyExtension + "\\OpenWithProgIds", progid, "");
				registryWriteHklmOrHkcu(keyApplication + "\\SupportedTypes", arg.associations[type].extension[i], "");
				/* Add extension support to Default Programs */
				Client.OS.Registry.Write(hklm, keyCapabilities + "\\FileAssociations", arg.associations[type].extension[i], progid);
				Client.OS.Registry.Write(hkcu, keyUserChoice, "Progid", progid);
			}
		}

		if ("protocol" in arg.associations[type]) {
			/* Flag to designate the progid as a protocol handler */
			registryWriteHklmOrHkcu(keyProgid, "URL Protocol", "");
			for (var i in arg.associations[type].protocol) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\" + arg.associations[type].protocol[i] + "\\UserChoice";
				/* Register the URI protocol and copy the progid information */
				var keyProtocol = "Software\\Classes\\" + arg.associations[type].protocol[i];
				registryWriteHklmOrHkcu(keyProtocol, "", arg.associations[type].name);
				registryWriteHklmOrHkcu(keyProtocol, "URL Protocol", "");
				registryWriteHklmOrHkcu(keyProtocol + "\\DefaultIcon", "", arg.associations[type].icon);
				registryWriteHklmOrHkcu(keyProtocol + "\\shell\\open\\command", "", arg.associations[type].command);
				/* Add URI protocol support to Default Programs */
				Client.OS.Registry.Write(hklm, keyCapabilities + "\\UrlAssociations", arg.associations[type].protocol[i], progid);
				Client.OS.Registry.Write(hkcu, keyUserChoice, "Progid", progid);
			}
		}

		if ("mime" in arg.associations[type]) {
			for (var i in arg.associations[type].mime) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\Shell\\Associations\\MIMEAssociations\\" + arg.associations[type].mime[i] + "\\UserChoice";
				/* Link the MIME type to the ProgID */
				registryWriteHklmOrHkcu(keyProgid, "Content Type", arg.associations[type].mime[i]);
				/* Add MIME type support to Default Programs */
				Client.OS.Registry.Write(hklm, keyCapabilities + "\\MimeAssociations", arg.associations[type].mime[i], progid);
				Client.OS.Registry.Write(hkcu, keyUserChoice, "Progid", progid);
			}
		}
	}
	/*
		TODO: After updating the registry keys, the program should broadcast the WM_SETTINGCHANGE message
		with wParam = 0 and lParam pointing to the null-terminated string "Software\Clients\[ClientTypeName]"
		to notify the operating system that the default client has changed.

		TODO: Any time you create or change a file association, it is important to notify the system that
		you have made a change by calling SHChangeNotify, and specifying the SHCNE_ASSOCCHANGED event.
		If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted. 
	*/
}

Client.OS.Browser = {};

Client.OS.Browser.SetHomepage = function (arg) {

	var delegates = {

		/* Microsoft Internet Explorer */
		ie: function (arg) {
			Client.OS.Registry.Write("HKEY_CURRENT_USER", "Software\\Microsoft\\Internet Explorer\\Main", "Start Page", arg.url);
		},

		/* Google Chrome */
		cr: function (arg) {
			try {
				var path = Client.OS.Registry.Read("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome", "InstallLocation") + "\\..\\User Data\\Default\\Preferences";
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
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchScopes", "DefaultScope", arg.name);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchScopes", "Version", 1);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "DisplayName", arg.name);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "URL", arg.url);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "SortIndex", 0);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchUrl", "", arg.url);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchUrl", "DefaultScope", arg.name);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "SearchUrl", "Version", 1);
				/* IE6 */
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "Main", "Search Bar", arg.url);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "Main", "Search Page", arg.url);
				Client.OS.Registry.Write("HKEY_CURRENT_USER", ie + "Search", "SearchAssistant", arg.url);
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

Client.CSS = {};

Client.CSS.HasClass = function (elem, name) {
	return (new RegExp("^|\b" + name + "\b|$")).test(elem.className);
};

Client.CSS.Show = function (elem) {
	Client.CSS.RemoveClass(elem, "hidden");
	Client.CSS.AddClass(elem, "visible");
};

Client.CSS.Hide = function (elem) {
	Client.CSS.RemoveClass(elem, "visible");
	Client.CSS.AddClass(elem, "hidden");
};

Client.CSS.Enable = function (elem) {
	elem.disabled = false;
	Client.CSS.RemoveClass(elem, "disabled");
	Client.CSS.AddClass(elem, "enabled");
};

Client.CSS.Disable = function (elem) {
	elem.disabled = true;
	Client.CSS.RemoveClass(elem, "enabled");
	Client.CSS.AddClass(elem, "disabled");
};

Client.CSS.AddClass = function (elem, name) {
	elem.className += " " + name;
};

Client.CSS.RemoveClass = function (elem, name) {
	var classes = elem.className.split(/\s/);
	var offset = 0;
	for (var i in classes)
		if (classes[i - offset] == name) {
			classes.splice(i - offset, 1);
			offset++;
		}
	elem.className = classes.join(" ");
};
