function dial_login ( visible )
{
	if ( typeof visible != 'boolean' )
		visible = true;

	/* Branding
	 */
	external.wnd.setTitle( external.globals( 'softwarename' ) );

	/* Reset user profile
	 */
	if ( external.globals( 'cfg' ) )
		external.globals( 'cfg' )( 'username' ) = external.globals( 'cfg' )( 'server' ) = '';

	/* Toggle form fields
	 */
	document.getElementById( 'txt-signin-password' ).style.display =
		document.getElementById( 'signin-password' ).style.display =
		document.getElementById( 'signin-remember' ).style.display =
		document.getElementById( 'txt-signin-remember' ).style.display =
		document.getElementById( 'signin-example-user' ).style.display =
		external.globals( 'authentication' ) == 'normal' ? '' : 'none';
	document.getElementById( 'signin-example-sso' ).style.display =
		external.globals( 'authentication' ) == 'normal' ? 'none' : '';

	/* Show sign in dialog
	 */
	document.getElementById( 'content-dialog' ).style.display = 'none';
	document.getElementById( 'signin-dialog' ).style.display = 'block';
	external.wnd.MinWidth = 330;
	external.wnd.MinHeight = 530;
	external.wnd.setSize( Math.max( external.globals( 'lastwidth' ), 330 ), Math.max( external.globals( 'lastheight' ), 530 ) );
	MenuBarUpdate();
	if ( visible )
	{
		external.wnd.hide( false );
		external.wnd.restore();
		external.wnd.focus();
	}

	/* Flush the list
	 */
	while ( document.getElementById( 'signin-recently-used' ).options.length )
		document.getElementById( 'signin-recently-used' ).options.remove(0);

	/* If no previous account exists (eg. first run) then try to extract last used account from GTalk
	 */
	if ( external.globals( 'last_address' ).length == 0 && external.globals( 'authentication' ) == 'normal' )
	{
		try {
			external.globals( 'last_address' ) = external.RegRead( 'HKEY_CURRENT_USER', 'Software\\Google\\Google Talk\\Accounts', 'a' ).split( '/' )[0];
		} catch ( error ) {
		}
	}

	/* Show last used address and password
	 */
	document.getElementById( 'signin-address' ).focus();
	document.getElementById( 'signin-address' ).focus();
	document.getElementById( 'signin-address' ).value = external.globals( 'last_address' );
	document.getElementById( 'signin-remember' ).checked = external.globals( 'last_password_remember' ).toString() == 'true';
	document.getElementById( 'signin-autoconnect' ).checked = external.globals( 'autologin' ).toString() == 'true';
	if ( external.globals( 'last_address' ).length && external.globals( 'authentication' ) == 'normal' )
	{
		document.getElementById( 'signin-password' ).focus();
		document.getElementById( 'signin-password' ).focus();
	}
	document.getElementById( 'signin-password' ).value = pass_code( false, external.globals( 'last_password' ) );

	/* Append the addresses to the pulldown
	 */
	var Addresses = dial_login_loadAccounts( /@/ );
	for ( var i = 0; i < Math.min( 6, Addresses.length ); ++i )
	{
		var opt = document.createElement( 'OPTION' );
		opt.text = opt.value = Addresses[i].Name;
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

	if ( external.globals( 'fixedserver' ).length )
	{
		if ( ! Address.User.length )
			Address.User = Address.Host;
		Address.Host = external.globals( 'fixedserver' );
	}

	/* Standard authentication
	 */
	if ( external.globals( 'authentication' ) == 'normal' )
	{
		if ( Address.User.length && Address.Host.length )
		{
			external.globals( 'last_password_remember' ) = document.getElementById( 'signin-remember' ).checked;
			external.globals( 'autologin' ) = document.getElementById( 'signin-autoconnect' ).checked;
			login( Address, document.getElementById( 'signin-password' ).value );
		}
		else
			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'invalid-user-address' ), external.globals( 'softwarename' ), 0 | 48 );
	}
	/* Windows Integrated Authentication
	 */
	else
	{
		if ( Address.Host.length )
		{
			Address.Parse( Address.Host );
			external.globals( 'autologin' ) = document.getElementById( 'signin-autoconnect' ).checked;
			login( Address, '' );
		}
		else
			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'invalid-sso-server' ), external.globals( 'softwarename' ), 0 | 48 );
	}
}
