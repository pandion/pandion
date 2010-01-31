/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.os");

client.os.browser = {};

client.os.browser.setHomepage = function (platforms, arg) {
	var reg = client.os.registry;
	var delegates = {

		/* Microsoft Internet Explorer */
		ie: function (arg) {
			reg.writeCU("Software\\Microsoft\\Internet Explorer\\Main", "Start Page", arg.homepage);
		},

		/* Google Chrome */
		cr: function (arg) {
			var writeSettings = function  () {
				try {
					var path = reg.readCU("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome", "InstallLocation") + "\\..\\User Data\\Default\\Preferences";
					if (external.FileExists(path)) {
						var file = external.file(path);
						var lines = [];
						while (!file.AtEnd)
							lines.push(file.ReadLine());
						var json = JSON.parse(lines.join("\n"));
						json.homepage = arg.homepage;
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
				} catch (error) {};
			};
			writeSettings();
			if (external.IsProcessRunning("chrome.exe"))
				var isRunning = setInterval(function () {
					if (!external.IsProcessRunning("chrome.exe")) {
						writeSettings();
						clearTimeout(isRunning);
					}
				}, 1500);
		},

		/* Mozilla Firefox */
		ff: function (arg) {
			var firefox = "Software\\Mozilla\\Firefox\\Extensions";
			var extension = firefox + "\\" + arg.xpi.id;
			/* Load the extension in Firefox on next startup */
			reg.writeCU(firefox, arg.xpi.id, arg.xpi.path);
			reg.writeCU(extension, "setHomepage", 1);
			/* Search engine meta data */
			reg.writeCU(extension, "homepage", arg.homepage);
		},

		/* Opera */
		op: function (arg) {
			var writeSettings = function  () {
				try {
					var path = external.GetSpecialFolder(0x001a) + "\\Opera\\Opera\\operaprefs.ini";
					if (external.FileExists(path)) {
						var file = external.file(path);
						var lines = [];
						while (!file.AtEnd) {
							var line = file.ReadLine();
							if (line.substr(0, 9) == "Home URL=")
								lines.push("Home URL=" + arg.homepage);
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
				} catch (error) {}
			};
			writeSettings();
			if (external.IsProcessRunning("opera.exe"))
				var isRunning = setInterval(function () {
					if (!external.IsProcessRunning("opera.exe")) {
						writeSettings();
						clearTimeout(isRunning);
					}
				}, 1500);
		}

	};

	for (var i = 0; i < platforms.length; i++)
		if (platforms[i] in delegates)
			delegates[platforms[i]](arg);
};

client.os.browser.setSearchbox = function (platforms, arg) {
	var reg = client.os.registry;
	var delegates = {

		/* Microsoft Internet Explorer */
		ie: function (arg) {
			var OpenServiceErrors = {
				OS_E_NOTFOUND: 0x80030002,
				OS_E_NOTSUPPORTED: 0x80004021,
				OS_E_CANCELLED: 0x80002ef1,
				OS_E_GPDISABLED: 0xc00d0bdc
			};
			try {
				/* IE8 */
				var guid = external.InstallMSIESearchProvider(arg.osd);
				var foo = external.SetDefaultMSIESearchProvider(guid);
			} catch (error) {
				if (0 === (error.number ^ OpenServiceErrors.OS_E_CANCELLED)) {
					/* Also cancel setting of search box in other browsers. */
					return false;
				} else {
					var ie = "Software\\Microsoft\\Internet Explorer\\";
					/* IE7 */
					reg.writeCU(ie + "SearchScopes", "DefaultScope", arg.name);
					reg.writeCU(ie + "SearchScopes", "Version", 1);
					reg.writeCU(ie + "SearchScopes\\" + arg.name, "DisplayName", arg.name);
					reg.writeCU(ie + "SearchScopes\\" + arg.name, "URL", arg.searchpage);
					reg.writeCU(ie + "SearchScopes\\" + arg.name, "SortIndex", 0);
					reg.writeCU(ie + "SearchUrl", "", arg.searchpage);
					reg.writeCU(ie + "SearchUrl", "DefaultScope", arg.name);
					reg.writeCU(ie + "SearchUrl", "Version", 1);
					/* IE6 */
					reg.writeCU(ie + "Main", "Search Bar", arg.searchpage);
					reg.writeCU(ie + "Main", "Search Page", arg.searchpage);
					reg.writeCU(ie + "Search", "SearchAssistant", arg.searchpage);
				}
			}
		},

		/* Mozilla Firefox */
		ff: function (arg) {
			var firefox = "Software\\Mozilla\\Firefox\\Extensions";
			var extension = firefox + "\\" + arg.xpi.id;
			/* Load the extension in Firefox on next startup */
			reg.writeCU(firefox, arg.xpi.id, arg.xpi.path);
			reg.writeCU(extension, "setSearchbox", 1);
			/* Search engine meta data */
			reg.writeCU(extension, "icon", arg.icon);
			reg.writeCU(extension, "keyword", arg.keyword);
			reg.writeCU(extension, "name", arg.name);
			reg.writeCU(extension, "osd", arg.osd);
			reg.writeCU(extension, "replace", arg.replace);
		}

	};

	for (var i = 0; i < platforms.length; i++)
		if (platforms[i] in delegates)
			if (!delegates[platforms[i]](arg))
				break;
};
