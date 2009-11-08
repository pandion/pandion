function OnLoginAuthSend ( iq )
{
	/* Supported authentication mechanisms
	 */
	if ( iq.Type == 'result' )
	{
		var hook		= new XMPPHookIQ();
		hook.Window		= external.wnd;
		hook.Callback	= 'OnLoginAuthReceive';

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="set"><query/></iq>' );
		dom.documentElement.setAttribute( 'id', hook.Id );
		dom.documentElement.setAttribute( 'to', external.globals( 'cfg' )( 'server' ) );

		if ( iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="jabber:iq:auth"]/username' ) )
		{
			var tag		= dom.createElement( 'username' );
			tag.text	= external.globals( 'cfg' )( 'username' );
			dom.documentElement.firstChild.appendChild( tag );
		}

		if ( iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="jabber:iq:auth"]/resource' ) )
		{
			var tag		= dom.createElement( 'resource' );
			tag.text	= external.globals( 'cfg' )( 'resource' );
			dom.documentElement.firstChild.appendChild( tag );
		}

		if ( iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="jabber:iq:auth"]/digest' ) )
		{
			var tag		= dom.createElement( 'digest' );
			tag.text	= external.StringToSHA1( external.globals( 'XMPPSessionID' ) + external.globals( 'cfg' )( 'password' ) );
			dom.documentElement.firstChild.appendChild( tag );
		}

		else if ( iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="jabber:iq:auth"]/password' ) )
		{
			var tag		= dom.createElement( 'password' );
			tag.text	= external.globals( 'cfg' )( 'password' );
			dom.documentElement.firstChild.appendChild( tag );
		}

		dom.documentElement.firstChild.setAttribute( 'xmlns', 'jabber:iq:auth' );

		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
	else
	{
		OnLoginAuthError( iq );
	}
}

function OnLoginAuthReceive ( iq )
{
	/* Logged in or not
	 */
	if ( iq.Type == 'result' )
	{
		if ( external.windows.Exists( 'signup' ) )
			external.windows( 'signup' ).Do( 'recv_finish', '' );
		else
			OnLoginGetDisco();
	}
	else
	{
		OnLoginAuthError( iq );
	}
}

function OnLoginAuthError ( iq )
{
	if ( external.windows.Exists( 'signup' ) )
		external.windows( 'signup' ).Do( 'step1_recv', iq );
	else
		OnLoginAbort();
}

function OnLoginAbort ()
{
	if ( external.windows.Exists( 'signup' ) )
		external.windows( 'signup' ).Do( 'step1_error', null );
	else
	{
		/* Plugin event
		 */
		var context = new ActiveXObject( 'Scripting.Dictionary' );
		context.Add( 'name', 'onLoginFailure' );
		external.globals( 'ClientPluginContainer' ).DispatchEvent( context );

		warn( 'SENT: </stream:stream>' );
		external.XMPP.SendText( '</stream:stream>' );
		external.XMPP.Disconnect();

		if ( ! external.globals( 'sspiserver' ).length )
		{
			dial_login();
			external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_autherror' ), external.globals( 'softwarename' ), 0 | 48 );
		}
	}
}

function OnLoginBind ( iq )
{
	/* Successful binding
	 */
	if ( iq.Type == 'result' )
	{
		if ( iq.XMLDOM.selectSingleNode( '/iq[@type="result"]/bind[@xmlns="urn:ietf:params:xml:ns:xmpp-bind"]/jid' ) )
		{
			var Address = new XMPPAddress( iq.XMLDOM.selectSingleNode( '/iq[@type="result"]/bind[@xmlns="urn:ietf:params:xml:ns:xmpp-bind"]/jid' ).text );
			if ( Address.LongAddress() != external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) + '/' + external.globals( 'cfg' )( 'resource' ) )
			{
				/* Reload profile
				 */
				LoadProfile( Address );

				/* Reset our address
				 */
				var cfg = external.globals( 'cfg' );
				cfg( 'username' )	= '';
				cfg( 'server' )		= '';
				cfg( 'resource' )	= '';

				/* Redraw status mode, status message, avatar, name
				 */
				mode_new( parseInt( cfg( 'lastmode' ), 10 ), cfg( 'lastmsg' ) );

				/* Update our address
				 */
				cfg( 'username' )	= Address.User;
				cfg( 'server' )		= Address.Host;
				cfg( 'resource' )	= Address.Resource;
			}
		}

		/* Got an address for the session
		 */
		if ( external.globals( 'cfg' )( 'username' ).length && external.globals( 'cfg' )( 'server' ).length && external.globals( 'cfg' )( 'resource' ).length )
		{
			var hook		= new XMPPHookIQ();
			hook.Window		= external.wnd;
			hook.Callback	= 'OnLoginSession';
//			hook.From		= external.globals( 'cfg' )( 'server' );

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="set"><session xmlns="urn:ietf:params:xml:ns:xmpp-session"/></iq>' );
			dom.documentElement.setAttribute( 'id', hook.Id );
			dom.documentElement.setAttribute( 'to', external.globals( 'cfg' )( 'server' ) );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
		/* Address incomplete
		 */
		else
		{
			OnLoginAbort();
		}
	}
	/* Can not specify our own resource so try requesting one
	 */
	else if ( iq.XMLDOM.selectSingleNode( '/iq[@type="error"]/bind[@xmlns="urn:ietf:params:xml:ns:xmpp-bind"]/resource' ) )
	{
		var hook		= new XMPPHookIQ();
		hook.Window		= external.wnd;
		hook.Callback	= 'OnLoginBind';

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="set"><bind xmlns="urn:ietf:params:xml:ns:xmpp-bind"/></iq>' );
		dom.documentElement.setAttribute( 'id', hook.Id );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
	/* Can not bind a resource at all
	 */
	else
	{
		OnLoginAbort();
	}
}

function OnLoginSession ( iq )
{
	/* Received a new session
	 */
	if ( iq.Type == 'result' )
	{
		if ( external.windows.Exists( 'signup' ) )
			external.windows( 'signup' ).Do( 'recv_finish', '' );
		else
			OnLoginGetDisco();
	}
	/* The resource is already in use, generate a random resource and retry
	 */
	else if ( iq.XMLDOM.selectSingleNode( '/iq[@type="error"]/error/conflict[@xmlns="urn:ietf:params:xml:ns:xmpp-stanzas"]' ) )
	{
		external.globals( 'cfg' )( 'resource' ) = external.globals( 'cfg' )( 'resource' ) + ' (' + Math.round( Math.random() * 0xffff ) + ')';

		var hook		= new XMPPHookIQ();
		hook.Window		= external.wnd;
		hook.Callback	= 'OnLoginBind';

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="set"><bind xmlns="urn:ietf:params:xml:ns:xmpp-bind"><resource/></bind></iq>' );
		dom.documentElement.setAttribute( 'id', hook.Id );
		dom.documentElement.firstChild.firstChild.text = external.globals( 'cfg' )( 'resource' );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
	/* Server can not create a session
	 */
	else
	{
		OnLoginAbort();
	}
}

function OnLoginGetDisco ()
{
	external.globals( 'ClientServices' ).JID = external.globals( 'ClientServices' ).Name = external.globals( 'cfg' )( 'server' );

	var hook		= new XMPPHookIQ();
	hook.Window		= external.wnd;
	hook.Callback	= 'OnLoginDiscoItems';
	hook.From		= external.globals( 'cfg' )( 'server' );

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	dom.loadXML( '<iq type="get"><query xmlns="http://jabber.org/protocol/disco#items"/></iq>' );
	dom.documentElement.setAttribute( 'id', hook.Id );
	dom.documentElement.setAttribute( 'to', external.globals( 'cfg' )( 'server' ) );
	warn( 'SENT: ' + dom.xml );
	external.XMPP.SendXML( dom );

	OnLoginGetRoster();
	OnLoginGetPrivacyLists();
}

function OnLoginDiscoItems ( iq )
{
	/* Query the disco items on the server
	 */
	if ( iq.Type == 'result' )
	{
		var DiscoItems = iq.XMLDOM.selectNodes( '/iq/*[@xmlns="http://jabber.org/protocol/disco#items"]/item[@jid and not( @node )]' );
		if ( DiscoItems.length )
		{
			for ( var i = 0; i < DiscoItems.length; ++i )
			{
				var Address = new XMPPAddress( DiscoItems.item(i).getAttribute( 'jid' ) );

				if ( ! external.globals( 'ClientServices' ).PendingDisco.Exists( Address.ShortAddress() ) )
				{
					external.globals( 'ClientServices' ).PendingDisco.Add( Address.ShortAddress(), null );

					var hook		= new XMPPHookIQ();
					hook.Window		= external.wnd;
					hook.Callback	= 'OnLoginDiscoInfo';
					hook.From		= Address.ShortAddress();

					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<iq type="get"><query xmlns="http://jabber.org/protocol/disco#info"/></iq>' );
					dom.documentElement.setAttribute( 'id', hook.Id );
					dom.documentElement.setAttribute( 'to', Address.ShortAddress() );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}
			}
		}
		/* Fall back to browse mechanism
		 */
		else
		{
			OnLoginGetBrowse();
		}
	}
	/* Fall back to browse mechanism
	 */
	else
	{
		OnLoginGetBrowse();
	}
}

function OnLoginDiscoInfo ( iq )
{
	if ( external.globals( 'ClientServices' ).PendingDisco.Exists( iq.FromAddress.ShortAddress() ) )
	{
		external.globals( 'ClientServices' ).PendingDisco.Remove( iq.FromAddress.ShortAddress() );

		external.globals( 'ClientServices' ).FromIQDisco( iq );

		if ( ! external.globals( 'ClientServices' ).PendingDisco.Count )
			OnReceivedServices();
	}
}

function OnLoginGetBrowse ()
{
	var hook		= new XMPPHookIQ();
	hook.Window		= external.wnd;
	hook.Callback	= 'OnLoginBrowse';
	hook.From		= external.globals( 'cfg' )( 'server' );

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:browse"/></iq>' );
	dom.documentElement.setAttribute( 'id', hook.Id );
	dom.documentElement.setAttribute( 'to', external.globals( 'cfg' )( 'server' ) );
	warn( 'SENT: ' + dom.xml );
	external.XMPP.SendXML( dom );
}

function OnLoginBrowse ( iq )
{
	/* Our transports and services as browse results
	 */
	if ( iq.Type == 'result' )
	{
		external.globals( 'ClientServices' ).FromIQBrowse( iq );
		OnReceivedServices();
		MenuBarUpdate( 'tools' );
	}
	/* Fall back to agents mechanism
	 */
	else
	{
		var hook		= new XMPPHookIQ();
		hook.Window		= external.wnd;
		hook.Callback	= 'OnLoginAgents';
		hook.From		= external.globals( 'cfg' )( 'server' );

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:agents"/></iq>' );
		dom.documentElement.setAttribute( 'id', hook.Id );
		dom.documentElement.setAttribute( 'to', external.globals( 'cfg' )( 'server' ) );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
}

function OnLoginAgents ( iq )
{
	/* Our transports and services as agents
	 */
	if ( iq.Type == 'result' )
		external.globals( 'ClientServices' ).FromIQAgents( iq );

	OnReceivedServices();
	MenuBarUpdate( 'tools' );
}

function OnLoginGetPrivacyLists ()
{
	var hook		= new XMPPHookIQ();
	hook.Window		= external.wnd;
	hook.Callback	= 'OnLoginReceivePrivacyLists';

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:privacy"/></iq>' );
	dom.documentElement.setAttribute( 'id', hook.Id );
	warn( 'SENT: ' + dom.xml );
	external.XMPP.SendXML( dom );
}

function OnLoginReceivePrivacyLists ( iq )
{
	/* Server supports jabber:iq:privacy
	 */
	if ( iq.Type == 'result' )
	{
		/* According to JEP-0126 this SHOULD be the standard invisibility list; bad luck if it is not
		 */
		if ( iq.XMLDOM.selectSingleNode( '/iq[@type="result"]/query[@xmlns="jabber:iq:privacy"]/list[@name="invisible"]' ) )
		{
			external.globals( 'XMPPPrivacy' ) = 0;
			if ( external.globals( 'XMPPReconnect' ) && external.globals( 'cfg' )( 'lastmode' ) == 5 )
				mode_new( external.globals( 'cfg' )( 'lastmode' ), external.globals( 'cfg' )( 'lastmsg' ) );
		}
		else
		{
			var hook		= new XMPPHookIQ();
			hook.Window		= external.wnd;
			hook.Callback	= 'OnLoginPrivacySetInvisible';

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="set"><query xmlns="jabber:iq:privacy"><list name="invisible"><item action="deny" order="1"><presence-out/></item></list></query></iq>' );
			dom.documentElement.setAttribute( 'id', hook.Id );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
	}
	/* Server does not support jabber:iq:privacy
	 */
	else
	{
		external.globals( 'XMPPPrivacy' ) = -1;
	}
}

function OnLoginPrivacySetInvisible ( iq )
{
	/* Server supports jabber:iq:privacy
	 */
	if ( iq.Type == 'result' )
	{
		external.globals( 'XMPPPrivacy' ) = 0;
		if ( external.globals( 'XMPPReconnect' ) && external.globals( 'cfg' )( 'lastmode' ) == 5 )
			mode_new( external.globals( 'cfg' )( 'lastmode' ), external.globals( 'cfg' )( 'lastmsg' ) );
	}
	/* Server does not support jabber:iq:privacy
	 */
	else
	{
		external.globals( 'XMPPPrivacy' ) = -1;
	}
}

function OnLoginGetRoster ()
{
	var hook		= new XMPPHookIQ();
	hook.Window		= external.wnd;
	hook.Callback	= 'OnLoginRoster';

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:roster"/></iq>' );
	dom.documentElement.setAttribute( 'id', hook.Id );
	warn( 'SENT: ' + dom.xml );
	external.XMPP.SendXML( dom );
}

function OnReceivedServices ()
{
	external.globals( 'ClientRoster' ).RefreshAll();
	if ( external.windows.Exists( 'transport_list' ) )
		external.windows( 'transport_list' ).Do( 'transport_refresh', null );
}

function OnLoginRoster ( iq )
{
	document.getElementById( 'connecting'	).style.display = 'none';
	document.getElementById( 'commerror'	).style.display = 'none';

	if ( external.globals( 'ClientRoster' ).Groups.Count )
		external.globals( 'ClientRoster' ).ReloadFromIQ( iq );
	else
		external.globals( 'ClientRoster' ).FromIQ( iq );

	document.getElementById( 'rosterfield'	).style.display = external.globals( 'ClientRoster' ).Groups.Count ? 'block'	: 'none';
	document.getElementById( 'welcomemsg'	).style.display = external.globals( 'ClientRoster' ).Groups.Count ? 'none'	: 'block';

	var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
	for ( var i = 0; i < ContainerNames.length; ++i )
		external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).InputUpdate();

	if ( external.windows.Exists( 'transport_list' ) )
		external.windows( 'transport_list' ).Do( 'transport_refresh', null );

	if ( external.windows.Exists( 'conference_browse' ) )
		external.windows( 'conference_browse' ).Do( 'Refresh', null );

	MenuBarUpdate( 'actions' );
	MenuBarUpdate( 'tools' );

	external.globals( 'connecttime' )			= ( new Date() ).getTime();
	external.globals( 'soundtime' )				= ( new Date() ).getTime() + 8000;
	external.globals( 'autoaway' )				= setTimeout( 'autoaway( -1 , -1 )', 2000 );
	external.globals( 'XMPPKeepAliveInterval' )	= setInterval( 'external.XMPP.SendText( " " )', 30000 );
	external.globals( 'XMPPReconnect' )			= true;

	mode_new( external.globals( 'cfg' )( 'lastmode' ), external.globals( 'cfg' )( 'lastmsg' ) );

	/* Check for an updated version
	 */
	if ( isNaN( parseInt( external.globals( 'last_autoupdate' ), 10 ) ) )
		external.globals( 'last_autoupdate' ) = 0;
	else
		external.globals( 'last_autoupdate' ) = parseInt( external.globals( 'last_autoupdate' ), 10 );

	if ( external.globals( 'last_autoupdate' ) < ( new Date() ).getTime() - ( /* 7 * */ 24 * 3600 * 1000 ) )
		dial_autoupdate( false );
	else
	{
		var Now = ( new Date() ).getTime();
		var Future = external.globals( 'last_autoupdate' ) + /* 7 * */ 24 * 3600 * 1000;
		var Remaining = Future - Now;
		external.globals( 'AutoUpdateTimeout' ) = setTimeout( 'dial_autoupdate( false )', Remaining );
	}

	/* Activate the fileserver on a random or mapped port
	 */
//	external.HTTPEngine.Listen( isNaN( parseInt( external.globals( 'cfg' )( 'filetransferport' ), 10 ) ) ? 0 : parseInt( external.globals( 'cfg' )( 'filetransferport' ), 10 ) );

	/* Execute any command line actions that were queued while disconnected.
	 */
	while ( external.globals( 'CommandLineQueue' ).length )
		CommandLineShift();

	/* Open welcome screen
	 */
	if ( external.globals( 'welcomescreen' ) == 'yes' )
		dial_welcome();

	/* Plugin event
	 */
	var context = new ActiveXObject( 'Scripting.Dictionary' );
	context.Add( 'name', 'onLoginSuccess' );
	external.globals( 'ClientPluginContainer' ).DispatchEvent( context );

	/* Reconnect conference rooms
	 */
	var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
	for ( var i = 0; i < TrackerNames.length; ++i )
		external.globals( 'ConferenceSessionPool' ).Trackers( TrackerNames[i] ).Connect();

	/* Automatically open bookmarked conference rooms
	 */
	var dom					= new ActiveXObject( 'MSXML2.DOMDocument' );
	dom.async				= false;
	dom.resolveExternals	= false;
	dom.load( external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) + '\\bookmarks.xml' );
	if ( dom.documentElement )
	{
		var BookmarkNodes = dom.documentElement.selectNodes( '/bookmarks/room[@address]' );
		for ( var i = 0; i < BookmarkNodes.length; ++i )
			if ( BookmarkNodes.item( i ).getAttribute( 'autojoin' ).toString() == 'true' )
			{
				var Address		= BookmarkNodes.item( i ).getAttribute( 'address' );
				var Password	= BookmarkNodes.item( i ).getAttribute( 'password' );
				dial_conference( Address, Password );
			}
	}
}
