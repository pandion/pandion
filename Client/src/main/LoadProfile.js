function LoadProfile ( Address )
{
	/* Make sure the profile directory exists
	 */
	if ( ! external.Directory.Exists( external.globals( 'usersdir' ) + 'Profiles\\' + Address.ShortAddress() ) )
		external.Directory.Create( external.globals( 'usersdir' ) + 'Profiles\\' + Address.ShortAddress() );

	/* Load the default profile data, and overwrite it with the custom profile data it if exists
	 */
	external.globals( 'settingsfile' ) = external.globals( 'usersdir' ) + 'Profiles\\' + Address.ShortAddress() + '\\settings.xml';
	external.globals( 'cfg' ) = file2hash( external.globals( 'cwd' ) + '..\\settings\\default.xml' );
	var cfg_custom = file2hash( external.globals( 'settingsfile' ) );
	var cfg = external.globals( 'cfg' );

	if ( cfg_custom )
	{
		var keys = ( new VBArray( cfg_custom.Keys() ) ).toArray();
		for ( var i = 0; i < keys.length; i++ )
			if ( cfg.Exists( keys[i] ) )
				cfg( keys[i] ) = cfg_custom( keys[i] );
	}

	/* Select one of the default avatars if necessary
	 */
	if ( ! external.globals( 'cfg' ).Item( 'avatar' ).length )
	{
		var	Avatars = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\avatars' ) ).toArray();
		var Hash = parseInt( external.StringToSHA1( Address.ShortAddress() ).substr( 0, 8 ), 16 );
		external.globals( 'cfg' ).Item( 'avatar' ) = Avatars[ Hash % Avatars.length ].Name;
	}

	/* Copy avatar from cache to personal repository
	 */
	if ( ! external.FileExists( external.globals( 'usersdir' ) + 'My Avatars\\' + external.globals( 'cfg' ).Item( 'avatar' ) )
		&& external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + external.globals( 'cfg' ).Item( 'avatar' ) ) )
	{
		external.File( external.globals( 'usersdir' ) + 'Avatars\\' + external.globals( 'cfg' ).Item( 'avatar' ) ).Copy( external.globals( 'usersdir' ) + 'My Avatars\\' + external.globals( 'cfg' ).Item( 'avatar' ) );
	}

	/* Default directory where filetransfers are stored
	 */
	if ( ! cfg( 'downloaddir' ).length )
		cfg( 'downloaddir' ) = external.GetSpecialFolder( 0x0005 ) + '\\';

	/* List of addresses that are blocked
	 */
	cfg( 'blocklist' ) = cfg( 'blocklist' ).split( ',' );
	for ( var i = 0; i < cfg( 'blocklist' ).length; i++ )
		if ( cfg( 'blocklist' )[i].length && ! external.globals( 'block' ).Exists( cfg( 'blocklist' )[i] ) )
			external.globals( 'block' ).Add( cfg( 'blocklist' )[i], '' );

	/* Don't restore as idle, use available instead
	 */
	if ( parseInt( cfg( 'lastmode' ), 10 ) === 6 )
		cfg( 'lastmode' ) = 0;

	/* Display the window
	 */
	MenuBarUpdate();
	external.wnd.setTitle( Address.ShortAddress() + ' - ' + external.globals( 'softwarename' ) );
	if ( external.CmdLine.substr( external.CmdLine.length - 10 ) !== '/minimized' ) {
		if ( external.wnd.isMinimized )
			external.wnd.restore();
		external.wnd.hide( false );
		external.wnd.focus();
	}

	/* Set Login Address to global variable
	*/
	external.globals( 'LoginAddress' ) = Address;

	/* Set the path of Open Trackers
	*/
	external.globals( 'OpenTrackersFile' ) = external.globals( 'usersdir' ) + 'Profiles\\' + Address.ShortAddress() + '\\opentrackers.xml';
}
