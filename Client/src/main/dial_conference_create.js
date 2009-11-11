function dial_conference_create ( inviteJid, inviteGroup )
{
	inviteJid = inviteJid || '';
	inviteGroup = inviteGroup || '';
	if ( external.windows.Exists( 'conference_create' ) )
	{
		if ( external.windows( 'conference_create' ).isMinimized )
			external.windows( 'conference_create' ).restore();
		external.windows( 'conference_create' ).focus();
	}
	else
		with ( external.createWindow( 'conference_create', external.globals( 'cwd' ) + 'conference_create.html', [ window, inviteJid, inviteGroup ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_room_create' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 450, 300 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 300 ) / 2 );
		}
}
