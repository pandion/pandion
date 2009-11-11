function dial_status_message ( Mode )
{
	if ( external.windows.Exists( 'leaveanote' ) )
	{
		external.windows( 'leaveanote' ).Do( 'SetMode', Mode );
		if ( external.windows( 'leaveanote' ).isMinimized )
			external.windows( 'leaveanote' ).restore();
		external.windows( 'leaveanote' ).focus();
	}
	else
		with ( external.createWindow( 'leaveanote', external.globals( 'cwd' ) + 'mode_select.html', [ window, Mode ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_status' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\idle\\note.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 420, 140 );
			setPos( ( screen.availWidth - 420 ) / 2, ( screen.availHeight - 140 ) / 2 );
		}
}
