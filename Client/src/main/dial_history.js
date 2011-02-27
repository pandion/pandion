function dial_history ( jid )
{
	if ( external.windows.Exists( jid + '/history' ) )
	{
		if ( external.windows( jid + '/history' ).isMinimized )
			external.windows( jid + '/history' ).restore();
		external.windows( jid + '/history' ).focus();
	}
	else
		with ( external.createWindow( jid + '/history', external.globals( 'cwd' ) + 'history.html', new Array( window, jid ) ) )
		{
			var nick = jid;
			if ( external.globals( 'ClientRoster' ).Items.Exists( jid ) )
				nick = external.globals( 'ClientRoster' ).Items.Item( jid ).Name;
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_history', [ nick ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\history\\books.ico' );
			setSize( Math.max( 600,  Math.round( window.screen.availWidth / 2 ) ), Math.max( 400,  Math.round( window.screen.availHeight / 2 ) ) );
			setPos( ( screen.availWidth - ( Math.max( 600,  Math.round( window.screen.availWidth / 2 ) ) ) ) / 2, ( screen.availHeight - ( Math.max( 400,  Math.round( window.screen.availHeight / 2 ) ) ) ) / 2 );
			MinWidth = 300;
			MinHeight = 200;
		}
}
