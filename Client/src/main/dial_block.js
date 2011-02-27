function dial_block ( Address )
{
	if ( typeof Address == 'string' )
		Address = new XMPPAddress( Address );

	var ShortAddress = Address.ShortAddress();

	/* Unblock
	 */
	if ( external.globals( 'block' ).Exists( ShortAddress ) )
	{
		external.globals( 'block' ).Remove( ShortAddress );

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<presence type="subscribed"/>' );
		dom.documentElement.setAttribute( 'to', ShortAddress );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
	/* Block
	 */
	else
	{
		external.globals( 'block' ).Add( ShortAddress, null );

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<presence type="unavailable"/>' );
		dom.documentElement.setAttribute( 'to', ShortAddress );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );

		dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<presence type="unsubscribed"/>' );
		dom.documentElement.setAttribute( 'to', ShortAddress );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Update the conversation window and roster.
	 */
	if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
	{
		external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).RefreshAll();
		external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).UpdateTracker();
	}

	/* Update the settings window.
	 */
	external.globals( 'cfg' ).Item( 'blocklist' ) = ( new VBArray( external.globals( 'block' ).Keys() ) ).toArray().toString();
	if ( external.windows.Exists( 'preferences' ) )
		external.windows( 'preferences' ).Do( 'block_refresh', 0 );
}
