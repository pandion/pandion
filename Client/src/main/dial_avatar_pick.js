function dial_avatar_pick ()
{
	if ( external.windows.Exists( 'pick_avatar' ) )
		external.windows( 'pick_avatar' ).focus();
	else
		with ( external.createWindow( 'pick_avatar', external.globals( 'cwd' ) + 'avatar.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_avatar' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\tree.ico' );
			resizeable( false );
			setSize( 500, 400 );
			setPos( ( screen.availWidth - 500 ) / 2, ( screen.availHeight - 400 ) / 2 );
			focus();
		}
}
