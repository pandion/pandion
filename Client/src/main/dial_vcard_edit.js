function dial_vcard_edit ( jid )
{
	if ( ! jid )
		jid = external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' );
	if ( external.windows.Exists( jid + '/vcard_edit' ) )
	{
		if ( external.windows( jid + '/vcard_edit' ).isMinimized )
			external.windows( jid + '/vcard_edit' ).restore();
		external.windows( jid + '/vcard_edit' ).focus();
	}
	else
		with ( external.createWindow( jid + '/vcard_edit', external.globals( 'cwd' ) + 'vcard_edit.html', [ window, jid ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_vcard_edit' ) );
			resizeable( false );
			showMinBox( true );
			setIcon( external.globals( 'cwd' ) + '..\\images\\vcard\\card.ico' );
			setSize( 490, 350 );
			setPos( ( screen.availWidth - 480 ) / 2, ( screen.availHeight - 350 ) / 2 );
		}
}
