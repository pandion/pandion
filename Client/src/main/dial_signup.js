function dial_signup ()
{
	if ( external.windows.Exists( 'signup' ) )
		external.windows( 'signup' ).focus();
	else if ( external.globals( 'websignup' ).length )
		with ( external.createWindow( 'signup', external.globals( 'cwd' ) + 'signup-web.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_signup', [ external.globals( 'softwarename' ) ] ) );
			resizeable( false );
			showMinBox( true );
			setSize( 500, 365 );
			setPos( ( screen.availWidth - 500 ) / 2, ( screen.availHeight - 365 ) / 2 );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
		}
	else
		with ( external.createWindow( 'signup', external.globals( 'cwd' ) + 'signup.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_signup', [ external.globals( 'softwarename' ) ] ) );
			resizeable( false );
			showMinBox( true );
			setSize( 500, 365 );
			setPos( ( screen.availWidth - 500 ) / 2, ( screen.availHeight - 365 ) / 2 );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
		}
}
