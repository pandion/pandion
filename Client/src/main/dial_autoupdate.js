function dial_autoupdate ()
{
	if ( ! external.windows.Exists( 'autoupdate' ) )
		with ( external.createWindow( 'autoupdate', external.globals( 'cwd' ) + 'autoupdate.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_autoupdate', [ external.globals( 'softwarename' ) ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 450, 350 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 350 ) / 2 );
		}
}
