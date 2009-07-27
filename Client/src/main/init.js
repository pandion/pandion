function init ()
{
	/* Check if regserver succeeded
	 */
	try
	{
		external.globals.Count;
	}
	catch ( e )
	{
		alert( 'Error: Can not activate the programming interfaces.\n\nThere are several possible reasons for this problem:\n\nCause: You do not have write access to the Windows Registry.\nSolution: Log in to Windows with more access rights (Administrator) and reinstall the software.\n\nCause: The Windows Registry is full.\nSolution: Increase the maximum size of the registry.' );
		window.opener = '';
		return window.close();
	}

	/* Global objects and variables
	 */
	external.globals.Add( 'autoaway',				'' );											// timeout id
	external.globals.Add( 'autoawaymode',			0 );											// 2=away, 3=xaway
	external.globals.Add( 'autoawaytime',			0 );											// in-activity
	external.globals.Add( 'autologin',				false );										// skip the login window
	external.globals.Add( 'AutoUpdateTimeout',		null );											// timeout of check for new versions
	external.globals.Add( 'BackgroundHooks',		new ActiveXObject( 'Scripting.Dictionary' ) );	// names of backgrounds being downloaded
	external.globals.Add( 'BackgroundList',			new ActiveXObject( 'MSXML2.DOMDocument' ) );	// online xml list of backgrounds
	external.globals.Add( 'block',					new ActiveXObject( 'Scripting.Dictionary' ) );	// stores the addresses of all users on the blocking list
	external.globals.Add( 'cfg',					new ActiveXObject( 'Scripting.Dictionary' ) );	// default settings
	external.globals.Add( 'ChatSessionPool',		new ChatSessionPool() );						// handles conversations
	external.globals.Add( 'ConferenceSessionPool',	new ConferenceSessionPool() );					// handles conferences
	external.globals.Add( 'connected',				false );										// flag if we have a connection
	external.globals.Add( 'connecttime',			0 );											// the time when the connection was established
	external.globals.Add( 'CommandLineQueue',		new Array() );									// buffer of command line actions
	external.globals.Add( 'debug',					false );										// enables console output
	external.globals.Add( 'headlines',				new Array() );									// alerts and headlines buffer
	external.globals.Add( 'HTTPEnginePort',			0 );											// port on which the HTTPEngine is listening
	external.globals.Add( 'language',				'' );											// which translation is being used
	external.globals.Add( 'last_address',			'' );											// last used address
	external.globals.Add( 'last_autoupdate',		0 );											// time of the last auto update check
	external.globals.Add( 'last_password',			'' );											// last used password
	external.globals.Add( 'last_password_remember',	false );										// remember my last used password
	external.globals.Add( 'EmoticonCache',			new EmoticonCache() );							// loads and caches emoticon styles
	external.globals.Add( 'ClientRoster',			new ClientRoster() );							// stores and displays the contact list
	external.globals.Add( 'ClientServices',			new ClientBrowseServer() );						// the server's iq:browse, iq:agents or disco result
	external.globals.Add( 'pending',				new ActiveXObject( 'Scripting.Dictionary' ) );	// callback pool
	external.globals.Add( 'ClientPluginContainer',	new ClientPluginContainer() );					// callback pool
	external.globals.Add( 'settingsfile',			'' );											// location of profile settings
	external.globals.Add( 'soundtime',				0 );											// the time when new notification sounds can be played
	external.globals.Add( 'soundallowed',			true );											// globally allow sounds to be played
	external.globals.Add( 'stderrlength',			120 );											// maximum entries in console buffer
	external.globals.Add( 'stderr',					new Array() );									// console buffer
	external.globals.Add( 'Translator',				new Translator() );								// string table
	external.globals.Add( 'uniqueid',				0 );											// counter for id attributes in message and iq packets
	external.globals.Add( 'XMPPConnected',			false );										// connection state
	external.globals.Add( 'XMPPEncryption',			'' );											// which method of encryption is being used
	external.globals.Add( 'XMPPSASLMechanism',		'' );											// authentication mechanism used for login
	external.globals.Add( 'XMPPSASLSSPIContinue',	true );											// internal flag used by SSPI authentication
	external.globals.Add( 'XMPPSessionID',			'' );											// stream session ID used for login
	external.globals.Add( 'XMPPKeepAliveInterval',	0 );											// keep alive packet interval
	external.globals.Add( 'XMPPPrivacy',			-1 );											// iq:privacy unsupported: -1, default: 0, invisible: 1
	external.globals.Add( 'XMPPReconnectTimeout',	0 );											// reconnect interval
	external.globals.Add( 'XMPPReconnect',			false );										// reconnect flag
	external.globals.Add( 'XMPPStreamVersion',		null );											// server protocol version
	external.globals.Add( 'XMPPSSPIReconnect',		false );										// already reconnecting with SSPI

	/* Load branding data from XML files
	 */
	var brandxml = new ActiveXObject( 'Scripting.Dictionary' );
	if ( brandxml = file2hash( external.globals( 'cwd' ) + '..\\settings\\brand.xml' ) )
	{
		var keys = ( new VBArray( brandxml.Keys() ) ).toArray();
		for ( var i = 0; i < keys.length; i++ )
			if ( external.globals.Exists( keys[i] ) )
				external.globals( keys[i] ) = brandxml( keys[i] );
			else
				external.globals.Add( keys[i], brandxml( keys[i] ) );
	}
	if ( brandxml = file2hash( external.globals( 'cwd' ) + '..\\settings\\override.xml' ) )
	{
		var keys = ( new VBArray( brandxml.Keys() ) ).toArray();
		for ( var i = 0; i < keys.length; i++ )
			if ( external.globals.Exists( keys[i] ) )
				external.globals( keys[i] ) = brandxml( keys[i] );
			else
				external.globals.Add( keys[i], brandxml( keys[i] ) );
	}

	/* Load branding data from registry
	 */
	var SettingNames = [ 'sspiserver', 'trayonly' ];
	for ( var i = 0; i < SettingNames.length; ++i )
	{
		try
		{
			external.globals( SettingNames[i] ) = external.RegRead( 'HKEY_CURRENT_USER', 'Software\\' + external.globals( 'softwarenamesafe' ) + '\\Settings', SettingNames[i] );
		}
		catch ( e )
		{
		}
	}

	/* Where profiles are stored: AppData or MyDocuments
	 */
	if ( external.GetSpecialFolder( 0x001a ).length)
		external.globals.Add( 'usersdir', external.GetSpecialFolder( 0x001a ) );
	else
		external.globals.Add( 'usersdir', external.GetSpecialFolder( 0x0005 ) );
	external.globals( 'usersdir' ) += '\\' + external.globals( 'softwarenamesafe' ) + '\\';
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) ) )
		external.Directory.Create( external.globals( 'usersdir' ) );

	/* Global settings
	 */
	var cfg_temp = new ActiveXObject( 'Scripting.Dictionary' );
	if ( cfg_temp = file2hash( external.globals( 'usersdir') + 'global.xml' ) )
	{
		var keys = ( new VBArray( cfg_temp.Keys() ) ).toArray();
		for ( var i = 0; i < keys.length; i++ )
			if ( external.globals.Exists( keys[i] ) )
				external.globals( keys[i] ) = cfg_temp( keys[i] );
	}

	/* Translate the window
	 */
	external.globals( 'Translator' ).Reload();
	external.globals( 'Translator' ).TranslateWindow( 'main', document );
	document.getElementById( 'txt_welcome' ).innerText = external.globals( 'Translator' ).Translate( 'main', 'txt_welcome', [ external.globals( 'softwarename' ) ] );

	/* Check the time limit
	 */
	if ( external.globals( 'softwareexpiredate' ).match( /^(\d{1,2})-(\d{1,2})-(\d{4})$/ ) && ( new Date() ).getTime() >= ( new Date( RegExp.$3, RegExp.$2 - 1, RegExp.$1 ) ).getTime() )
	{
		external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_expired' ), external.globals( 'softwarename' ), 0 | 48 );
		setTimeout( 'external.wnd.close()', 0 );
		return;
	}

	/* Copy default avatars to profiles directory
	 */
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Avatars' ) )
		external.Directory.Create( external.globals( 'usersdir' ) + 'Avatars' );
	var files = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\avatars' ) ).toArray();
	for ( var i = 0; i < files.length; ++i )
		if ( ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + files[i].Name ) )
			external.File( external.globals( 'cwd' ) + '..\\avatars\\' + files[i].Name ).Copy( external.globals( 'usersdir' ) + 'Avatars\\' + files[i].Name );

	/* Copy default avatars to profiles directory
	 */
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'My Avatars' ) )
		external.Directory.Create( external.globals( 'usersdir' ) + 'My Avatars' );
	var files = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\avatars' ) ).toArray();
	for ( var i = 0; i < files.length; ++i )
		if ( ! external.FileExists( external.globals( 'usersdir' ) + 'My Avatars\\' + files[i].Name ) )
			external.File( external.globals( 'cwd' ) + '..\\avatars\\' + files[i].Name ).Copy( external.globals( 'usersdir' ) + 'My Avatars\\' + files[i].Name );

	/* Copy default plug-ins to profiles directory
	 */
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Plugins' ) )
		external.Directory.Create( external.globals( 'usersdir' ) + 'Plugins' );
	files = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\plugins' ) ).toArray();
	for ( var i = 0; i < files.length; ++i )
		if ( ! external.FileExists( external.globals( 'usersdir' ) + 'Plugins\\' + files[i].Name ) )
			external.File( external.globals( 'cwd' ) + '..\\plugins\\' + files[i].Name ).Copy( external.globals( 'usersdir' ) + 'Plugins\\' + files[i].Name );

	/* Copy default emoticons to profiles directory
	 */
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Emoticons' ) )
		external.Directory.Create( external.globals( 'usersdir' ) + 'Emoticons' );
	files = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\emoticons' ) ).toArray();
	for ( var i = 0; i < files.length; ++i )
		if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Emoticons\\' + files[i].Name ) )
			external.UnZip( external.globals( 'cwd' ) + '..\\emoticons\\' + files[i].Name, external.globals( 'usersdir' ) + 'Emoticons\\' )

	/* Copy default backgrounds to profiles directory
	 */
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Backgrounds' ) )
		external.Directory.Create( external.globals( 'usersdir' ) + 'Backgrounds' );
	files = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\backgrounds' ) ).toArray();
	for ( var i = 0; i < files.length; ++i )
		if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Backgrounds\\' + files[i].Name ) )
			external.UnZip( external.globals( 'cwd' ) + '..\\backgrounds\\' + files[i].Name, external.globals( 'usersdir' ) + 'Backgrounds\\' )

	/* Browsing backgrounds event
	 */
	external.globals( 'BackgroundList' ).onreadystatechange = BackgroundBrowse;

	/* DOM and input events
	 */
	document.onselectstart	= document.ondragstart = function(){return event.srcElement.tagName=='TEXTAREA'||event.srcElement.tagName=='INPUT'};
	document.onkeydown		= handle_keyboard;
	window.onunload			= outnit;
	external.wnd.setActivationHandler( 'OnWindowActivate' );

	/* Handlers for XMPP connection
	 */
	external.XMPP.ConnectedHandler			= 'XMPPOnConnected';
	external.XMPP.DisconnectedHandler		= 'XMPPOnDisconnected';
	external.XMPP.StartTLSSucceededHandler	= 'XMPPOnStartTLSSucceeded';
	external.XMPP.StartTLSFailedHandler		= 'XMPPOnStartTLSFailed';
	external.XMPP.StartSCSucceededHandler = 'XMPPOnStartSCSucceeded';
	external.XMPP.StartSCFailedHandler = 'XMPPOnStartSCSucceeded';
	external.XMPP.DocumentStartHandler		= 'XMPPOnDocumentStart';
	external.XMPP.DocumentEndHandler		= 'XMPPOnDocumentEnd';
	external.XMPP.IQHandler					= 'XMPPOnIQ';
	external.XMPP.MessageHandler			= 'XMPPOnMessage';
	external.XMPP.PresenceHandler			= 'XMPPOnPresence';
	external.XMPP.StreamHandler				= 'XMPPOnStream';

	/* Create the roster tab button
	 */
	external.globals( 'ClientPluginContainer' ).Plugins.Add( '/roster', new ClientPluginCore( external.globals( 'ClientPluginContainer' ) ) );
	with ( external.globals( 'ClientPluginContainer' ).Plugins( '/roster' ).ClientPluginTab = new ClientPluginTab( external.globals( 'ClientPluginContainer' ).Plugins( '/roster' ) ) )
	{
		Icon		= external.globals( 'cwd' ) + '..\\images\\brand\\default.ico';
		Tooltip		= external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip', [ external.globals('softwarename' ) ] );
		IsActive	= true;
		HTMLArea	= document.getElementById( 'rosterback' );
		DrawButton();
	}

	/* Brand the window
	 */
	external.wnd.setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
	if ( external.FileExists( external.globals( 'cwd' ) + '..\\images\\brand\\tray.ico' ) )
		external.notifyIcon.setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\tray.ico', 0 );
	else
		external.notifyIcon.setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico', 0 );
	external.notifyIcon.setText( external.globals( 'softwarename' ) );
	external.notifyIcon.show();

	/* Load plugins
	 */
	external.globals( 'ClientPluginContainer' ).Reload();

	/* Startup event
	 */
	CommandLine( external.CmdLine );
	external.wnd.setCmdLineHandler( 'CommandLine' );

	/* Hookup the window events
	 */
	external.notifyIcon.setHandler( 'TrayCommotion' );
	external.wnd.setCloseHandler( 'TrayOnClose' );
	external.wnd.setMinHandler( 'TrayOnMinimize' );

	/* Connect to the HTTPEngine
	 */
	external.HTTPEngine.subscribe( external.wnd );

	/* Menubar event
	 */
	external.wnd.setMenuHandler( 'MenuBarSelect' );

	/* Prepare banner area
	 */
	if ( external.globals( 'bannerhalf' ).length )
		BannerLoad( 234, 60, external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'bannerhalf' ) ) );

	/* Use SSPI to login
	 */
	if ( external.globals( 'sspiserver' ).length )
	{
		var Address = new XMPPAddress( external.globals( 'sspiserver' ) );
		login( Address, '' );
	}
	/* Automatically login (normal)
	 */
	else if ( ( external.globals( 'autologin' ).toString() == 'true' || external.CmdLine.substr( external.CmdLine.length - 10 ) == '/minimized' )
		&& external.globals( 'authentication' ) == 'normal' && external.globals( 'last_address' ).match( /@/ ) && external.globals( 'last_password' ).length )
	{
		var Address		= new XMPPAddress( external.globals( 'last_address' ) );
		var Password	= pass_code( false, external.globals( 'last_password' ) );
		login( Address, Password );
	}
	/* Automatically login (ntlm)
	 */
	else if ( ( external.globals( 'autologin' ).toString() == 'true' || external.CmdLine.substr( external.CmdLine.length - 10 ) == '/minimized' )
		&& external.globals( 'authentication' ) == 'ntlm' && external.globals( 'last_address' ).match( /^[^@]+$/ ) )
	{
		var Address = new XMPPAddress( external.globals( 'last_address' ) );
		login( Address, '' );
	}
	/* Prepare the login window
	 */
	else
		dial_login( external.CmdLine.length < 10 || external.CmdLine.substr( external.CmdLine.length - 10 ) != '/minimized' );
}
