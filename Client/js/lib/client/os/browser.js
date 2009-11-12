client.namespace("client.os");

client.os.browser = {};

client.os.browser.setHomepage = function (arg) {

	var delegates = {

		/* Microsoft Internet Explorer */
		ie: function (arg) {
			client.os.registry.write("HKEY_CURRENT_USER", "Software\\Microsoft\\Internet Explorer\\Main", "Start Page", arg.url);
		},

		/* Google Chrome */
		cr: function (arg) {
			try {
				var path = client.os.Registry.Read("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome", "InstallLocation") + "\\..\\User Data\\Default\\Preferences";
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

client.os.browser.setSearchbox = function (arg) {

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
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchScopes", "DefaultScope", arg.name);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchScopes", "Version", 1);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "DisplayName", arg.name);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "URL", arg.url);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchScopes\\" + arg.name, "SortIndex", 0);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchUrl", "", arg.url);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchUrl", "DefaultScope", arg.name);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "SearchUrl", "Version", 1);
				/* IE6 */
				client.os.registry.write("HKEY_CURRENT_USER", ie + "Main", "Search Bar", arg.url);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "Main", "Search Page", arg.url);
				client.os.registry.write("HKEY_CURRENT_USER", ie + "Search", "SearchAssistant", arg.url);
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
