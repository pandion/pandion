function dial_file ( Address )
{
	if ( typeof Address == 'string' )
		Address = new XMPPAddress( Address );

	var ShortAddress = Address.ShortAddress();

	var id = 'sd' + ( ++external.globals( 'uniqueid' ) );
	with ( external.createWindow( ShortAddress + '/' + id + '/file_send', external.globals( 'cwd' ) + 'file_send.html', new Array( ShortAddress, Address.Resource, id ) ) )
	{
		setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_file_send', [ external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) ? external.globals( 'ClientRoster' ).Items( ShortAddress ).Name : ShortAddress ] ) );
		setIcon( external.globals( 'cwd' ) + '..\\images\\filetransfer\\disk.ico' );
		resizeable( false );
		showMinBox( true );
		MinHeight = MinWidth = 0;
		setSize( 350, 190 );
		setPos( ( screen.availWidth - 350 ) / 2, ( screen.availHeight - 190 ) / 2 );
	}
}
