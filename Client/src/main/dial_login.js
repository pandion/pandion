function dial_login ( Visible )
{
	if ( typeof Visible != 'boolean' )
		Visible = true;

	external.wnd.hide( true );

	if ( external.windows.Exists( 'login' ) )
		external.windows( 'login' ).focus();
	else
		with ( external.createWindow( 'login', external.globals( 'cwd' ) + 'login.html', [ window, Visible ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_login', [ external.globals( 'softwarename' ) ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			showMinBox( true );
			setSize( 400, 260 );
			setPos( ( screen.availWidth - 400 ) / 2, ( screen.availHeight - 260 ) / 2 );
		}
}
