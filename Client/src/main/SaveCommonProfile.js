function SaveCommonProfile ()
{
	var Data = new ActiveXObject( 'Scripting.Dictionary' );
	Data.Add( 'aot', external.globals( 'aot' ) );
	Data.Add( 'authentication', external.globals( 'authentication' ) );
	Data.Add( 'autologin', external.globals( 'autologin' ) );
	Data.Add( 'encryption', external.globals( 'encryption' ) );
	Data.Add( 'installupdates', external.globals( 'installupdates' ) );
	Data.Add( 'language', external.globals( 'language' ) );
	Data.Add( 'last_address', external.globals( 'last_address' ) );
	Data.Add( 'last_autoupdate', external.globals( 'last_autoupdate' ) );
	Data.Add( 'last_password', external.globals( 'last_password_remember' ).toString() != 'true' ? '' : external.globals( 'last_password' ) );
	Data.Add( 'last_password_remember', external.globals( 'last_password_remember' ) );
	Data.Add( 'serveraddress', external.globals( 'serveraddress' ) );
	Data.Add( 'serverport', external.globals( 'serverport' ) );
	Data.Add( 'softwaretrack', external.globals( 'softwaretrack' ) );
	Data.Add( 'welcomescreen', external.globals( 'welcomescreen' ) );
	Data.Add( 'welcomesettings', external.globals( 'welcomesettings' ) );

	// Querying position/size requires the main dialog to be visible.
	external.wnd.restore();

	if ( external.IsRectOnMonitor(
			external.wnd.top,
			external.wnd.left + external.globals( 'lastwidth' ),
			external.wnd.top + external.globals( 'lastheight' ),
			external.wnd.left )
		&& Math.abs( external.wnd.left ) !== 32000
		&& Math.abs( external.wnd.top ) !== 32000
	)
	{
		Data.Add( 'lastposx', external.wnd.left );
		Data.Add( 'lastposy', external.wnd.top );
	}
	else
	{
		Data.Add( 'lastposx', external.globals( 'lastposx' ) );
		Data.Add( 'lastposy', external.globals( 'lastposy' ) );
	}

	Data.Add( 'lastwidth', external.globals( 'lastwidth' ) );
	Data.Add( 'lastheight', external.globals( 'lastheight' ) );
	if ( document.getElementById( 'content-dialog' ).style.display == 'block' )
	{
		if ( external.wnd.width > 50 )
			Data( 'lastwidth' ) = external.globals( 'lastwidth' ) = external.wnd.width;
		if ( external.wnd.height > 50 )
			Data( 'lastheight' ) = external.globals( 'lastheight' ) = external.wnd.height;
	}

	hash2file( Data, external.globals( 'usersdir' ) + 'global.xml' );
}
