function dial_conference_invite ( Tracker, Contact, Group )
{
	var ShortAddress = Tracker.Address.ShortAddress();
	if ( external.windows.Exists( 'conference_invite/' + ShortAddress ) )
		external.windows( 'conference_invite/' + ShortAddress ).focus();
	else
		with ( external.createWindow( 'conference_invite/' + ShortAddress, external.globals( 'cwd' ) + 'conference-invite.html', [ window, Tracker, Contact, Group ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_conference_invite' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 300, 450 );
			setPos( ( screen.availWidth - 300 ) / 2, ( screen.availHeight - 450 ) / 2 );
		}
}
