function XMPPOnDocumentStart ( ReceivedFragment )
{
try {
	warn( 'RECV: ' + ReceivedFragment );

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	dom.loadXML( ReceivedFragment + '</stream:stream>' );

	/* Get the stream ID and version
	 */
	external.globals( 'XMPPSessionID' ) = dom.documentElement.getAttribute( 'id' );
	external.globals( 'XMPPStreamVersion' ) = dom.documentElement.getAttribute( 'version' );

	/* Accept server specified hostname
	 */
	if ( dom.documentElement.getAttribute( 'from' ) && dom.documentElement.getAttribute( 'from' ).toLowerCase() != external.globals( 'cfg' ).Item( 'server' ).toLowerCase() )
	{
		var Username = external.globals( 'cfg' ).Item( 'username' );
		var Server = dom.documentElement.getAttribute( 'from' );
		var Resource = external.globals( 'cfg' ).Item( 'resource' );

		LoadProfile( new XMPPAddress( Username + '@' + Server + '/' + Resource ) );

		external.globals( 'cfg' ).Item( 'username' ) = Username;
		external.globals( 'cfg' ).Item( 'server' ) = Server;
		external.globals( 'cfg' ).Item( 'resource' ) = Resource;
	}

	/* XMPP compliant server
	 */
	if ( external.globals( 'XMPPStreamVersion' ) )
	{
		/* Wait for stream:features to arrive at XMPPOnStream
		 */
	}
	/* Plain Jabber server
	 */
	else
	{
		/* TLS is not available so disconnect
		 */
		if ( external.globals( 'encryption' ) == 'tls' )
		{
			external.globals( 'XMPPReconnect' ) = false;
			warn( 'SENT: </stream:stream>' );
			external.XMPP.SendText( '</stream:stream>' );
			external.XMPP.Disconnect();
		}
		/* Use Jabber authentication protocol
		 */
		else
		{
			var hook = new XMPPHookIQ();
			hook.Window = external.wnd;
			hook.Callback = 'OnLoginAuthSend';

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:auth"><username/></query></iq>' );
			dom.documentElement.firstChild.firstChild.text = external.globals( 'cfg' ).Item( 'username' );
			dom.documentElement.setAttribute( 'to', external.globals( 'cfg' ).Item( 'server' ) );
			dom.documentElement.setAttribute( 'id', hook.Id );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
	}
} catch (error) {
	warn("ReceivedFragment: " + ReceivedFragment);
	warn("XMPPOnDocumentStart error: " + error.description);
}
}
