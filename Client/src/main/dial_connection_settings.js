function dial_connection_settings ()
{
	if ( external.windows.Exists( 'connection_settings' ) )
	{
		if ( external.windows( 'connection_settings' ).isMinimized )
			external.windows( 'connection_settings' ).restore();
		external.windows( 'connection_settings' ).focus();
	}
	else
		with ( external.createWindow( 'connection_settings', external.globals( 'cwd' ) + 'connection_settings.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_connection_settings' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			setSize( 375, 330 );
			setPos( ( screen.availWidth - 375 ) / 2, ( screen.availHeight - 330 ) / 2 );
		}
}
