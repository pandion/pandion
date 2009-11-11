function dial_background_list ()
{
	if ( external.windows.Exists( 'background_list' ) )
	{
		if ( external.windows( 'background_list' ).isMinimized )
			external.windows( 'background_list' ).restore();
		external.windows( 'background_list' ).focus();
	}
	else
		with ( external.createWindow( 'background_list', external.globals( 'cwd' ) + 'background_list.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_background_list' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\backgrounds\\theme.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
