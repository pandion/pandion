function dial_status_message ( Mode )
{
	if ( external.windows.Exists( 'leaveanote' ) )
	{
		external.windows( 'leaveanote' ).Do( 'SetMode', Mode );
		external.windows( 'leaveanote' ).focus();
	}
	else
		with ( external.createWindow( 'leaveanote', external.globals( 'cwd' ) + 'mode_select.html', [ window, Mode ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_status' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\idle\\note.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 420, 160 );
			setPos( ( screen.availWidth - 420 ) / 2, ( screen.availHeight - 160 ) / 2 );
		}
}
