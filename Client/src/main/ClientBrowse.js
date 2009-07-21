/* Manages the services operated by the server at this address
 */
function ClientBrowseServer ()
{
	this.JID			= '';
	this.Name			= '';
	this.Admin			= false;
	this.AdminJ2		= '';
	this.Services		= new ActiveXObject( 'Scripting.Dictionary' );
	this.PendingDisco	= new ActiveXObject( 'Scripting.Dictionary' );

	this.Clear			= Clear;
	this.AddService		= AddService;
	this.FromIQDisco	= FromIQDisco;
	this.FromIQAgents	= FromIQAgents;
	this.FromIQBrowse	= FromIQBrowse;
	this.FromIQRoster	= FromIQRoster;
	this.FromPresence	= FromPresence;
	this.FindByOptions	= FindByOptions;

	/* Parse the XML with disco#info
	 */
	function FromIQDisco ( iq )
	{
		if ( iq.Type != 'result' )
			return;
		var jid = iq.FromAddress.ShortAddress();
		if ( ! this.Services.Exists( jid ) )
			this.Services.Add( jid, new ClientBrowseService( jid ) );

		var Identities = iq.XMLDOM.selectNodes( '/iq/*[@xmlns="http://jabber.org/protocol/disco#info"]/identity[@type and @category]' );

		for ( var i = 0; i < Identities.length; ++i )
		{
			if ( Identities.item(i).getAttribute( 'name' ) )
				this.Services( jid ).Name = Identities.item(i).getAttribute( 'name' );

			var IDCategory	= Identities.item(i).getAttribute( 'category' );
			var IDType		= Identities.item(i).getAttribute( 'type' );

			if ( IDCategory == 'gateway' )
			{
				switch ( IDType )
				{
					case 'aim':			this.Services( jid ).Options |= 0x0001 | 0x0008; break;
					case 'tlen':		this.Services( jid ).Options |= 0x0001 | 0x8000; break;
					case 'x-tlen':		this.Services( jid ).Options |= 0x0001 | 0x8000; break;
					case 'gadu-gadu':	this.Services( jid ).Options |= 0x0001 | 0x0800; break;
					case 'icq':			this.Services( jid ).Options |= 0x0001 | 0x0004; break;
					case 'msn':			this.Services( jid ).Options |= 0x0001 | 0x0002; break;
					case 'sms':			this.Services( jid ).Options |= 0x0001 | 0x2000; break;
					case 'smtp':		this.Services( jid ).Options |= 0x0001 | 0x1008; break;
					case 'yahoo':		this.Services( jid ).Options |= 0x0001 | 0x0010; break;
					default:			this.Services( jid ).Options |= 0x0001;
				}
			}
			else if ( IDCategory == 'conference' )
			{
				this.Services( jid ).Options |= 0x0020;

				if ( IDType == 'text' )
					this.Services( jid ).Options |= 0x0040 | 0x0080;
			}
		}

		if ( iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="http://jabber.org/protocol/disco#info"]/feature[@var="jabber:iq:search"]' ) )
			this.Services( jid ).Options |= 0x0100;

		if ( iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="http://jabber.org/protocol/disco#info"]/feature[@var="jabber:iq:register"]' ) )
			this.Services( jid ).Options |= 0x0200;
	}

	/* Parse the XML with jabber:iq:agents
	 */
	function FromIQAgents ( iq )
	{
		this.JID = this.Name = iq.FromAddress.ShortAddress();
		if ( iq.Type != 'result' )
			return;
		var agents = iq.XMLDOM.selectNodes( '/iq/*[@xmlns="jabber:iq:agents"]/agent[@jid]' );
		for ( var i = 0; i < agents.length; ++i )
		{
			var item	= agents.item(i);
			var jid		= item.getAttribute( 'jid' ).toLowerCase();
			if ( ! this.Services.Exists( jid ) )
				this.Services.Add( jid, new ClientBrowseService( jid ) );
			with ( this.Services( jid ) )
			{
				if ( item.selectSingleNode( 'name' ) )
					Name = item.selectSingleNode( 'name' ).text;
				if ( item.selectSingleNode( 'service' ) )
					switch ( item.selectSingleNode( 'service' ).text.toLowerCase() )
					{
						case 'msn':         Options |= 0x0001 | 0x0002; break;
						case 'icq':         Options |= 0x0001 | 0x0004; break;
						case 'aim':         Options |= 0x0001 | 0x0008; break;
						case 'yahoo':       Options |= 0x0001 | 0x0010; break;
						case 'gadugadu':    Options |= 0x0001 | 0x0800; break;
						case 'x-gadugadu':  Options |= 0x0001 | 0x0800; break;
						case 'tlen':		Options |= 0x0001 | 0x8000; break;
						case 'x-tlen':		Options |= 0x0001 | 0x8000; break;
						case 'smtp':		Options |= 0x0001 | 0x1000; break;
						case 'sms':			Options |= 0x0001 | 0x2000; break;
						case 'x-sms':		Options |= 0x0001 | 0x2000; break;
						case 'weather':		Options |= 0x0001 | 0x4000; break;
						case 'x-weather':	Options |= 0x0001 | 0x4000; break;
						case 'public':      Options |= 0x0020 | 0x0040; break;
						case 'private':     Options |= 0x0020 | 0x0080; break;
						case 'jud':         Options |= 0x0100; break;
						case 'users-agent': Options |= 0x0100; break;
					}
				if ( item.selectSingleNode( 'groupchat' ) )
					Options |= 0x0020 | 0x040;
				if ( item.selectSingleNode( 'search' ) )
					Options |= 0x0100;
				if ( item.selectSingleNode( 'register' ) )
					Options |= 0x0200;
			}
		}
	}

	/* Parse the XML with jabber:iq:browse
	 */
	function FromIQBrowse ( iq )
	{
		this.JID = this.Name = iq.FromAddress.ShortAddress();
		if ( iq.Type != 'result' )
			return;
		if ( iq.XMLDOM.selectSingleNode( '/iq/conference[@xmlns="jabber:iq:browse"]' ) )
			return;
		var server = iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns="jabber:iq:browse"]' );
		if ( ! server )
			return;
		if ( server.selectSingleNode( 'ns[. = "jabber:iq:admin"]' ) )
			this.Admin = true;
		this.Name = server.getAttribute( 'name' );
		var nodes = server.selectNodes( '*[@jid and @type]' );
		for ( var i = 0; i < nodes.length; ++i )
		{
			var item = nodes.item(i);
			var jid = item.getAttribute( 'jid' ).toLowerCase();
			if ( ! this.Services.Exists( jid ) )
				this.Services.Add( jid, new ClientBrowseService( jid ) );
			with ( this.Services( jid ) )
			{
				if ( item.getAttribute( 'name' ) )
					Name = item.getAttribute( 'name' );
				switch ( item.getAttribute( 'type' ).toLowerCase() )
				{
					case 'msn':			Options |= 0x0001 | 0x0002; break;
					case 'icq':			Options |= 0x0001 | 0x0004; break;
					case 'aim':			Options |= 0x0001 | 0x0008; break;
					case 'yahoo':		Options |= 0x0001 | 0x0010; break;
					case 'gadugadu':	Options |= 0x0001 | 0x0800; break;
					case 'x-gadugadu':	Options |= 0x0001 | 0x0800; break;
					case 'tlen':		Options |= 0x0001 | 0x8000; break;
					case 'x-tlen':		Options |= 0x0001 | 0x8000; break;
					case 'smtp':		Options |= 0x0001 | 0x1000; break;
					case 'sms':			Options |= 0x0001 | 0x2000; break;
					case 'x-sms':		Options |= 0x0001 | 0x2000; break;
					case 'weather':		Options |= 0x0001 | 0x4000; break;
					case 'x-weather':	Options |= 0x0001 | 0x4000; break;
					case 'public':		Options |= 0x0020 | 0x0040; break;
					case 'private':		Options |= 0x0020 | 0x0080; break;
					case 'jud':			Options |= 0x0100; break;
				}
				if ( item.getAttribute( 'category' ) == 'conference' )
					Options |= 0x0020;
				if ( item.selectSingleNode( 'ns[. = "jabber:iq:search"]' ) )
					Options |= 0x0100;
				if ( item.selectSingleNode( 'ns[. = "jabber:iq:register"]' ) )
					Options |= 0x0200;
				if ( item.selectSingleNode( 'ns[. = "jabber:iq:gateway"]' ) )
					Options |= 0x0001 | 0x0400;
			}
		}
	}

	/* Parse the transport's roster item
	 */
	function FromIQRoster ( contact )
	{
		var fulljid	= contact.getAttribute( 'jid' );
		var jid		= fulljid.toLowerCase();
		if ( jid.indexOf( '/' ) != -1 )
			jid = jid.substr( 0, jid.indexOf( '/' ) );
		if ( this.Services.Exists( jid ) && this.Services( jid ).Options & 0x001 )
		{
			var subscription = contact.getAttribute( 'subscription' );
			if ( subscription == 'remove' )
			{
				this.Services( jid ).TransportMode = 0;
				this.Services( jid ).TransportRosterJID = '';
			}
			else
			{
				this.Services( jid ).TransportRosterJID = fulljid;
				if ( this.Services( jid ).TransportMode == 0 && ( subscription == 'from' || subscription == 'both' ) )
					this.Services( jid ).TransportMode = -1;
			}
			if ( external.windows.Exists( 'transport_list' ) )
				external.windows( 'transport_list' ).Do( 'transport_refresh', jid );
		}
	}

	/* Parse the transport's presence
	 */
	function FromPresence ( presence )
	{
		var jid = presence.FromAddress.ShortAddress();
		if ( ! this.Services.Exists( jid ) )
			this.Services.Add( jid, new ClientBrowseService( jid ) );
		this.Services( jid ).Options |= 0x001;
		if ( presence.Type == 'error' )
			this.Services( jid ).TransportMode = -1;
		else if ( presence.Type == 'unavailable' )
			this.Services( jid ).TransportMode = 0;
		else
			this.Services( jid ).TransportMode = 1;
		if ( external.windows.Exists( 'transport_list' ) )
			external.windows( 'transport_list' ).Do( 'transport_refresh', jid );
	}

	/* Purge all data
	 */
	function Clear ()
	{
		this.JID		= '';
		this.Name		= '';
		this.Admin		= false;
		this.AdminJ2	= '';
		this.Services.RemoveAll();
	}

	/* Find any services at this server that have the required features
	 */
	function FindByOptions ( match )
	{
		var retval	= new Array();
		var jids	= ( new VBArray( this.Services.Keys() ) ).toArray();
		for ( var i = 0; i < jids.length; ++i )
			if ( ( this.Services( jids[i] ).Options & match ) == match )
				retval.push( this.Services( jids[i] ) );
		return retval;
	}

	/* Wrapper to add a new service
	 */
	function AddService ( jid, options )
	{
		if ( ! this.Services.Exists( jid ) )
		{
			this.Services.Add( jid, new ClientBrowseService( jid ) );
			this.Services( jid ).Options = options;
			if ( ( options & 0x0001 ) && external.windows.Exists( 'transport_list' ) )
				external.windows( 'transport_list' ).Do( 'transport_refresh', null );
		}
	}
}

/* What's this service capable of?
 */
function ClientBrowseService ( jid )
{
	this.JID		= jid;
	this.Name		= jid;
	this.Options	= 0x0000;
	/* 0x0001 - transport
	 * 0x0002 - msn
	 * 0x0004 - icq
	 * 0x0008 - aim
	 * 0x0010 - yahoo
	 * 0x0020 - conference
	 * 0x0040 - public
	 * 0x0080 - private
	 * 0x0100 - search
	 * 0x0200 - register
	 * 0x0400 - gateway
	 * 0x0800 - gadugadu
	 * 0x1000 - smtp
	 * 0x2000 - sms
	 * 0x4000 - weather
	 * 0x8000 - tlen
	 */

	/* Only registered transports should have this because they are added to the roster and represent the presence of the user on that network.
	 */
	this.TransportMode		= 0;
	this.TransportRosterJID	= '';
}
