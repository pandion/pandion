client.namespace("client.os");

client.os.registerDefaultPrograms = function (arg) {
	var registryWriteHklmOrHkcu = function (key, value, data) {
		if (client.os.registry.write(hklm, key, value, data))
			client.os.registry.remove(hkcu, key, value);
		else
			client.os.registry.write(hkcu, key, value, data);
	};
	var hkcu = "HKEY_CURRENT_USER";
	var hklm = "HKEY_LOCAL_MACHINE";
	var keyClientCategory = "Software\\Clients\\" + arg.client.category;
	var keyClient = keyClientCategory + "\\" + arg.name;
	var keyCapabilities = keyClient + "\\Capabilities";

	/* Registration as default program for specific service (IM, email, web browser, ...) */
	client.os.registry.write(hklm, keyClientCategory, "", arg.name);
	client.os.registry.write(hklm, keyClient, "", arg.name);
	client.os.registry.write(hklm, keyClient + "\\InstallInfo", "IconsVisible", 1);
	client.os.registry.write(hklm, keyClient + "\\InstallInfo", "ShowIconsCommand", arg.client.show);
	client.os.registry.write(hklm, keyClient + "\\InstallInfo", "HideIconsCommand", arg.client.hide);
	client.os.registry.write(hklm, keyClient + "\\InstallInfo", "ReinstallCommand", arg.client.reinstall);

	/* Registration as default handler of file extension, MIME type, URI protocol */
	client.os.registry.write(hklm, "Software\\RegisteredApplications", arg.name, keyCapabilities);
	client.os.registry.write(hklm, keyCapabilities, "ApplicationIcon", arg.icon);
	client.os.registry.write(hklm, keyCapabilities, "ApplicationName", arg.name);
	client.os.registry.write(hklm, keyCapabilities, "ApplicationDescription", arg.description);

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
			if (arg.associations[type].shortcut)
				registryWriteHklmOrHkcu(keyProgid, "IsShortcut", "");
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
				client.os.registry.write(hklm, keyCapabilities + "\\FileAssociations", arg.associations[type].extension[i], progid);
				client.os.registry.write(hkcu, keyUserChoice, "Progid", progid);
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
				client.os.registry.write(hklm, keyCapabilities + "\\UrlAssociations", arg.associations[type].protocol[i], progid);
				client.os.registry.write(hkcu, keyUserChoice, "Progid", progid);
			}
		}

		if ("mime" in arg.associations[type]) {
			for (var i in arg.associations[type].mime) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\Shell\\Associations\\MIMEAssociations\\" + arg.associations[type].mime[i] + "\\UserChoice";
				/* Link the MIME type to the ProgID */
				registryWriteHklmOrHkcu(keyProgid, "Content Type", arg.associations[type].mime[i]);
				/* Add MIME type support to Default Programs */
				client.os.registry.write(hklm, keyCapabilities + "\\MimeAssociations", arg.associations[type].mime[i], progid);
				client.os.registry.write(hkcu, keyUserChoice, "Progid", progid);
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
};
