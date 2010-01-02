function dial_autoupdate ( selectVersion )
{
	if ( ! external.windows.Exists( 'autoupdate' ) )
		with ( external.createWindow( 'autoupdate', external.globals( 'cwd' ) + 'autoupdate.html', { window: window, selectVersion: !! selectVersion } ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_autoupdate', [ external.globals( 'softwarename' ) ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 450, 380 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 380 ) / 2 );
		}
}
