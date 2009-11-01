function dial_headlines ()
{
	if ( external.windows.Exists( 'headline_log' ) )
		external.windows( 'headline_log' ).focus();
	else
		with ( external.createWindow( 'headline_log', external.globals( 'cwd' ) + 'headline_log.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_headlines' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\headlines\\feed.ico' );
			MinHeight = 382;
			MinWidth = 385;
			setSize( 385, 382 );
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
