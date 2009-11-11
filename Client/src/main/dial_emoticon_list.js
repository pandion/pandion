function dial_emoticon_list ()
{
	if ( external.windows.Exists( 'emoticon_list' ) )
	{
		if ( external.windows( 'emoticon_list' ).isMinimized )
			external.windows( 'emoticon_list' ).restore();
		external.windows( 'emoticon_list' ).focus();
	}
	else
		with ( external.createWindow( 'emoticon_list', external.globals( 'cwd' ) + 'emoticon_list.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_emoticon_list' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\emoticons\\emoticon.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
