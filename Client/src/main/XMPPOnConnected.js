function XMPPOnConnected ()
{
	if ( ! external.globals( 'XMPPConnected' ) )
	{
		external.globals( 'XMPPConnected' ) = true;

		var Str = '<?xml version="1.0"?>';
		warn( 'SENT: ' + Str );
		external.XMPP.SendText( Str );

		if ( external.globals( 'bannerhalf' ).length )
			BannerConnect();

		if ( external.globals( 'bannerfull' ).length )
		{
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).DOM.BannerDelayConnect( 2500 );

			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Containers( ContainerNames[i] ).DOM.BannerDelayConnect( 2500 );
		}
	}

	if ( external.globals( 'encryption' ) == 'ssl' )
	{
		external.globals( 'XMPPEncryption' ) = 'ssl';
		document.getElementById( 'mode-secure' ).style.display = 'inline';
	}

	var Str = '<stream:stream to="' + external.globals( 'cfg' )( 'server' ) + '" xml:lang="' + external.globals( 'language' ) + '" xmlns="jabber:client" xmlns:stream="http://etherx.jabber.org/streams"' + ( external.globals( 'encryption' ) == 'ssl' ? '' : ' version="1.0"' ) + '>';
	warn( 'SENT: ' + Str );
	external.XMPP.SendText( Str );
}
