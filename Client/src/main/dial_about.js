function dial_about ()
{
	if ( external.windows.Exists( 'about' ) )
		external.windows( 'about' ).focus();
	else
		with ( external.createWindow( 'about', external.globals( 'cwd' ) + 'about.html', 0 ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_about', [ external.globals( 'softwarename' ) ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			setSize( 516, 266 );
			setPos( ( screen.availWidth - 516 ) / 2, ( screen.availHeight - 266 ) / 2 );
		}
}
