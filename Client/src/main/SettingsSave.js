function SettingsSave ()
{
	var cfg = external.globals( 'cfg' );
	if ( cfg( 'username' ).length && cfg( 'server' ).length )
	{
		/* Do not save the password
		 */
		var Password = cfg( 'password' );
		cfg( 'password' ) = '';

		/* Window position and dimensions
		 */
		if ( external.wnd.left > 0 - external.wnd.width / 2 )
			cfg( 'lastposx' )	= external.wnd.left;
		if ( external.wnd.top > 0 - 20 )
			cfg( 'lastposy' )	= external.wnd.top;
		if ( external.wnd.width > 100 )
			cfg( 'lastwidth' )	= external.wnd.width;
		if ( external.wnd.height > 100 )
			cfg( 'lastheight' )	= external.wnd.height;

		/* Contact list display
		 */
		if ( external.globals( 'ClientRoster' ).Groups.Count )
		{
			var hiddengroups		= new Array();
			var hiddengroupsoffline	= new Array();
			var groupnames			= ( new VBArray( external.globals( 'ClientRoster' ).Groups.Keys() ) ).toArray();
			for ( var i = 0; i < groupnames.length; ++i )
			{
				if ( ! external.globals( 'ClientRoster' ).Groups( groupnames[i] ).ShowAll )
					hiddengroups.push( groupnames[i] );
				if ( external.globals( 'ClientRoster' ).Groups( groupnames[i] ).ShowOffline )
					hiddengroupsoffline.push( groupnames[i] );
			}
			cfg( 'hiddengroups' )			= hiddengroups.join( '\n' );
			cfg( 'hiddengroupsoffline' )	= hiddengroupsoffline.join( '\n' );
		}

		/* Write settings to disk
		 */
		hash2file( cfg, external.globals( 'settingsfile' ) );

		/* Continue using current password
		 */
		cfg( 'password' ) = Password;
	}
}
