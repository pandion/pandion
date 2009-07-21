function dial_server_admin ()
{
	if ( external.windows.Exists( 'admin' ) )
		external.windows( 'admin' ).focus();
	else
		with ( external.createWindow( 'admin', external.globals( 'cwd' ) + 'admin.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_admin_server' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\webserver.ico' );
			resizeable( false );
			MinHeight = 382;
			MinWidth = 385;
			setSize( 385, 382 );
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
