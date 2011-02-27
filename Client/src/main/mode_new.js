function mode_new ( mode, msg )
{
	var cfg = external.globals( 'cfg' );
	if ( mode == -1 )
	// offline
	{
		external.globals( 'XMPPReconnect' ) = false;
		document.getElementById( 'connecting' ).style.display = 'none';
		document.getElementById( 'rosterfield' ).style.display = 'block';
		document.getElementById( 'mode-avatar' ).src = external.globals( 'usersdir' ) + 'My Avatars\\' + cfg( 'avatar' );
		document.getElementById( 'mode-name' ).innerText = cfg( 'nick' );
		if ( msg.length )
			cfg( 'lastmsg' ) = msg;
		document.getElementById( 'mode-message' ).innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_offline' ) + ( cfg( 'lastmsg' ).length ? ' - ' + cfg( 'lastmsg' ) : '' );

		if ( external.globals( 'XMPPConnected' ) )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence type="unavailable"/>' );
			if ( cfg( 'lastmsg' ).length )
			{
				var Status = dom.createElement( 'status' );
				Status.text = cfg( 'lastmsg' );
				dom.documentElement.appendChild( Status );
			}
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );

			warn( 'SENT: </stream:stream>' );
			external.XMPP.SendText( '</stream:stream>' );
		}

		external.XMPP.Disconnect();
		MenuBarUpdate( 'file' );
	}
	else
	// any kind of online
	{
		cfg( 'lastmsg' ) = msg;
		cfg( 'lastmode' ) = mode;
		MenuBarUpdate( 'file' );
		if ( ! cfg( 'nick' ).length )
			cfg( 'nick' ) = cfg( 'username' );
		if ( ! cfg( 'nick' ).length )
			cfg( 'nick' ) = cfg( 'server' );
		document.getElementById( 'mode-name' ).innerText = cfg( 'nick' );
		document.getElementById( 'mode-avatar' ).src = external.globals( 'usersdir' ) + 'My Avatars\\' + cfg( 'avatar' );
		var mode_text = [
			external.globals( 'Translator' ).Translate( 'main', 'cl_available' ),
			external.globals( 'Translator' ).Translate( 'main', 'cl_ffc' ),
			external.globals( 'Translator' ).Translate( 'main', 'cl_away' ),
			external.globals( 'Translator' ).Translate( 'main', 'cl_xaway' ),
			external.globals( 'Translator' ).Translate( 'main', 'cl_busy' ),
			external.globals( 'Translator' ).Translate( 'main', 'cl_invisible' ),
			external.globals( 'Translator' ).Translate( 'main', 'cl_idle' )
		];
		document.getElementById( 'mode-message' ).innerText = mode_text[mode] + ( msg.length ? ' - ' + msg.replace( /[\n\r]+/mg, ' - ' ) : '' );

		if ( document.getElementById( 'connecting' ).style.display == 'block' )
			return;

		if ( ! external.globals( 'XMPPConnected' ) )
		{
			document.getElementById( 'connecting' ).style.display = 'block';
			document.getElementById( 'commerror' ).style.display = 'none';
			document.getElementById( 'welcomemsg' ).style.display = 'none';
			document.getElementById( 'rosterfield' ).style.display = 'none';

			if ( ! cfg( 'resource' ).length )
				cfg( 'resource' ) = external.globals( 'softwarename' );

			clearTimeout( external.globals( 'XMPPReconnectTimeout' ) );

			/* Plugin event
			 */
			var context = new ActiveXObject( 'Scripting.Dictionary' );
			context.Add( 'name', 'onConnect' );
			external.globals( 'ClientPluginContainer' ).DispatchEvent( context );

			/* Attempt the connection
			 */
			var host = external.globals( 'serveraddress' ).length ? external.globals( 'serveraddress' ) : cfg( 'server' );
			var port = external.globals( 'serverport' ).length ? external.globals( 'serverport' ) : ( external.globals( 'encryption' ) == 'ssl' ? 5223 : 5222 );
			var ssl = external.globals( 'encryption' ) == 'ssl' ? 1 : 0;
			warn( 'EVNT: Connecting to ' + host + ( ssl ? ':' + port + ' (ssl on)' : '' ) );
			external.XMPP.Connect( host, port, ssl );
		}
		else if ( cfg( 'username' ).length && cfg( 'server' ).length && cfg( 'resource' ).length )
		{
			var priority = 0, show = '', type = '';
			switch ( mode )
			{
				case 0:
				case 5: priority = 8; break;
				case 1: priority = 10; show = 'chat'; break;
				case 2: priority = 4; show = 'away'; break;
				case 3:
				case 6: priority = 2; show = 'xa'; break;
				case 4: priority = 6; show = 'dnd'; break;
			}

			if ( mode == 5 )
				switch ( external.globals( 'XMPPPrivacy' ) )
				{
					case -1:
						type = 'invisible';
					break;
					case 0:
						external.globals( 'XMPPPrivacy' ) = 1;
						var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
						dom.loadXML( '<presence type="unavailable"/>' );
						warn( 'SENT: ' + dom.xml );
						external.XMPP.SendXML( dom );

						dom = new ActiveXObject( 'Msxml2.DOMDocument' );
						dom.loadXML( '<iq type="set"><query xmlns="jabber:iq:privacy"><active name="invisible"/></query></iq>' );
						warn( 'SENT: ' + dom.xml );
						external.XMPP.SendXML( dom );
					break;
				}
			else if ( external.globals( 'XMPPPrivacy' ) == 1 )
			{
				external.globals( 'XMPPPrivacy' ) = 0;
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<iq type="set"><query xmlns="jabber:iq:privacy"><active/></query></iq>' );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence><x xmlns="jabber:x:avatar"><hash/></x></presence>' );
			dom.selectSingleNode( '/presence/x/hash' ).text = cfg( 'avatar' );

			if ( msg.length )
			{
				var tag = dom.createElement( 'status' );
				tag.text = msg;
				dom.documentElement.appendChild( tag );
			}
			if ( priority )
			{
				var tag = dom.createElement( 'priority' );
				tag.text = priority;
				dom.documentElement.appendChild( tag );
			}
			if ( show.length )
			{
				var tag = dom.createElement( 'show' );
				tag.text = show;
				dom.documentElement.appendChild( tag );
			}
			if ( type.length )
			{
				dom.documentElement.setAttribute( 'type', type );
			}

			/* Plug-in event
			 */
			var context = new ActiveXObject( 'Scripting.Dictionary' );
			context.Add( 'name', 'onSendPresence' );
			context.Add( 'xmldom', dom );
			external.globals( 'ClientPluginContainer' ).DispatchEvent( context );

			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );

			var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
			for ( var i = 0; i < TrackerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).SendPresence( mode, msg );
		}
	}
}
