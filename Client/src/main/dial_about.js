function dial_about ()
{
	if ( external.windows.Exists( 'about' ) )
	{
		if ( external.windows( 'about' ).isMinimized )
			external.windows( 'about' ).restore();
		external.windows( 'about' ).focus();
	}
	else
		with ( external.createWindow( 'about', external.globals( 'cwd' ) + 'about.html', 0 ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_about', [ external.globals( 'softwarename' ) ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			setSize( 540, 266 );
			setPos( ( screen.availWidth - 540 ) / 2, ( screen.availHeight - 266 ) / 2 );
		}

}
