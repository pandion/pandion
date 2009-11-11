function dial_groupmessage ( name )
{
	if ( external.windows.Exists( name + '/groupmessage' ) )
	{
		if ( external.windows( name + '/groupmessage' ).isMinimized )
			external.windows( name + '/groupmessage' ).restore();
		external.windows( name + '/groupmessage' ).focus();
	}
	else
		with ( external.createWindow( name + '/groupmessage', external.globals( 'cwd' ) + 'groupmessage.html', new Array( window, name ) ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_group_message' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\chat-container\\bubble.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 550, 400 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 300 ) / 2 );
		}
}
