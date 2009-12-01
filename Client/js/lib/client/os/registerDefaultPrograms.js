/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.os");

client.os.registerDefaultPrograms = function (arg) {
	var reg = client.os.registry;
	var keyClientCategory = "Software\\Clients\\" + arg.client.category;
	var keyClient = keyClientCategory + "\\" + arg.name;
	var keyCapabilities = keyClient + "\\Capabilities";

	/* Registration as default program for specific service (IM, email, web browser, ...) */
	reg.writeLM(keyClientCategory, "", arg.name);
	reg.writeLM(keyClient, "", arg.name);
	reg.writeLM(keyClient + "\\InstallInfo", "IconsVisible", 1);
	reg.writeLM(keyClient + "\\InstallInfo", "ShowIconsCommand", arg.client.show);
	reg.writeLM(keyClient + "\\InstallInfo", "HideIconsCommand", arg.client.hide);
	reg.writeLM(keyClient + "\\InstallInfo", "ReinstallCommand", arg.client.reinstall);

	/* Registration as default handler of file extension, MIME type, URI protocol */
	reg.writeLM("Software\\RegisteredApplications", arg.name, keyCapabilities);
	reg.writeLM(keyCapabilities, "ApplicationIcon", arg.icon);
	reg.writeLM(keyCapabilities, "ApplicationName", arg.name);
	reg.writeLM(keyCapabilities, "ApplicationDescription", arg.description);

	/* Documentation: http://msdn.microsoft.com/en-us/library/cc144154%28VS.85%29.aspx */
	for (var type in arg.associations) {
		var progid = arg.name + ("protocol" in arg.associations[type] ? ".Url." : ".File.") + type;
		var keyProgid = "Software\\Classes\\" + progid;
		var keyApplication = "Software\\Classes\\Applications\\" + arg.name + ".exe";

		/* ProgID */
		if ("extension" in arg.associations[type] || "protocol" in arg.associations[type]) {
			reg.writeLMCU(keyProgid, "", arg.associations[type].name);
			reg.writeLMCU(keyProgid + "\\DefaultIcon", "", arg.associations[type].icon);
			reg.writeLMCU(keyProgid + "\\shell\\open\\command", "", arg.associations[type].command);
			if (arg.associations[type].shortcut)
				reg.writeLMCU(keyProgid, "IsShortcut", "");
		}

		if ("extension" in arg.associations[type]) {
			/* Open automatically in web browser */
			reg.writeLMCU(keyProgid, "BrowserFlags", 0x00000008);
			reg.writeLMCU(keyProgid, "EditFlags", 0x00010000);

			/* Appearance in Open With... */
			reg.writeLMCU(keyApplication, "NoOpenWith", "");
			reg.writeLMCU(keyApplication + "\\DefaultIcon", "", arg.associations[type].icon);
			reg.writeLMCU(keyApplication + "\\shell\\open\\command", "", arg.associations[type].command);

			for (var i in arg.associations[type].extension) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\" + arg.associations[type].extension[i] + "\\UserChoice";
				var keyExtension = "Software\\Classes\\" + arg.associations[type].extension[i];
				/* Register the file extension and link to the ProgID */
				reg.writeLMCU(keyExtension, "", progid);
				/* Always show up for this extension in Open With... */
				reg.writeLMCU(keyExtension + "\\OpenWithList\\" + arg.name + ".exe", "", "");
				reg.writeLMCU(keyExtension + "\\OpenWithProgIds", progid, "");
				reg.writeLMCU(keyApplication + "\\SupportedTypes", arg.associations[type].extension[i], "");
				/* Add extension support to Default Programs */
				reg.writeLM(keyCapabilities + "\\FileAssociations", arg.associations[type].extension[i], progid);
				reg.writeCU(keyUserChoice, "Progid", progid);
			}
		}

		if ("protocol" in arg.associations[type]) {
			/* Flag to designate the progid as a protocol handler */
			reg.writeLMCU(keyProgid, "URL Protocol", "");
			for (var i in arg.associations[type].protocol) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\" + arg.associations[type].protocol[i] + "\\UserChoice";
				/* Register the URI protocol and copy the progid information */
				var keyProtocol = "Software\\Classes\\" + arg.associations[type].protocol[i];
				reg.writeLMCU(keyProtocol, "", arg.associations[type].name);
				reg.writeLMCU(keyProtocol, "URL Protocol", "");
				reg.writeLMCU(keyProtocol + "\\DefaultIcon", "", arg.associations[type].icon);
				reg.writeLMCU(keyProtocol + "\\shell\\open\\command", "", arg.associations[type].command);
				/* Add URI protocol support to Default Programs */
				reg.writeLM(keyCapabilities + "\\UrlAssociations", arg.associations[type].protocol[i], progid);
				reg.writeCU(keyUserChoice, "Progid", progid);
			}
		}

		if ("mime" in arg.associations[type]) {
			for (var i in arg.associations[type].mime) {
				var keyUserChoice = "Software\\Microsoft\\Windows\\Shell\\Associations\\MIMEAssociations\\" + arg.associations[type].mime[i] + "\\UserChoice";
				/* Link the MIME type to the ProgID */
				reg.writeLMCU(keyProgid, "Content Type", arg.associations[type].mime[i]);
				/* Add MIME type support to Default Programs */
				reg.writeLM(keyCapabilities + "\\MimeAssociations", arg.associations[type].mime[i], progid);
				reg.writeCU(keyUserChoice, "Progid", progid);
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
