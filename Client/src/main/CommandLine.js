/* Fire the event in a seperate "thread".
 */
function CommandLine ( command )
{
	external.globals( 'CommandLineQueue' ).push( command );
	setTimeout( CommandLineShift, 0 );
	if ( external.windows.Exists( 'signup' ) )
	{
		external.wnd.hide( true );
		external.windows( 'signup' ).restore();
		external.windows( 'signup' ).focus();
	}
}

/* Parse the command line.
 */
function CommandLineShift ()
{
	var command = external.globals( 'CommandLineQueue' ).shift();
	var argument = command;
	if ( argument.charAt( 0 ) == '"' )
		argument = argument.substr( argument.indexOf( '"', 1 ) + 2 );
	else
		argument = argument.substr( argument.indexOf( ' ', 1 ) + 1 );

	if ( argument.charAt( 0 ) == '"' && argument.indexOf( '"', 1 ) != -1 )
		argument = argument.substr( 1, argument.indexOf( '"', 1 ) - 1 );

	while ( argument.charAt( 0 ) == ' ' )
		argument = argument.substr( 1 );

	warn( 'EVNT: CommandLine ' + argument );

	/* Shut down the client immediatly
	 */
	if ( argument.substr( 0, 6 ) == '/close' )
	{
		setTimeout( 'external.wnd.close()', 0 );
	}

	/* Hide the client in the tray
	 */
	else if ( argument.substr( 0, 9 ) == '/minimize' )
	{
		external.wnd.minimize();
		external.wnd.hide( true );
	}

	/* Default Programs - remove all shortcuts
	 */
	else if ( argument.substr( 0, 5 ) == '/hide' )
	{
		var locations = [
			external.GetSpecialFolder(16), // DESKTOPDIRECTORY
			external.GetSpecialFolder(25), // COMMON_DESKTOPDIRECTORY
			external.GetSpecialFolder(7), // STARTUP
			external.GetSpecialFolder(24), // COMMON_STARTUP
			external.GetSpecialFolder(2), // PROGRAMS
			external.GetSpecialFolder(23), // COMMON_PROGRAMS
			external.GetSpecialFolder(26) + "\\Microsoft\\Internet Explorer\\Quick Launch", // APPDATA Quick Launch
			external.GetSpecialFolder(35) + "\\Microsoft\\Internet Explorer\\Quick Launch" // COMMON_APPDATA Quick Launch
		];
		for (var i in locations) {
			var shortcut = locations[i] + "\\" + external.globals("softwarenamesafe") + ".lnk";
			if (external.FileExists(shortcut))
				try {
					external.file(shortcut).Delete();
				} catch (error) {
				}
		}
		try {
			external.RegWriteDWORD("HKEY_LOCAL_MACHINE", "Software\\Clients\\IM\\" + external.globals("softwarenamesafe") + "\\InstallInfo", "IconsVisible", 0);
		} catch (error) {
		}
		if (external.CmdLine == command)
			setTimeout( 'external.wnd.close()', 0 );
	}

	/* Default Programs - reveal all shortcuts
	 */
	else if ( argument.substr( 0, 5 ) == '/show' || argument.substr( 0, 10 ) == '/reinstall' )
	{
		var locations = [
			[external.GetSpecialFolder(25), // COMMON_DESKTOPDIRECTORY
			external.GetSpecialFolder(16), // DESKTOPDIRECTORY
			""],
			[external.GetSpecialFolder(24), // COMMON_STARTUP
			external.GetSpecialFolder(7), // STARTUP
			"/minimized"],
			[external.GetSpecialFolder(23), // COMMON_PROGRAMS
			external.GetSpecialFolder(2), // PROGRAMS
			""]
		];
		var shortcut = external.Shortcut;
		shortcut.path = external.globals("cwd") + "..\\" + external.globals("softwarenamesafe") + ".exe";
		shortcut.IconLocation = external.globals("cwd") + '..\\images\\brand\\default.ico,0';
		for (var i in locations) {
			shortcut.Arguments = locations[i][2];
			try {
				shortcut.Save(locations[i][0] + "\\" + external.globals("softwarenamesafe") + ".lnk");
			} catch (error) {
				try {
					shortcut.Save(locations[i][1] + "\\" + external.globals("softwarenamesafe") + ".lnk");
				} catch (error) {
				}
			}
		}	
		try {
			external.RegWriteDWORD("HKEY_LOCAL_MACHINE", "Software\\Clients\\IM\\" + external.globals("softwarenamesafe") + "\\InstallInfo", "IconsVisible", 1);
		} catch (error) {
		}
		if (external.CmdLine == command)
			setTimeout( 'external.wnd.close()', 0 );
	}

	/* Queue the command until we are fully authenticated
	 */
	else if ( ! external.globals( 'XMPPConnected' ) )
	{
		external.globals( 'CommandLineQueue' ).push( command );
	}

	/* "xmpp:" hyperlink
	 */
	else if ( argument.substr( 0, 5 ) == 'xmpp:' )
	{
		var address = new XMPPAddress(argument.substring(5, argument.indexOf("?") == -1 ? argument.length : argument.indexOf("?")));
		var queryType = argument.indexOf("?") == -1 ? "" : argument.substring(argument.indexOf("?") + 1, argument.length);
		if (queryType.indexOf(";") != -1)
			queryType = queryType.substring(0, queryType.indexOf(";"));
		if (queryType.indexOf("#") != -1)
			queryType = queryType.substring(0, queryType.indexOf("#"));
		switch (queryType) {
			case "":
			case "message":
				/* Open a conversation */
				dial_chat(address);
				break;
			case "roster":
			case "subscribe":
				/* Add to contact list */
				var dom = new ActiveXObject("Msxml2.DOMDocument");
				dom.loadXML('<iq type="set"><query xmlns="jabber:iq:roster"><item/></query></iq>');
				dom.documentElement.firstChild.firstChild.setAttribute("jid", address.ShortAddress());
				dom.documentElement.setAttribute("to", address.ShortAddress());
				warn("SENT: " + dom.xml);
				external.XMPP.SendXML(dom);

				/* Request a subscription */
				dom.loadXML('<presence type="subscribe"/>');
				dom.documentElement.setAttribute("to", address.ShortAddress());
				warn("SENT: " + dom.xml);
				external.XMPP.SendXML(dom);

				external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_cl_adding', [ address.ShortAddress() ] ), external.globals( 'softwarename' ), 0 | 64 );
				break;
			case "remove":
			case "unsubscribe":
				/* Delete from roster */
				if (external.globals("ClientRoster").Items.Exists(address.ShortAddress()))
					external.globals("ClientRoster").Items(address.ShortAddress()).Purge();
				break;
			case "invite":
			case "join":
				dial_conference(address.ShortAddress());
				break;
			case "register":
				dial_service_register(address.ShortAddress());
				break;
			case "vcard":
				dial_userinfo(address.ShortAddress(), address.Resource);
				break;
		}
	}

	/* Load file
	 */
	else if ( argument.length && external.FileExists( argument ) )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.async = false;
		dom.resolveExternals = false;
		dom.load( argument );

		/* Plug-in XML file
		 */
		if ( dom.documentElement )
		{
			/* Install and load the plugin
			 */
			if ( dom.selectSingleNode( '/plugin[@type and name and description and version]' ) )
			{
				var filename = argument;
				var slash = Math.max( filename.lastIndexOf( '/' ), filename.lastIndexOf( '\\' ) );
				if ( slash > -1 )
					filename = filename.substr( slash + 1 );
				filename = filename.replace( /\[\d+\]\.pdn$/, '.pdn' );
				if ( external.FileExists( external.globals( 'usersdir' ) + 'Plugins\\' + filename ) )
					external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_plugin_exists' ), external.globals( 'softwarename' ), 0 | 48 );
				else if ( external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_plugin_install', [ dom.selectSingleNode( '/plugin[@type]/name' ).text, dom.selectSingleNode( '/plugin[@type]/description' ).text ] ), external.globals( 'softwarename' ), 4 | 48 ) == 6 )
				{
					var Path = external.globals( 'usersdir' ) + 'Plugins\\';
					if ( ! external.Directory.Exists( Path ) )
						external.Directory.Create( Path );
					dom.save( Path + filename );
					external.globals( 'ClientPluginContainer' ).Reload();
					external.File( argument ).Delete();
				}
			}
		}
		/* Extract icon package
		 */
		else
		{
			var Directories	= new VBArray( external.Directory.ListDirs( external.globals( 'usersdir' ) + 'Emoticons\\' ) ).toArray();
			var Styles		= new ActiveXObject( 'Scripting.Dictionary' );
			for ( var i = 0; i < Directories.length; ++i )
				Styles.Add( Directories[i].Name, null );

			if ( external.UnZip( argument, external.globals( 'usersdir' ) + 'Emoticons\\' ) )
			{
				Directories	= new VBArray( external.Directory.ListDirs( external.globals( 'usersdir' ) + 'Emoticons\\' ) ).toArray();
				for ( var i = 0; i < Directories.length; ++i )
					if ( ! Styles.Exists( Directories[i].Name ) )
					{
						external.globals( 'cfg' )( 'emoticonset' ) = Directories[i].Name;
						if ( external.windows.Exists( 'emoticon_list' ) )
							external.windows( 'emoticon_list' ).Do( 'emoticon_refresh', null );
						if ( external.windows.Exists( 'emoticon_download' ) )
							external.windows( 'emoticon_download' ).Do( 'Draw', null );
						dial_emoticon_viewer( Directories[i].Name );
						return;
					}
				external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_emoticon_install' ), external.globals( 'softwarename' ), 0 | 64 );
			}
		}
	}
}
