function dial_emoticon_viewer ( StyleName )
{
	if ( external.windows.Exists( StyleName + '/emoset_view' ) )
	{
		if ( external.windows( StyleName + '/emoset_view' ).isMinimized )
			external.windows( StyleName + '/emoset_view' ).restore();
		external.windows( StyleName + '/emoset_view' ).focus();
	}
	else
		with ( external.createWindow( StyleName + '/emoset_view', external.globals( 'cwd' ) + 'emoset_view.html', [ window, StyleName ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_emoticon_view' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\emoticons\\emoticon.ico' );
			MinWidth = 240;
			MinHeight = 200;
			setSize( 385, 382 );
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
