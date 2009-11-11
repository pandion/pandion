function dial_emoticon_download ()
{
	if ( external.windows.Exists( 'emoticon_download' ) )
	{
		if ( external.windows( 'emoticon_download' ).isMinimized )
			external.windows( 'emoticon_download' ).restore();
		external.windows( 'emoticon_download' ).focus();
	}
	else
		with ( external.createWindow( 'emoticon_download', external.globals( 'cwd' ) + 'emoticon_download.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_emoticon_download' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\emoticons\\emoticon.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
