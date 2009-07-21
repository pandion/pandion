function XMPPOnIQ ( ReceivedXML )
{
	if ( external.globals( 'debug' ) )
		warn( 'RECV: ' + ReceivedXML.xml );

	var iq = new XMPPIQ();
	iq.FromDOM( ReceivedXML );

	/* Default to our server if missing from address
	 */
	if ( ! iq.FromAddress.Host.length )
		iq.FromAddress.Parse( external.globals( 'cfg' )( 'server' ) );

	/* Plug-in event
	 */
	var context = new ActiveXObject( 'Scripting.Dictionary' );
	context.Add( 'name', 'onXMPPIQ' );
	context.Add( 'xmldom', iq.XMLDOM );
	if ( external.globals( 'ClientPluginContainer' ).DispatchEvent( context ) )
		return;

	/* Check if someone is waiting for this packet
	 */
	if ( external.globals( 'pending' ).Exists( iq.Id ) )
	{
		var Hook = external.globals( 'pending' )( iq.Id );
		if
		(
			(
				Hook.From === false
				||
				Hook.From == iq.FromAddress.ShortAddress()
			)
			&&
			(
				Hook.Type == 0
				||
				( ( Hook.Type & 1 ) && ( iq.Type == 'get'		) )
				||
				( ( Hook.Type & 2 ) && ( iq.Type == 'set'		) )
				||
				( ( Hook.Type & 4 ) && ( iq.Type == 'result'	) )
				||
				( ( Hook.Type & 8 ) && ( iq.Type == 'error'		) )
			)
		)
		{
			try
			{
				Hook.Window.Do( Hook.Callback, iq );
				if ( --Hook.TTL == 0 )
					Hook.Destroy();
			}
			catch ( e )
			{
				/* Error occurs when the window is closed while the Callback is being executed
				 */
			}
			return;
		}
	}

	/* Parse the address
	 */
	var jid = iq.FromAddress.ShortAddress();
	var res = iq.FromAddress.Resource;

	/* Contact list changed
	 */
	if ( iq.Namespace == 'jabber:iq:roster' && iq.Type == 'set' )
	{
		document.getElementById( 'connecting'	).style.display = 'none';
		document.getElementById( 'commerror'	).style.display = 'none';
		if ( external.globals( 'ClientRoster'	).Groups.Count )
			external.globals( 'ClientRoster'	).ReceiveSetFromIQ( iq );
		else
			external.globals( 'ClientRoster'	).FromIQ( iq );
		document.getElementById( 'rosterfield'	).style.display = external.globals( 'ClientRoster' ).Groups.Count ? 'block'	: 'none';
		document.getElementById( 'welcomemsg'	).style.display = external.globals( 'ClientRoster' ).Groups.Count ? 'none'	: 'block';
		MenuBarUpdate( 'file' );
	}

	/* Send number of seconds since user activity
	 */
	else if ( iq.Namespace == 'jabber:iq:last' && iq.Type == 'get' )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="result"><query xmlns="jabber:iq:last"/></iq>' );
		dom.documentElement.firstChild.setAttribute( 'seconds', Math.floor( parseInt( external.globals( 'autoawaytime' ), 10 ) / 1000 ) );
		dom.documentElement.setAttribute( 'to', iq.From );
		if ( iq.Id.length )
			dom.documentElement.setAttribute( 'id', iq.Id );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Send software and system information
	 */
	else if ( iq.Namespace == 'jabber:iq:version' && iq.Type == 'get' )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="result"><query xmlns="jabber:iq:version"><name/><version/><os/></query></iq>' );
		dom.documentElement.setAttribute( 'to', iq.From );
		if ( iq.Id.length )
			dom.documentElement.setAttribute( 'id', iq.Id );
		dom.selectSingleNode( '/iq/query/name' ).text = external.globals( 'softwarename' );
		dom.selectSingleNode( '/iq/query/version' ).text = external.globals( 'softwareversion' );
		( /(Windows[^;\)]+)/ ).test( navigator.appVersion );
		var Windows = RegExp.$1;
		if ( Windows == 'Windows NT 5.0' )
			Windows = 'Windows 2000';
		else if ( Windows == 'Windows NT 5.1' )
			Windows = 'Windows XP';
		else if ( Windows == 'Windows NT 5.2' )
			Windows = 'Windows 2003';
		dom.selectSingleNode( '/iq/query/os' ).text = Windows + ' (' + navigator.userLanguage + ')';
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Send local time
	 */
	else if ( iq.Namespace == 'jabber:iq:time' && iq.Type == 'get' )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="result"><query xmlns="jabber:iq:time"><utc/><display/></query></iq>' );
		dom.documentElement.setAttribute( 'to', iq.From );
		if ( iq.Id.length )
			dom.documentElement.setAttribute( 'id', iq.Id );
		with ( new Date() )
		{
			var Y = getFullYear().toString();
			var M = (getMonth() + 1).toString();
			var D = getDate().toString();
			var h = getHours().toString();
			var m = getMinutes().toString();
			var s = getSeconds().toString();
			dom.selectSingleNode( '/iq/query/utc' ).text = Y + ( M.length > 1 ? '' : '0' ) + M + ( D.length > 1 ? '' : '0' ) + D + 'T' + ( h.length > 1 ? '' : '0' ) + h + ( m.length > 1 ? ':' : ':0' ) + m + ( s.length > 1 ? ':' : ':0' ) + s;
			dom.selectSingleNode( '/iq/query/display' ).text = toLocaleString();
		}
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Upload the user's avatar as Base64
	 */
	else if ( iq.Namespace == 'jabber:iq:avatar' && iq.Type == 'get' )
	{
		if ( external.globals( 'ConferenceSessionPool' ).Trackers.Exists( jid ) || ( external.globals( 'ClientRoster' ).Items.Exists( jid ) && ! external.globals( 'block' ).Exists( jid ) ) )
		{
			/* Upload the user's avatar.
			 */
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="result"><query xmlns="jabber:iq:avatar"><data/></query></iq>' );
			dom.documentElement.setAttribute( 'to', iq.From );
			if ( iq.Id.length )
				dom.documentElement.setAttribute( 'id', iq.Id );
			dom.selectSingleNode( '/iq/query/data' ).text = external.File( external.globals( 'usersdir' ) + 'My Avatars\\' + external.globals( 'cfg' )( 'avatar' ) ).ReadBase64();
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
		else
		{
			/* Not allowed
			 */
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="error"><query xmlns="jabber:iq:avatar"/><error code="405">Automatically denied.</error></iq>' );
			dom.documentElement.setAttribute( 'to', iq.From );
			if ( iq.Id.length )
				dom.documentElement.setAttribute( 'id', iq.Id );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
	}

	/* Incoming file transfer request
	 */
	else if ( iq.Namespace == 'jabber:iq:oob' && iq.Type == 'set' )
	{
		var url = iq.XMLDOM.selectSingleNode( '/iq/query/url' );
		if ( ! url || ! external.globals( 'ClientRoster' ).Items.Exists( jid ) || external.globals( 'block' ).Exists( jid ) )
			url = '';
		else
			url = url.text;

		if ( ( url.substr( 0, 7 ) == 'http://' || url.substr( 0, 8 ) == 'https://' ) && url.length > 9 )
		{
			if ( external.windows.Exists( jid + '/' + iq.Id + '/file_recv' ) )
				external.windows( jid + '/' + iq.Id + '/file_recv' ).focus();
			else
				with ( external.createWindow( jid + '/' + iq.Id + '/file_recv', external.globals( 'cwd' ) + 'file_recv.html', new Array( jid, res, iq.From, iq.Id, iq ) ) )
				{
					setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_file_recv', [ external.globals( 'ClientRoster' ).Items( jid ).Name ] ) );
					setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\disk.ico' );
					resizeable( false );
					showMinBox( true );
					MinHeight = MinWidth = 0;
					setSize( 350, 190 );
					setPos( ( screen.availWidth - 350 ) / 2, ( screen.availHeight - 190 ) / 2 );
				}
		}
		else
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="error"><error code="400">This protocol is not supported.</error></iq>' );
			dom.documentElement.setAttribute( 'to', iq.From );
			if ( iq.Id.length )
				dom.documentElement.setAttribute( 'id', iq.Id );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
	}

	/* Answer with information or error for service discovery
	 */
	else if ( iq.Namespace == 'http://jabber.org/protocol/disco#info' && iq.Type == 'get' )
	{
		var Node = iq.XMLDOM.selectSingleNode( '/iq[@type="get"]/query[@xmlns="http://jabber.org/protocol/disco#info"]' );
		if ( Node.getAttributeNode( 'node' ) )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="error"><query xmlns="http://jabber.org/protocol/disco#info"><error code="404" type="cancel"><item-not-found xmlns="urn:ietf:xml:params:ns:xmpp-stanzas"/></error></query></iq>' );
			dom.documentElement.setAttribute( 'to', iq.From );
			if ( iq.Id.length )
				dom.documentElement.setAttribute( 'id', iq.Id );
			dom.documentElement.firstChild.setAttribute( 'node', Node.getAttribute( 'node' ) );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
		else
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="result"><query><identity category="client" type="pc"/></query></iq>' );
			dom.documentElement.setAttribute( 'to', iq.From );
			if ( iq.Id.length )
				dom.documentElement.setAttribute( 'id', iq.Id );
			var SupportedFeatures = [
				'http://jabber.org/protocol/disco#info',
				'http://jabber.org/protocol/muc',
				'http://jabber.org/protocol/xhtml-im',
				'jabber:iq:avatar',
				'jabber:iq:last',
				'jabber:iq:oob',
				'jabber:iq:time',
				'jabber:iq:version'
			];
			for ( var i = 0; i < SupportedFeatures.length; ++i )
			{
				var Feature = dom.createElement( 'feature' );
				Feature.setAttribute( 'var', SupportedFeatures[i] );
				dom.documentElement.firstChild.appendChild( Feature );
			}
			dom.documentElement.firstChild.setAttribute( 'xmlns', 'http://jabber.org/protocol/disco#info' );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
	}

	/* Unknown packet
	 */
	else if ( iq.Type == 'set' || iq.Type == 'get' )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="error"><error type="cancel" code="501"><feature-not-implemented xmlns="urn:ietf:xml:params:ns:xmpp-stanzas"/></error></iq>' );
		dom.documentElement.setAttribute( 'to', iq.From );
		if ( iq.Id.length )
			dom.documentElement.setAttribute( 'id', iq.Id );
		dom.documentElement.firstChild.setAttribute( 'xmlns', iq.From );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
}
