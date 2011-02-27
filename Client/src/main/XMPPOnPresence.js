function XMPPOnPresence ( ReceivedXML )
{
	if ( external.globals( 'debug' ) )
		warn( 'RECV: ' + ReceivedXML.xml );

	var Presence = new XMPPPresence();
	Presence.FromDOM( ReceivedXML );

	/* Default to our server if missing from address
	 */
	if ( ! Presence.FromAddress.Host.length )
		Presence.FromAddress.Parse( external.globals( 'cfg' ).Item( 'server' ) );

	/* Plugin event
	 */
	var context = new ActiveXObject( 'Scripting.Dictionary' );
	context.Add( 'name', 'onXMPPPresence' );
	context.Add( 'xmldom', Presence.XMLDOM );
	if ( external.globals( 'ClientPluginContainer' ).DispatchEvent( context ) )
		return;

	var ShortAddress = Presence.FromAddress.ShortAddress();
	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );

	/* Subscription Request
	 */
	if ( Presence.Type == 'subscribe' )
	{
		/* This person is on the blocking list
		 */
		if ( external.globals( 'block' ).Exists( ShortAddress ) )
		{
			dom.loadXML( '<presence type="unsubscribed"/>' );
			dom.documentElement.setAttribute( 'to', Presence.From );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
		/* It's a transport or someone on the other side of a transport
		 */
		else if ( ( external.globals( 'ClientServices' ).Services.Exists( Presence.FromAddress.Host ) && external.globals( 'ClientServices' ).Services.Item( Presence.FromAddress.Host ).Options & 0x0001 ) || external.globals( 'ClientServices' ).PendingDisco.Exists( Presence.FromAddress.Host ) )
		{
			dom.loadXML( '<presence type="subscribed"/>' );
			dom.documentElement.setAttribute( 'to', Presence.From );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
			if ( Presence.FromAddress.User.length )
			{
				dom.loadXML( '<presence type="subscribe"/>' );
				dom.documentElement.setAttribute( 'to', Presence.From );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		}
		else
		{
			var Choice = parseInt( external.globals( 'cfg' ).Item( 'authorization' ), 10 );

			if ( Choice == 0 || ( Choice == 2 && ! external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) ) )
			{
				/* Ask
				 */
				if ( external.windows.Exists( ShortAddress + '/request' ) )
					external.windows( ShortAddress + '/request' ).focus();
				else
					with ( external.createWindow( ShortAddress + '/request', external.globals( 'cwd' ) + 'auth_invitation.html', new Array( window, ShortAddress ) ) )
					{
						setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_authrequest' ) );
						setIcon( external.globals( 'cwd' ) + '..\\images\\contact-add\\contact-add.ico' );
						resizeable( false );
						showMinBox( true );
						MinHeight = MinWidth = 0;
						setSize( 450, 140 );
						setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 140 ) / 2 );
					}
			}
			else if ( Choice == 3 )
			{
				/* Deny
				 */
				dom.loadXML( '<presence type="unsubscribed"/>' );
				dom.documentElement.setAttribute( 'to', Presence.From );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
			else
			{
				/* Accept
				 */
				dom.loadXML( '<presence type="subscribed"/>' );
				dom.documentElement.setAttribute( 'to', Presence.From );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );

				if ( ! external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
				{
					dom.loadXML( '<iq type="set"><query xmlns="jabber:iq:roster"><item/></query></iq>' );
					dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
					dom.selectSingleNode( '/iq/query/item' ).setAttribute( 'jid', ShortAddress );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}

				if
					(
						! external.globals( 'ClientRoster' ).Items.Exists( ShortAddress )
						||
						(
							! external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Ask
							&&
							(
								external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Subscription == 'none'
								||
								external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Subscription == 'from'
							)
						)
					)
				{
					dom.loadXML( '<presence type="subscribe"/>' );
					dom.documentElement.setAttribute( 'to', Presence.From );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}

				if ( ! external.wnd.isActive() )
				{
					external.wnd.hide( false );
					external.wnd.restore();
					external.wnd.focus();
				}

				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'msg_cl_added', [ Presence.FromAddress.CleanAddress() ] ), external.globals( 'softwarename' ), 0 | 64 );
			}
		}
	}

	/* Subscription Approved
	 */
	else if ( Presence.Type == 'subscribed' )
	{
	}

	/* Subscription Denied
	 */
	else if ( Presence.Type == 'unsubscribed' || ( Presence.Type == 'error' && Presence.XMLDOM.selectSingleNode( '/presence[@type="error"]/error[@code="404"]' ) ) )
	{
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) && external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Ask )
		{
			var desc;
			var error = Presence.XMLDOM.selectSingleNode( '/presence/status' );
			if ( error && ! Presence.XMLDOM.selectSingleNode( '/presence/error/*[@xmlns="urn:ietf:params:xml:ns:xmpp-stanzas"]' ) )
				desc = error.text;
			else
				desc = external.globals( 'Translator' ).Translate( 'main', 'cl_error_notexist' );

			dom.loadXML( '<iq type="set"><query xmlns="jabber:iq:roster"><item subscription="remove"/></query></iq>' );
			dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
			dom.selectSingleNode( '/iq/query/item' ).setAttribute( 'jid', ShortAddress );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );

			if ( ! external.wnd.isActive() )
			{
				external.wnd.hide( false );
				external.wnd.restore();
				external.wnd.focus();
			}

			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'cl_error_cannotadd', [ Presence.FromAddress.CleanAddress(), desc ] ), external.globals( 'softwarename' ), 0 | 48 );
		}
	}

	/* Subscription Canceled
	 */
	else if ( Presence.Type == 'unsubscribe' )
	{
	}

	/* Conference participant
	 */
	else if ( external.globals( 'ConferenceSessionPool' ).Trackers.Exists( ShortAddress ) || external.globals( 'ConferenceSessionPool' ).TrackersLoading.Exists( ShortAddress ) )
	{
		var Event = new ConferenceSessionEvent();
		Event.Type = 'presence';
		Event.Address = Presence.FromAddress;
		Event.Payload = Presence;
		external.globals( 'ConferenceSessionPool' ).AddEvent( Event );
	}

	/* Status of a Contact
	 */
	else if ( ShortAddress.length )
	{
		external.globals( 'ClientRoster' ).ReceivePresence( Presence );
	}

	/* Invisible not supported/allowed by server
	 */
	else if ( Presence.Type == 'error' && external.globals( 'cfg' ).Item( 'lastmode' ) == 5 )
	{
		mode_new( 0, external.globals( 'cfg' ).Item( 'lastmsg' ) );
	}

	/* Disconnected
	 */
	else if ( Presence.Type == 'error' )
	{
		external.XMPP.Disconnect();
	}
}
