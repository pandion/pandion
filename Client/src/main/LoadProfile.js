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
	if ( ! external.globals( 'cfg' )( 'avatar' ).length )
	{
		var	Avatars = new VBArray( external.Directory.ListFiles( external.globals( 'cwd' ) + '..\\avatars' ) ).toArray();
		var Hash = parseInt( external.StringToSHA1( Address.ShortAddress() ).substr( 0, 8 ), 16 );
		external.globals( 'cfg' )( 'avatar' ) = Avatars[ Hash % Avatars.length ].Name;
	}

	/* Copy avatar from cache to personal repository
	 */
	if ( ! external.FileExists( external.globals( 'usersdir' ) + 'My Avatars\\' + external.globals( 'cfg' )( 'avatar' ) )
		&& external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + external.globals( 'cfg' )( 'avatar' ) ) )
	{
		external.File( external.globals( 'usersdir' ) + 'Avatars\\' + external.globals( 'cfg' )( 'avatar' ) ).Copy( external.globals( 'usersdir' ) + 'My Avatars\\' + external.globals( 'cfg' )( 'avatar' ) );
	}

	/* Default directory where filetransfers are stored
	 */
	if ( ! cfg( 'downloaddir' ).length )
		cfg( 'downloaddir' ) = external.GetSpecialFolder( 0x0005 ) + '\\';

	/* Default translated auto-away messages
	 */
	if ( ! cfg( 'autoawaytext' ).length )
		cfg( 'autoawaytext' ) = external.globals( 'Translator' ).Translate( 'main', 'autoawaymessage' );
	if ( ! cfg( 'autoxawaytext' ).length )
		cfg( 'autoxawaytext' ) = external.globals( 'Translator' ).Translate( 'main', 'autoxawaymessage' );

	/* List of addresses that are blocked
	 */
	cfg( 'blocklist' ) = cfg( 'blocklist' ).split( ',' );
	for ( var i = 0; i < cfg( 'blocklist' ).length; i++ )
		if ( cfg( 'blocklist' )[i].length && ! external.globals( 'block' ).Exists( cfg( 'blocklist' )[i] ) )
			external.globals( 'block' ).Add( cfg( 'blocklist' )[i], '' );

	/* Restore the user's preferred window dimensions and location
	 */
	MenuBarUpdate();
	if ( external.wnd.isMinimized )
		external.wnd.restore();

	/* Display the window
	 */
	external.wnd.setTitle( Address.ShortAddress() + ' - ' + external.globals( 'softwarename' ) );
	external.wnd.hide( false );
	external.wnd.focus();
}
