function dial_plugin_list ()
{
	if ( external.windows.Exists( 'plugin_list' ) )
		external.windows( 'plugin_list' ).focus();
	else
		with ( external.createWindow( 'plugin_list', external.globals( 'cwd' ) + 'plugin_list.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_plugin_list' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\plug-ins\\tiles.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
