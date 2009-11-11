function XMPPOnConnected ()
{
	if ( ! external.globals( 'XMPPConnected' ) )
	{
		external.globals( 'XMPPConnected' ) = true;
		var Str = '<?xml version="1.0"?>';
		warn( 'SENT: ' + Str );
		external.XMPP.SendText( Str );
	}

	if ( external.globals( 'encryption' ) == 'ssl' )
	{
		external.globals( 'XMPPEncryption' ) = 'ssl';
		document.getElementById( 'mode-secure' ).style.display = 'inline';
	}

	var Str = '<stream:stream to="' + external.globals( 'cfg' )( 'server' ) + '" xml:lang="' + external.globals( 'language' ) + '" xmlns="jabber:client" xmlns:stream="http://etherx.jabber.org/streams" version="1.0">';
	warn( 'SENT: ' + Str );
	external.XMPP.SendText( Str );
}
