function dial_plugins ()
{
	if ( external.windows.Exists( 'plugins' ) )
		external.windows( 'plugins' ).focus();
	else
		with ( external.createWindow( 'plugins', external.globals( 'cwd' ) + 'plugins.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_plugins' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\plug.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
