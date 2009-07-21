function dial_password_change ()
{
	if ( external.windows.Exists( 'pass_change' ) )
		external.windows( 'pass_change' ).focus();
	else
		with ( external.createWindow( 'pass_change', external.globals( 'cwd' ) + 'pass_change.html', 0 ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_password' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\lock.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 400, 110 );
			setPos( ( screen.availWidth - 400 ) / 2, ( screen.availHeight - 110 ) / 2 );
		}
}
