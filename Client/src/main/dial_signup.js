function dial_signup ()
{
	if ( external.windows.Exists( 'signup' ) )
	{
		if ( external.windows( 'signup' ).isMinimized )
			external.windows( 'signup' ).restore();
		external.windows( 'signup' ).focus();
	}
	else if ( external.globals( 'websignup' ).length )
		dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'websignup' ) ) );
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
