/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
client.namespace("client.os");

client.os.launchInBrowser = function (address) {
	if (address.substr(0, 7) == "http://" || address.substr(0, 8) == "https://") {
		try {
			//Windows 10 default browser
			var W10AppClass = external.RegRead("HKEY_CLASSES_ROOT", external.RegRead("HKEY_CURRENT_USER","Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice","ProgId") + "\\shell\\open\\command", "");
			if (W10AppClass.indexOf("iexplore.exe") != -1)
				return window.open(address, "_blank");
		} catch(e) {
			try {
				var HTTPHandler = client.os.registry.readCR("HTTP\\shell\\open\\command", "");
				if (HTTPHandler.indexOf("iexplore.exe") != -1)
					return window.open(address, "_blank");
			} catch(e) {}
		}
	}
	external.shellExec("open", address, "", "", 5);
};
