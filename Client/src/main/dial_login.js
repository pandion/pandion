function dial_login ( visible )
{
	if ( typeof visible != 'boolean' )
		visible = true;

	/* Branding
	 */
	external.wnd.setTitle( external.globals( 'softwarename' ) );
	document.getElementById( 'signin-register' ).href = external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'websignup' ) );

	/* Reset user profile
	 */
	external.globals( 'cfg' )( 'username' ) = external.globals( 'cfg' )( 'server' ) = '';

	/* Show sign in dialog
	 */
	document.getElementById( 'content-dialog' ).style.display = 'none';
	document.getElementById( 'signin-dialog' ).style.display = 'block';
	MenuBarUpdate();
	external.wnd.setAOT( false );
	external.wnd.MinWidth = 0;
	external.wnd.MinHeight = 0;
	if ( visible )
	{
		external.wnd.hide( false );
		external.wnd.restore();
		external.wnd.focus();
	}
	external.wnd.setSize( 350, 600 );
	external.wnd.setPos( ( screen.availWidth - 350 ) / 2, ( screen.availHeight - 600 ) / 2 );

	/* Flush the list
	 */
	while ( document.getElementById( 'signin-recently-used' ).options.length )
		document.getElementById( 'signin-recently-used' ).options.remove(0);

	/* Show last used address and password
	 */
	document.getElementById( 'signin-address' ).focus();
	document.getElementById( 'signin-address' ).focus();
	document.getElementById( 'signin-address' ).value = '';
	document.getElementById( 'signin-password' ).value = '';
	document.getElementById( 'signin-remember' ).checked = false;
	document.getElementById( 'signin-autoconnect' ).checked = false;
	if ( external.globals( 'last_address' ).length && external.globals( 'last_address' ).match( /@/ ) )
	{
		document.getElementById( 'signin-address' ).value = external.globals( 'last_address' );
		document.getElementById( 'signin-password' ).focus();
		document.getElementById( 'signin-password' ).focus();
		if ( external.globals( 'last_password_remember' ).toString() == 'true' )
		{
			document.getElementById( 'signin-remember' ).checked = true;
			document.getElementById( 'signin-autoconnect' ).checked = external.globals( 'autologin' ).toString() == 'true';
			document.getElementById( 'signin-password' ).value = pass_code( false, external.globals( 'last_password' ) );
		}
	}

	/* Append the addresses to the pulldown
	 */
	var Addresses = dial_login_loadAccounts( /@/ );
	for ( var i = 0; i < Math.min( 6, Addresses.length ); ++i )
	{
		var opt = document.createElement( 'OPTION' );
		var Address = external.globals( 'fixedserver' ).length ? Addresses[i].Name.substr( 0, Addresses[i].Name.indexOf( '@' ) + 1 ) + external.globals( 'fixedserver' ) : Addresses[i].Name;
		opt.text = opt.value = Address;
		document.getElementById( 'signin-recently-used' ).add( opt );
	}
}

function dial_login_loadAccounts ( ValidAccount )
{
	/* Build list of recently used accounts
	 */
	var CurrentTime = ( new Date() ).getTime();
	var Addresses = [];
	var Directories = new VBArray( external.Directory.ListDirs( external.globals( 'usersdir' ) + 'Profiles\\' ) ).toArray();
	for ( var i = 0; i < Directories.length; ++i )
	{
		if ( Directories[i].Name != external.globals( 'last_address' ) && Directories[i].Name.match( ValidAccount ) )
		{
			var Path = external.globals( 'usersdir' ) + 'Profiles\\' + Directories[i].Name + '\\settings.xml';
			var LastWriteTime = external.FileExists( Path ) ? ( new Date( external.file( Path ).LastWriteTime ) ).getTime() : -1;
			if ( LastWriteTime > CurrentTime - ( 30 * 24 * 3600 * 1000 ) || LastWriteTime == -1 )
				Addresses.push({
					LastWriteTime: LastWriteTime,
					Name: Directories[i].Name
				});
		}
	}

	/* First entry should be the last used address
	 */
	if ( external.globals( 'last_address' ).length && external.globals( 'last_address' ).match( ValidAccount ) )
		Addresses.push({
			LastWriteTime: Number.MAX_VALUE,
			Name: external.globals( 'last_address' )
		});

	/* Order according to last used date
	 */
	Addresses.sort(
		function ( a, b )
		{
			if ( a.LastWriteTime > b.LastWriteTime )
				return -1;
			else if ( a.LastWriteTime == b.LastWriteTime )
				return 0;
			else
				return 1;
		}
	);

	return Addresses;
}

function dial_login_changeRemember ()
{
	var Checked = document.getElementById( 'signin-remember' ).checked;
	document.getElementById( 'txt-signin-autoconnect' ).disabled = ! Checked;
	document.getElementById( 'signin-autoconnect' ).disabled = ! Checked;
}

function dial_login_connect ()
{
	var Address = new XMPPAddress( document.getElementById( 'signin-address' ).value );
	var Password = document.getElementById( 'signin-password' ).value;

	if ( external.globals( 'fixedserver' ).length )
	{
		if ( ! Address.User.length )
			Address.User = Address.Host;
		Address.Host = external.globals( 'fixedserver' );
	}
	else if ( ! Address.User.length )
	{
		Address.User = Address.Host;

		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.async = false;
		dom.resolveExternals = false;
		dom.load( external.globals( 'cwd' ) + '..\\settings\\servers.xml' );
		var DefaultServer = dom.selectSingleNode( '/servers/xmpp[@default="yes"]' );
		if ( DefaultServer )
			Address.Host = DefaultServer.text;
	}

	if ( Address.User.length && Address.Host.length )
	{
		external.globals( 'last_password_remember' ) = document.getElementById( 'signin-remember' ).checked;
		external.globals( 'autologin' ) = document.getElementById( 'signin-autoconnect' ).checked;
		login( Address, Password );
	}
	else
		external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'login', 'invalid' ), external.globals( 'softwarename' ), 0 | 48 );
}
