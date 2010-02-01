function dial_webbrowser (address) {
	if (address.substr(0, 5).toLowerCase() === "xmpp:") {
		var xmppUri = new XMPPAddress(address.substr(5));
		switch (xmppUri.Querytype) {
			case "":
			case "message":
				/* Open a conversation */
				dial_chat(xmppUri);
				break;
			case "roster":
			case "subscribe":
				/* Add to contact list */
				var dom = new ActiveXObject("Msxml2.DOMDocument");
				dom.loadXML('<iq type="set"><query xmlns="jabber:iq:roster"><item/></query></iq>');
				dom.documentElement.firstChild.firstChild.setAttribute("jid", xmppUri.ShortAddress());
				dom.documentElement.setAttribute("to", xmppUri.ShortAddress());
				warn("SENT: " + dom.xml);
				external.XMPP.SendXML(dom);

				/* Request a subscription */
				dom.loadXML('<presence type="subscribe"/>');
				dom.documentElement.setAttribute("to", xmppUri.ShortAddress());
				warn("SENT: " + dom.xml);
				external.XMPP.SendXML(dom);

				external.wnd.messageBox(false, external.globals("Translator").Translate("main", "msg_cl_adding", [xmppUri.ShortAddress()]), external.globals("softwarename"), 0 | 64);
				break;
			case "remove":
			case "unsubscribe":
				/* Delete from roster */
				if (external.globals("ClientRoster").Items.Exists(xmppUri.ShortAddress()))
					external.globals("ClientRoster").Items(xmppUri.ShortAddress()).Purge();
				break;
			case "invite":
			case "join":
				dial_conference(xmppUri.ShortAddress());
				break;
			case "register":
				dial_service_register(xmppUri.ShortAddress());
				break;
			case "vcard":
				dial_userinfo(xmppUri.ShortAddress(), xmppUri.Resource);
				break;
		}
	} else if (address.substr(0, 7).toLowerCase() === "http://"
		|| address.substr(0, 8).toLowerCase() === "https://") {
		var HTTPHandler = "";
		try {
			HTTPHandler = external.RegRead("HKEY_CLASSES_ROOT", "HTTP\\shell\\open\\command", "");
		} catch (e) {}
		if (HTTPHandler.toLowerCase().indexOf("iexplore.exe") !== -1)
			window.open(address, "_blank");
		else
			external.shellExec("open", address, "", "", 0);
	} else
		external.shellExec("open", address, "", "", 0);
}
