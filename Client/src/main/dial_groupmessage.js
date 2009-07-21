function dial_groupmessage ( name )
{
	if ( external.windows.Exists( name + '/groupmessage' ) )
		return external.windows( name + '/groupmessage' ).focus();
	else
		with ( external.createWindow( name + '/groupmessage', external.globals( 'cwd' ) + 'groupmessage.html', new Array( window, name ) ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_group_message' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\letter.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 550, 400 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 300 ) / 2 );
		}
}
