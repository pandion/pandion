function dial_transport_list ()
{
	if ( external.windows.Exists( 'transport_list' ) )
		external.windows( 'transport_list' ).focus();
	else
		with ( external.createWindow( 'transport_list', external.globals( 'cwd' ) + 'transport_list.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_transport_list' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\bulb.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}
