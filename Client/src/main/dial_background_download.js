function dial_background_download ()
{
	if ( external.windows.Exists( 'background_download' ) )
	{
		if ( external.windows( 'background_download' ).isMinimized )
			external.windows( 'background_download' ).restore();
		external.windows( 'background_download' ).focus();
	}
	else
		with ( external.createWindow( 'background_download', external.globals( 'cwd' ) + 'background_download.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_background_download' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\backgrounds\\theme.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
