function dial_conference_create ()
{
	if ( external.windows.Exists( 'conference_create' ) )
		return external.windows( 'conference_create' ).focus();
	else
		with ( external.createWindow( 'conference_create', external.globals( 'cwd' ) + 'conference_create.html', new Array( window ) ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_room_create' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 450, 300 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 300 ) / 2 );
		}
}
