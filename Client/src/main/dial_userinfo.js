function dial_userinfo ( Address )
{
	if ( typeof Address == 'string' )
		Address = new XMPPAddress( Address );

	var ShortAddress = Address.ShortAddress();

	var Name = ShortAddress;
	if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		Name = external.globals( 'ClientRoster' ).Items( ShortAddress ).Name;
	else if ( ShortAddress == external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) )
		Name = external.globals( 'cfg' )( 'nick' );

	if ( external.windows.Exists( ShortAddress + '/' + Address.Resource + '/vcard' ) )
		external.windows( ShortAddress + '/' + Address.Resource + '/vcard' ).focus();
	else
		with ( external.createWindow( ShortAddress + '/' + Address.Resource + '/vcard', external.globals( 'cwd' ) + 'vcard.html', new Array( ShortAddress, Address.Resource ) ) )
		{
			setTitle( Name + ' - ' + external.globals( 'Translator' ).Translate( 'main', 'wnd_vcard_view' ) );
			resizeable( false );
			showMinBox( true );
			setIcon( external.globals( 'cwd' ) + '..\\images\\vcard\\card.ico' );
			setSize( 490, 350 );
			setPos( ( screen.availWidth - 490 ) / 2, ( screen.availHeight - 350 ) / 2 );
		}
}
