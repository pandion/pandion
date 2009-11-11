function dial_bookmarks_add ( Address, Password, AutoJoin, Edit )
{
	if ( ! Address )
		Address = '';
	else if ( typeof Address == 'string' )
		Address = ( new XMPPAddress( Address ) ).LongAddress();
	else
		Address = Address.LongAddress();

	if ( external.windows.Exists( 'bookmarks_add/' + Address ) )
	{
		if ( external.windows( 'bookmarks_add/' + Address ).isMinimized )
			external.windows( 'bookmarks_add/' + Address ).restore();
		external.windows( 'bookmarks_add/' + Address ).focus();
	}
	else
		with ( external.createWindow( 'bookmarks_add/' + Address, external.globals( 'cwd' ) + 'bookmarks_add.html', new Array( window, Address, Password, AutoJoin, Edit ) ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', Edit ? 'wnd_bookmarks_edit' : 'wnd_bookmarks_add' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 340, 200 );
			setPos( ( screen.availWidth - 340 ) / 2, ( screen.availHeight - 200 ) / 2 );
		}
}
