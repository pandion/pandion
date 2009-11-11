function dial_preferences ( section )
{
	if ( external.windows.Exists( 'preferences' ) )
	{
		external.windows( 'preferences' ).Do( 'tree_goto', section );
		if ( external.windows( 'preferences' ).isMinimized )
			external.windows( 'preferences' ).restore();
		external.windows( 'preferences' ).focus();
	}
	else
		with ( external.createWindow( 'preferences', external.globals( 'cwd' ) + 'preferences.html', new Array( window, section ) ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_settings' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\preferences\\tools.ico' );
			resizeable( false );
			showMinBox( true );
			setSize( 490, 320 );
			setPos( ( screen.availWidth - 490 ) / 2, ( screen.availHeight - 320 ) / 2 );
		}
}
