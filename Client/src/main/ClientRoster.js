/* Save the avatar from VCard to disk and show it in the roster
 */
function ClientRosterAvatarVCard ( iq )
{
	var ShortAddress = iq.FromAddress.ShortAddress();
	if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
	{
		var RosterItem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
		var Resources = ( new VBArray( RosterItem.Resources.Keys() ) ).toArray();
		for ( var i = 0; i < Resources.length; ++i )
		{
			var RosterResource = RosterItem.Resources.Item( Resources[i] );
			if ( iq.Id == RosterResource.LoadingAvatarId )
			{
				var Node = null;
				if ( Node = iq.XMLDOM.selectSingleNode( '/iq[@type = "result"]/*[@xmlns = "vcard-temp"]/PHOTO/BINVAL' ) )
				{
					if ( RosterResource.LoadingAvatarHash.length == 40 && ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ) && Node.text.length && Node.text != 'AAAAAA==' )
					{
						external.File( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ).WriteBase64( Node.text );
						RosterResource.Avatar = RosterResource.LoadingAvatarHash;
						for ( var j = 0; j < RosterItem.Groups.length; ++j )
							RosterResource.Redraw( RosterItem.Roster.Groups.Item( RosterItem.Groups[j] ) );
						RosterItem.UpdateTracker();
					}
					RosterResource.LoadingAvatarHash	= '';
					RosterResource.LoadingAvatarId		= '';
					break;
				}
				else if ( Node = iq.XMLDOM.selectSingleNode( '/iq[@type = "result"]/*[@xmlns = "vcard-temp"]/PHOTO' ) )
				{
					if ( RosterResource.LoadingAvatarHash.length == 40 && ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ) && Node.text.length )
					{
						external.File( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ).WriteBase64( Node.text );
						RosterResource.Avatar = RosterResource.LoadingAvatarHash;
						for ( var j = 0; j < RosterItem.Groups.length; ++j )
							RosterResource.Redraw( RosterItem.Roster.Groups.Item( RosterItem.Groups[j] ) );
						RosterItem.UpdateTracker();
					}
					RosterResource.LoadingAvatarHash	= '';
					RosterResource.LoadingAvatarId		= '';
					break;
				}
				else
				{
					var hook		= new XMPPHookIQ();
					hook.Window		= external.wnd;
					hook.Callback	= 'ClientRosterAvatarStorage';
					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<iq type="get"><query xmlns="storage:client:avatar"/></iq>' );
					dom.documentElement.setAttribute( 'id', hook.Id );
					dom.documentElement.setAttribute( 'to', ShortAddress );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
					RosterResource.LoadingAvatarId = hook.Id;
					break;
				}
			}
		}
	}
}

/* Save the avatar from public storage to disk and show it in the roster
 */
function ClientRosterAvatarStorage ( iq )
{
	var ShortAddress = iq.FromAddress.ShortAddress();
	if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
	{
		var RosterItem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
		var Resources = ( new VBArray( RosterItem.Resources.Keys() ) ).toArray();
		for ( var i = 0; i < Resources.length; ++i )
		{
			var RosterResource = RosterItem.Resources.Item( Resources[i] );
			if ( iq.Id == RosterResource.LoadingAvatarId )
			{
				if ( iq.XMLDOM.selectSingleNode( '/iq[@type = "result"]/query[@xmlns = "storage:client:avatar"]/data' ) )
				{
					if ( RosterResource.LoadingAvatarHash.length == 40 && ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ) )
					{
						external.File( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ).WriteBase64( iq.XMLDOM.selectSingleNode( '/iq/query/data' ).text );
						RosterResource.Avatar = RosterResource.LoadingAvatarHash;
						for ( var j = 0; j < RosterItem.Groups.length; ++j )
							RosterResource.Redraw( RosterItem.Roster.Groups.Item( RosterItem.Groups[j] ) );
						RosterItem.UpdateTracker();
					}
					RosterResource.LoadingAvatarHash	= '';
					RosterResource.LoadingAvatarId		= '';
					break;
				}
				else
				{
					var hook		= new XMPPHookIQ();
					hook.Window		= external.wnd;
					hook.Callback	= 'ClientRosterAvatarDirect';
					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:avatar"/></iq>' );
					dom.documentElement.setAttribute( 'id', hook.Id );
					dom.documentElement.setAttribute( 'to', ShortAddress + '/' + Resources[i] );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
					RosterResource.LoadingAvatarId = '';
					break;
				}
			}
		}
	}
}

/* Save the avatar to disk and show it in the roster
 */
function ClientRosterAvatarDirect ( iq )
{
	if ( iq.Namespace == 'jabber:iq:avatar' && iq.Type == 'result' )
	{
		var ShortAddress = iq.FromAddress.ShortAddress();
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			var RosterItem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
			if (  RosterItem.Resources.Exists( iq.FromAddress.Resource ) )
			{
				var RosterResource = RosterItem.Resources.Item( iq.FromAddress.Resource );
				if ( RosterResource.LoadingAvatarHash.length == 40 && ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ) )
				{
					external.File( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ).WriteBase64( iq.XMLDOM.selectSingleNode( '/iq/query/data' ).text );
					RosterResource.Avatar = RosterResource.LoadingAvatarHash;
					for ( var i = 0; i < RosterItem.Groups.length; ++i )
						RosterResource.Redraw( RosterItem.Roster.Groups.Item( RosterItem.Groups[i] ) );
					RosterItem.UpdateTracker();
				}
				RosterResource.LoadingAvatarHash = '';
			}
		}
	} else if ( iq.Namespace == 'jabber:iq:avatar' && iq.Type == 'error' ) 
	{
		LoadAvatarFromVersion ( iq );
		/*try
		{
			var ShortAddress = iq.FromAddress.ShortAddress();
			var RosterItem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
			var RosterResource = RosterItem.Resources.Item( iq.FromAddress.Resource );
			var hook		= new XMPPHookIQ();
			hook.Window		= external.wnd;
			hook.Callback	= 'ClientRosterAvatarPubsub';
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="get"><pubsub xmlns="http://jabber.org/protocol/pubsub"><items node="urn:xmpp:avatar:data"></items></pubsub></iq>' );
			dom.documentElement.setAttribute( 'id', hook.Id );
			dom.documentElement.setAttribute( 'to', ShortAddress );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
			RosterResource.LoadingAvatarId = '';
		} catch (ex) { } */
	}
}

/* Load from version
*/
function LoadAvatarFromVersion ( iq ) {
	try 
	{
		var ShortAddress = iq.FromAddress.ShortAddress();
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			var RosterItem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
			var Resources = ( new VBArray( RosterItem.Resources.Keys() ) ).toArray();
			
			for ( var i = 0; i < Resources.length; ++i )
			{
				var RosterResource = RosterItem.Resources.Item( Resources[i] );
				RosterItem.LoadingVersion	= true;
				var hook					= new XMPPHookIQ();
				hook.Window					= external.wnd;
				hook.Callback				= 'ClientRosterVersion';
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:version"/></iq>' );
				dom.documentElement.setAttribute( 'id', hook.Id );
				dom.documentElement.setAttribute( 'to', ShortAddress + '/' + Resources[i] );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		}
	}
	catch (ex) {}
}

/* Get avatar from others
*/
function ClientRosterAvatarPubsub ( iq )
{
	if ( iq.Namespace == 'http://jabber.org/protocol/pubsub' && iq.Type == 'result' )
	{
		var ShortAddress = iq.FromAddress.ShortAddress();
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			var RosterItem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
			var Resources = ( new VBArray( RosterItem.Resources.Keys() ) ).toArray();
			for ( var i = 0; i < Resources.length; ++i )
			{
				var RosterResource = RosterItem.Resources.Item( Resources[i] );
				if ( RosterResource.LoadingAvatarHash.length == 40 && ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ) )
				{
					if ( iq.XMLDOM.selectSingleNode( '/iq/pubsub/items/item/data' ) ) {
						external.File( external.globals( 'usersdir' ) + 'Avatars\\' + RosterResource.LoadingAvatarHash ).WriteBase64( iq.XMLDOM.selectSingleNode( '/iq/pubsub/items/item/data' ).text );
						RosterResource.Avatar = RosterResource.LoadingAvatarHash;
						for ( var i = 0; i < RosterItem.Groups.length; ++i )
							RosterResource.Redraw( RosterItem.Roster.Groups.Item( RosterItem.Groups[i] ) );
						RosterItem.UpdateTracker();
					} 
					else 
					{
						LoadAvatarFromVersion ( iq );
					}
				}
				RosterResource.LoadingAvatarHash = '';
			}
		}
	} 
}

/* Enter new name in roster if previously it had only the address
 */
function ClientRosterVcard ( iq )
{
	var ShortAddress = iq.FromAddress.ShortAddress();
	if ( external.globals( 'ClientRoster' ).LoadingVcard.Exists( ShortAddress ) )
		external.globals( 'ClientRoster' ).LoadingVcard.Remove( ShortAddress );
	if ( iq.Namespace == 'vcard-temp' && iq.Type == 'result' &&
		external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) &&
		(
			external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name == iq.FromAddress.CleanAddress() ||
			external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name == ShortAddress
		)
	)
	{
		var vcard = new VCARD();
		vcard.FromIQ( iq );
		var nickname	= vcard.get( 'nickname' ).replace( /\s+/mg, ' ' );
		var fullname	= vcard.get( 'fn' ).replace( /\s+/mg, ' ' );
		var firstname	= vcard.get( 'given' ).replace( /\s+/mg, ' ' );
		var lastname	= vcard.get( 'family' ).replace( /\s+/mg, ' ' );
		var show = '';
		if ( fullname.length )
			show = fullname;
		else if ( firstname.length && lastname.length )
			show = firstname + ' ' + lastname;
		else if ( firstname.length )
			show = firstname;
		else if ( nickname.length )
			show = nickname;
		if ( show.length && show != external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name )
			external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).SetName( show );
	}
}

/* Update the avatar based on the software name
 */
function ClientRosterVersion ( iq )
{
	if ( iq.Namespace == 'jabber:iq:version' && iq.Type == 'result' )
	{
		var ShortAddress = iq.FromAddress.ShortAddress();
		var name = iq.XMLDOM.selectSingleNode( '/iq/query/name' );
		if ( ! name )
			return;
		name = name.text;
		if ( ! name.length )
			return;
		if ( ! external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
			return;
		var rosteritem = external.globals( 'ClientRoster' ).Items.Item( ShortAddress );
		if ( ! rosteritem.Resources.Exists( iq.FromAddress.Resource ) )
			return;
		var rosterresource = rosteritem.Resources.Item( iq.FromAddress.Resource );
		if ( rosterresource.Avatar.length )
			return;
		rosterresource.Avatar = external.globals( 'ClientRoster' ).GetAvatar( name, ShortAddress );
		if ( rosterresource.Avatar.length )
			for ( var i = 0; i < rosteritem.Groups.length; ++i )
				rosterresource.Redraw( rosteritem.Roster.Groups.Item( rosteritem.Groups[i] ) );
		rosteritem.UpdateTracker();
	}
}

/* Display right click menu for the contact list
 */
function ClientRosterContextMenu ()
{
	event.cancelBubble = true;

	var cfg			= external.globals( 'cfg' );
	var ViewSize	= cfg( 'contactlistdisplay' ) == 'detailed';

	var Display = external.newPopupMenu;
	Display.AddItem( true, false,   ViewSize, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_detailed' ),			30 );
	Display.AddItem( true, false, ! ViewSize, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_compact' ),			31 );

	var Menu = external.newPopupMenu;
	Menu.AddItem( true, false, false, false, 0,					external.globals( 'Translator' ).Translate( 'main', 'menu_axn_add' ),		1 );
	Menu.AddItem( true, false, false, false, 0,					external.globals( 'Translator' ).Translate( 'main', 'menu_axn_addgroup' ),	2 );
	Menu.AddSeparator();
	Menu.AddItem( true, false, false, false, Display.Handle,	external.globals( 'Translator' ).Translate( 'main', 'menu_tool_view' ),		3 );
	Menu.AddSeparator();
	Menu.AddItem( true, false, false, false, 0,					external.globals( 'Translator' ).Translate( 'main', 'menu_tool_settings' ),	4 );
	Menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );

	switch ( Menu.Choice )
	{
		case 1: // add user
			dial_adduser();			break;
		case 2: // add group
			dial_group_create();	break;
		case 4: // settings
			dial_preferences( '' );	break;
		case 30: // detailed
			if ( cfg( 'contactlistdisplay' ) == 'compact' )
			{
				cfg( 'contactlistdisplay' ) = 'detailed';
				var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
				for ( var i = 0; i < TrackerNames.length; ++i )
					external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
				external.globals( 'ClientRoster' ).RefreshAll();
			}
			MenuBarUpdate( 'tools' );
		break;
		case 31: // compact
			if ( cfg( 'contactlistdisplay' ) == 'detailed' )
			{
				cfg( 'contactlistdisplay' ) = 'compact';
				var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
				for ( var i = 0; i < TrackerNames.length; ++i )
					external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
				external.globals( 'ClientRoster' ).RefreshAll();
			}
			MenuBarUpdate( 'tools' );
		break;
	}
}

/* This stores the contact list in a hierarchical structure.
 *
 * ClientRoster > ClientRosterGroup > ClientRosterItem > ClientRosterResource
 *
 * The ClientRoster holds a dictionary of ClientRosterGroups and
 * ClientRosterItems.  Every ClientRosterGroup holds a Dictionary
 * of the addresses of the contacts that are members of that 
 * group.  Each ClientRosterItem has a Dictionary of its resources.
 */
function ClientRoster ()
{
	this.Items				= new ActiveXObject( 'Scripting.Dictionary' );
	this.Groups				= new ActiveXObject( 'Scripting.Dictionary' );
	this.IsFlashing			= false;
	this.HTMLElement		= document.getElementById( 'rostercell' );
	this.LoadingVcard		= new ActiveXObject( 'Scripting.Dictionary' );
	this.DragNDropOrigin	= '';

	this.Clear				= Clear;
	this.Search				= new ClientRosterSearch();
	this.FromIQ				= FromIQ;
	this.GetAvatar			= GetAvatar;
	this.AllOffline			= AllOffline;
	this.RefreshAll			= RefreshAll;
	this.CreateGroup		= CreateGroup;
	this.ReloadFromIQ		= ReloadFromIQ;
	this.ReceivePresence	= ReceivePresence;
	this.ReceiveSetFromIQ	= ReceiveSetFromIQ;

	/* Load the default avatar mapping.
	 */
	var AvatarMap = new ActiveXObject( 'Scripting.Dictionary' );
	AvatarMap.Add( 'adium',				'adium'			);
	AvatarMap.Add( 'bot',				'cog'			);
	AvatarMap.Add( 'brim',				'brim'			);
	AvatarMap.Add( 'buddyspace',		'buddyspace'	);
	AvatarMap.Add( 'desktop',			'desktop'		);
	AvatarMap.Add( 'centericq',			'console'		);
	AvatarMap.Add( 'chatopus',			'palm'			);
	AvatarMap.Add( 'everybuddy',		'mac'			);
	AvatarMap.Add( 'exodus',			'exodus'		);
	AvatarMap.Add( 'gabber',			'gnome'			);
	AvatarMap.Add( 'gaim',				'gaim'			);
	AvatarMap.Add( 'gajim',				'gajim'			);
	AvatarMap.Add( 'gmail',				'googletalk'	);
	AvatarMap.Add( 'greenthumb',		'thumb'			);
	AvatarMap.Add( 'gossip',			'gnome'			);
	AvatarMap.Add( 'gush',				'gush'			);
	AvatarMap.Add( 'hapi',				'hapi'			);
	AvatarMap.Add( 'home',				'house'			);
	AvatarMap.Add( 'ichat',				'mac'			);
	AvatarMap.Add( 'imagent',			'mac'			);
	AvatarMap.Add( 'astrachat',			'astrachat'		);
	AvatarMap.Add( 'imcom',				'console'		);
	AvatarMap.Add( 'imendio',			'gnome'			);
	AvatarMap.Add( 'imov',				'imov'			);
	AvatarMap.Add( 'imservices-libgaim','gaim'			);
	AvatarMap.Add( 'jabber.net',		'dotnet'		);
	AvatarMap.Add( 'jabber messenger',	'jim'			);
	AvatarMap.Add( 'jabberce',			'imov'			);
	AvatarMap.Add( 'jabbermessenger',	'jim'			);
	AvatarMap.Add( 'jabbernaut',		'mac'			);
	AvatarMap.Add( 'jabberwocky',		'amiga'			);
	AvatarMap.Add( 'jabbix',			'jabbix'		);
	AvatarMap.Add( 'jarl',				'jarl'			);
	AvatarMap.Add( 'jajc',				'jajc'			);
	AvatarMap.Add( 'jitsi',				'jitsi'			);
	AvatarMap.Add( 'jbother',			'jbother'		);
	AvatarMap.Add( 'just another',		'jajc'			);
	AvatarMap.Add( 'jwchat',			'jwchat'		);
	AvatarMap.Add( 'kopete',			'kopete'		);
	AvatarMap.Add( 'konnekt',			'konnekt'		);
	AvatarMap.Add( 'laptop',			'laptop'		);
	AvatarMap.Add( 'libgaim',			'gaim'			);
	AvatarMap.Add( 'miranda',			'miranda'		);
	AvatarMap.Add( 'monal',				'monal'			);
	AvatarMap.Add( 'myjab',				'myjabber'		);
	AvatarMap.Add( 'mobile',			'sms'			);
	AvatarMap.Add( 'mcenter',			'neosmt'		);
	AvatarMap.Add( 'neos',				'neosmt'		);
	AvatarMap.Add( 'nitro',				'nitro'			);
	AvatarMap.Add( 'notebook',			'laptop'		);
	AvatarMap.Add( 'office',			'work'			);
	AvatarMap.Add( 'palm',				'palm'			);
	AvatarMap.Add( 'pandion',			'pandion'		);
	AvatarMap.Add( 'papla',				'papla'			);
	AvatarMap.Add( 'pidgin',			'pidgin'		);
	AvatarMap.Add( 'pocket',			'palm'			);
	AvatarMap.Add( 'psi',				'psi'			);
	AvatarMap.Add( 'punjab',			'punjab'		);
	AvatarMap.Add( 'rhymbox',			'rhymbox'		);
	AvatarMap.Add( 'rival',				'rival'			);
	AvatarMap.Add( 'sapo',				'sapo'			);
	AvatarMap.Add( 'shaolo',			'shaolo'		);
	AvatarMap.Add( 'simplesend',		'jarl'			);
	AvatarMap.Add( 'skabber',			'skabber'		);
	AvatarMap.Add( 'smack',				'smack'			);
	AvatarMap.Add( 'soapbox',			'soapbox'		);
	AvatarMap.Add( 'spark',				'spark'			);
	AvatarMap.Add( 'talk',				'googletalk'	);
	AvatarMap.Add( 'tipicim',			'tipicim'		);
	AvatarMap.Add( 'tipicme',			'tipicme'		);
	AvatarMap.Add( 'tkabber',			'tkabber'		);
	AvatarMap.Add( 'tkcjabber',			'cog'			);
	AvatarMap.Add( 'tlen',				'tlen'			);
	AvatarMap.Add( 'trillian',			'trillian'		);
	AvatarMap.Add( 'tvjab',				'mac'			);
	AvatarMap.Add( 'vista',				'vista'			);
	AvatarMap.Add( 'weather',			'weather'		);
	AvatarMap.Add( 'webmsgr',			'webmessenger'	);
	AvatarMap.Add( 'work',				'work'			);
	AvatarMap.Add( 'wpkontakt',			'kontakt'		);
	AvatarMap.Add( 'wxskabber',			'skabber'		);
	AvatarMap.Add( 'xmppimcom',			'console'		);
	AvatarMap.Add( 'yabber',			'yabber'		);

	/* Redraw all contacts
	 */
	function RefreshAll ()
	{
		var itemnames = ( new VBArray( this.Items.Keys() ) ).toArray();
		for ( var i = 0; i < itemnames.length; ++i )
			this.Items.Item( itemnames[i] ).RefreshAll();
	}

	/* Parses the XML in search of an avatar
	 * Returns the relative path to the avatar file
	 */
	function GetAvatar ( str, jid )
	{
		str = str.toLowerCase();

		/* Default avatar mapping
		 */
		for ( var i = str.length; i > 1; i-- )
			if ( AvatarMap.Exists( str.substr( 0, i ) ) )
				return AvatarMap.Item( str.substr( 0, i ) ) + '.gif';

		/* Transport contact
		 */
		var domain = jid.indexOf( '@' ) > -1 ? jid.substr( jid.indexOf( '@' ) + 1 ) : jid;
		if ( external.globals( 'ClientServices' ).Services.Exists( domain ) && external.globals( 'ClientServices' ).Services.Item( domain ).Options & 0x0001 )
			switch ( external.globals( 'ClientServices' ).Services.Item( domain ).Options & 0xF81E )
			{
				case 0x0002: return 'msn.gif';
				case 0x0004: return 'icq.gif';
				case 0x0008: return 'aim.gif';
				case 0x0010: return 'yahoo.gif';
				case 0x0800: return 'gadugadu.gif';
				case 0x1000: return 'email.gif';
				case 0x2000: return 'sms.gif';
				case 0x4000: return 'weather.gif';
				case 0x8000: return 'tlen.gif';
			}

		/* Guess the transport and add it to the services list
		 */
		if ( domain.charAt( 3 ) == '.' )
			switch ( domain.substr( 0, 3 ) )
			{
				case 'msn': external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x0002 ); return 'msn.gif';
				case 'icq': external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x0004 ); return 'icq.gif';
				case 'aim': external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x0008 ); return 'aim.gif';
				case 'sms': external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x2000 ); return 'sms.gif';
				case 'yim': external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x0010 ); return 'yahoo.gif';
			}
		if ( domain.substr( 0, 3 ) == 'gg.' )
		{
			external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x0800 );
			return 'gadugadu.gif';
		}
		if ( domain.substr( 0, 5 ) == 'tlen.' )
		{
			external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x8000 );
			return 'tlen.gif';
		}
		if ( domain.substr( 0, 6 ) == 'email.' )
		{
			external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x1000 );
			return 'email.gif';
		}
		if ( domain.substr( 0, 6 ) == 'yahoo.' )
		{
			external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x0010 );
			return 'yahoo.gif';
		}
		if ( domain.substr( 0, 7 ) == 'aspsms.' )
		{
			external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x2000 );
			return 'sms.gif';
		}
		if ( domain.substr( 0, 8 ) == 'weather.' )
		{
			external.globals( 'ClientServices' ).AddService( domain, 0x0001 | 0x4000 );
			return 'weather.gif';
		}

		/* Generic service icon
		 */
		if ( jid.indexOf( '/' ) == -1 && str.length == 0 )
			return 'cog.gif';

		return '';
	}

	/* Send fake presence packets to the rosteritems so they all go offline
	 */
	function AllOffline ()
	{
		var itemnames = ( new VBArray( this.Items.Keys() ) ).toArray();
		var presence = new XMPPPresence();
		presence.Type = 'unavailable';
		for ( var i = 0; i < itemnames.length; ++i )
			if ( this.Items.Item( itemnames[i] ).Resources.Count )
			{
				var theItem = this.Items.Item( itemnames[i] );
				var resources = ( new VBArray( theItem.Resources.Keys() ) ).toArray();
				for ( var j = 0; j < resources.length; ++j )
					theItem.ReceivePresence( theItem.JID, resources[j], presence );
			}
	}

	/* Remove all groups, items and resources from the roster
	 */
	function Clear ()
	{
		var itemnames = ( new VBArray( this.Items.Keys() ) ).toArray();
		for ( var i = 0; i < itemnames.length; ++i )
			this.Items.Item( itemnames[i] ).Clear();
		this.Items.RemoveAll();
		var groupnames = ( new VBArray( this.Groups.Keys() ) ).toArray();
		for ( var i = 0; i < groupnames.length; ++i )
			this.Groups.Item( groupnames[i] ).Clear();
		this.Groups.RemoveAll();
	}

	/* Parses Presence objects
	 */
	function ReceivePresence ( Presence )
	{
		var ShortAddress = Presence.FromAddress.ShortAddress();
		if ( external.globals( 'ClientServices' ).Services.Exists( ShortAddress ) || external.globals( 'ClientServices' ).PendingDisco.Exists( ShortAddress ) )
			external.globals( 'ClientServices' ).FromPresence( Presence );
		else if ( this.Items.Exists( ShortAddress ) )
		{
			if ( Presence.Type != 'error' )
			{
				/* Play notification sounds and show toaster
				 */
				if ( external.globals( 'cfg' ).Item( 'lastmode' ) < 2 || external.globals( 'cfg' ).Item( 'lastmode' ) == 5 )
					for ( var i = 0; i < this.Items.Item( ShortAddress ).Groups.length; i++ )
						if ( this.Groups.Item( this.Items.Item( ShortAddress ).Groups[i] ).ShowAll )
						{
							if ( Presence.Type == 'unavailable' && external.globals( 'cfg' ).Item( 'soundoffline' ).toString() == 'true' && this.Items.Item( ShortAddress ).Resources.Count )
								sound_play( external.globals( 'cfg' ).Item( 'soundofflinefile' ), false );
							else if ( Presence.Type == 'available' && external.globals( 'cfg' ).Item( 'soundonline' ).toString() == 'true' && ! this.Items.Item( ShortAddress ).Resources.Count )
								sound_play( external.globals( 'cfg' ).Item( 'soundonlinefile' ), false );
							if ( Presence.Type == 'available' && external.globals( 'cfg' ).Item( 'alertonline' ).toString() == 'true' && ! this.Items.Item( ShortAddress ).Resources.Count && external.globals( 'connecttime' ) + 30000 < ( new Date() ).getTime() )
							{
								var Toaster					= new Headline();
								Toaster.Archive				= false;
								Toaster.ShowOptions			= true;
								Toaster.OpenConversation	= Presence.FromAddress;
								Toaster.Address				= Presence.FromAddress;
								Toaster.Title				= this.Items.Item( ShortAddress ).Name;
								Toaster.Message				= external.globals( 'Translator' ).Translate( 'main', 'user_online', [ this.Items.Item( ShortAddress ).Name ] );
								Toaster.Show();
							}
							break;
						}
				/* Pass the presence on to the item
				 */
				this.Items.Item( ShortAddress ).ReceivePresence( ShortAddress, Presence.FromAddress.Resource, Presence );
			}
		}
	}

	/* Private function that does the grunt work of updating the roster exactly as specified
	 */
	function ParseXMLItem ( roster, jid, contact )
	{
		var name = contact.getAttribute( 'name' );
		var subscription = contact.getAttribute( 'subscription' );
		var ask = contact.getAttribute( 'ask' );

		/* none:  I can't see him  +  he can't see me   ->  Unknown Status
		 * from:  I can't see him  +  he can see me     ->  Unknown Status
		 *   to:  I can see him    +  he can't see me   ->  Offline
		 * both:  I can see him    +  he can see me     ->  Offline
		 *
		 *    subscription="none"  +  ask="subscribe"   ->  Awaiting Authorization
		 *    subscription="from"  +  ask="subscribe"   ->  Awaiting Authorization
		 */

		/* Delete the item
		 */
		if ( subscription == 'remove' )
		{
			if ( roster.Items.Exists( jid ) )
				roster.Items.Item( jid ).Clear();
			if ( roster.Items.Exists( jid ) )
				roster.Items.Item( jid ).UpdateTracker();
			return;
		}

		if ( ! name || name == jid )
		{
			name = ( new XMPPAddress( jid ) ).CleanAddress();
			/* Retrieve the nickname from the JUD
			 */
			if ( ! roster.LoadingVcard.Exists( jid ) )
			{
				roster.LoadingVcard.Add( jid, null );
				var hook		= new XMPPHookIQ();
				hook.Window		= external.wnd;
				hook.Callback	= 'ClientRosterVcard';
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<iq type="get"><vCard xmlns="vcard-temp"/></iq>' );
				dom.documentElement.setAttribute( 'id', hook.Id );
				dom.documentElement.setAttribute( 'to', jid );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		}

		/* Add the item to its rostergroups and draw them
		 */
		if ( ! roster.Items.Exists( jid ) )
		{
			/* Get a list of all groups this contact is in, or add to default group
			 */
			var groupnodes = contact.selectNodes( 'group' );
			var groups = new Array();
			for ( var j = 0; j < groupnodes.length; ++j )
			{
				var groupname = groupnodes.item( j ).text;
				if ( groupname == 'Groupless' || groupname == 'General' || groupname == 'Unfiled' || groupname == 'Contacts' )
					groupname = external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' );
				if ( groupname.length )
					groups.push( groupname );
			}
			if ( ! groups.length )
				groups.push( external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' ) );
			/* Draw the offline item in the groups
			 */
			for ( var j = 0; j < groups.length; ++j )
			{
				with ( roster.CreateGroup( groups[j] ).CreateItem( jid ) )
				{
					Name = name;
					Subscription = subscription;
					Ask = ask;

					var GroupExists = false;
					for ( var k = 0; k < Groups.length; ++k )
						if ( Groups[k] == groups[j] )
						{
							GroupExists = true;
							break;
						}
					if ( ! GroupExists )
						Groups.push( groups[j] );

					if ( roster.Groups.Item( groups[j] ).Items.Count == 1 )
						roster.Groups.Item( groups[j] ).Draw();
					else if ( roster.Groups.Item( groups[j] ).ShowOffline )
						Draw( roster.Groups.Item( groups[j] ) );
					/* Hide the "display offline contacts" button
					 */
					var itemnames = ( new VBArray( roster.Groups.Item( groups[j] ).Items.Keys() ) ).toArray();
					var o;
					for ( o = 0; o < itemnames.length; ++o )
						if ( ! roster.Items.Item( itemnames[o] ).Resources.Count )
							break;
					if ( o < itemnames.length )
						roster.Groups.Item( groups[j] ).HTMLShowAll.style.display = 'inline';
				}
			}
		}

		/* Check its groups and redraw the groups, items or resources
		 */
		else if ( subscription == 'to' || subscription == 'both' )
		{
			var theItem = roster.Items.Item( jid );
			/* Get a new list of all groups this contact is in, or add to default group
			 */
			var newgroups = new ActiveXObject( 'Scripting.Dictionary' );
			var groupnodes = contact.selectNodes( 'group' );
			for ( var l = 0; l < groupnodes.length; ++l )
			{
				var groupname = groupnodes.item( l ).text;
				if ( groupname == 'Groupless' || groupname == 'General' || groupname == 'Unfiled' || groupname == 'Contacts' )
					groupname = external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' );
				if ( groupname.length && ! newgroups.Exists( groupname ) )
					newgroups.Add( groupname, null );
			}
			if ( ! newgroups.Count )
				newgroups.Add( external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' ), null );
			/* Delete the item from any groups it is no longer part of
			 */
			for ( var m = 0; m < theItem.Groups.length; ++m )
				if ( ! newgroups.Exists( theItem.Groups[m] ) )
				{
					var theGroup = roster.Groups.Item( theItem.Groups[m] );
					if ( theItem.Resources.Count )
					{
						/* Remove online resources
						 */
						var resources = ( new VBArray( theItem.Resources.Keys() ) ).toArray();
						for ( var o = 0; o < resources.length; ++o )
							theItem.Resources.Item( resources[o] ).Hide( theGroup );
						theGroup.Items.Remove( jid );
 						if ( theGroup.Items.Count )
							theGroup.HTMLOnline.firstChild.style.display = theGroup.HTMLOnline.children.length > 1 ? 'none' : 'block';
						else
						{
							theGroup.Clear();
							roster.Groups.Remove( theItem.Groups[m] );
						}
					}
					else
					{
						/* Remove offline item
						 */
						theItem.Hide( theGroup );
						theGroup.Items.Remove( jid );
						if ( ! theGroup.Items.Count )
						{
							theGroup.Clear();
							roster.Groups.Remove( theItem.Groups[m] );
						}
						else
						{
							/* Hide the "display offline contacts" button
							 */
							var itemnames = ( new VBArray( theGroup.Items.Keys() ) ).toArray();
							var q;
							for ( q = 0; q < itemnames.length; ++q )
								if ( ! roster.Items.Item( itemnames[q] ).Resources.Count )
									break;
							if ( q >= itemnames.length )
								theGroup.HTMLShowAll.style.display = 'none';
						}
					}
				}
			/* Show the rosteritem or its resources in the new list of groups
			 */
			theItem.Name = name;
			theItem.Subscription = subscription;
			theItem.Ask = ask;
			theItem.Groups = ( new VBArray( newgroups.Keys() ) ).toArray();
			for ( var n = 0; n < theItem.Groups.length; ++n )
			{
				var theGroup = roster.CreateGroup( theItem.Groups[n] );
				if ( ! theGroup.HTMLHeader.className )
					theGroup.Draw();
				if ( theItem.Resources.Count )
				{
					/* Resources
					 */
					if ( ! theGroup.Items.Exists( jid ) )
						theGroup.Items.Add( jid, null );
					var resources = ( new VBArray( theItem.Resources.Keys() ) ).toArray();
					for ( var p = 0; p < resources.length; ++p )
					{
						theResource = theItem.Resources.Item( resources[p] );
						if ( theResource.HTMLElements.Exists( theGroup.Name ) )
							theResource.Redraw( theGroup );
						else
						{
							theResource.Draw( theGroup );
							/* Hide the "no one is online" text
							 */
							theGroup.HTMLOnline.firstChild.style.display = 'none';
							/* Hide the "display offline contacts" button
							 */
							var itemnames = ( new VBArray( theGroup.Items.Keys() ) ).toArray();
							var q;
							for ( q = 0; q < itemnames.length; ++q )
								if ( ! roster.Items.Item( itemnames[q] ).Resources.Count )
									break;
							if ( q >= itemnames.length )
								theGroup.HTMLShowAll.style.display = 'none';
						}
					}
				}
				else
				{
					/* Offline item
					 */
					if ( theGroup.Items.Exists( jid ) )
					{
						if ( theItem.HTMLElements.Exists( theGroup.Name ) )
							theItem.Update( theGroup );
						else if ( theGroup.ShowOffline )
							theItem.Draw( theGroup );
					}
					else if ( theGroup.ShowOffline )
					{
						theGroup.Items.Add( jid, null );
						if ( theGroup.ShowOffline )
							theItem.Draw( theGroup );
					}
					else
						theGroup.Items.Add( jid, null );
					if ( theGroup.ShowAll )
						theGroup.HTMLShowAll.style.display = 'inline';
				}
			}
		}

		/* Make it go offline with lurker|awaiting status
		 */
		else
		{
			var theItem = roster.Items.Item( jid );
			/* Delete the resources.
			 */
			var resources = ( new VBArray( theItem.Resources.Keys() ) ).toArray();
			for ( var j = 0; j < resources.length; ++j )
				theItem.Resources.Item( resources[j] ).Clear();
			theItem.Resources.RemoveAll();
			/* Get a new list of all groups this contact is in, or add to default group
			 */
			var newgroups = new ActiveXObject( 'Scripting.Dictionary' );
			var groupnodes = contact.selectNodes( 'group' );
			for ( var l = 0; l < groupnodes.length; ++l )
			{
				var groupname = groupnodes.item( l ).text;
				if ( groupname == 'Groupless' || groupname == 'General' || groupname == 'Unfiled' || groupname == 'Contacts' )
					groupname = external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' );
				if ( groupname.length && ! newgroups.Exists( groupname ) )
					newgroups.Add( groupname, null );
			}
			if ( ! newgroups.Count )
				newgroups.Add( external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' ), null );
			/* Delete the item from any groups it is no longer part of
			 */
			for ( var m = 0; m < theItem.Groups.length; ++m )
				if ( ! newgroups.Exists( theItem.Groups[m] ) )
				{
					var theGroup = roster.Groups.Item( theItem.Groups[m] );
					theItem.Hide( theGroup );
					theGroup.Items.Remove( jid );
					if ( ! theGroup.Items.Count )
					{
						/* Remove the group if it's empty
						 */
						theGroup.Clear();
						roster.Groups.Remove( theItem.Groups[m] );
					}
					else
					{
						/* Hide or show the "no one is online" text
						 */
						theGroup.HTMLOnline.firstChild.style.display = theGroup.HTMLOnline.children.length > 1 ? 'none' : 'block';
						/* Hide the "display offline contacts" button
						 */
						var itemnames = ( new VBArray( theGroup.Items.Keys() ) ).toArray();
						var o;
						for ( o = 0; o < itemnames.length; ++o )
							if ( ! roster.Items.Item( itemnames[o] ).Resources.Count )
								break;
						if ( o >= itemnames.length )
							theGroup.HTMLShowAll.style.display = 'none';
					}
				}
			/* Show the rosteritem in the new list of groups
			 */
			theItem.Name = name ? name : jid;
			theItem.Subscription = subscription;
			theItem.Ask = ask;
			theItem.Groups = ( new VBArray( newgroups.Keys() ) ).toArray();
			for ( var n = 0; n < theItem.Groups.length; ++n )
			{
				var theGroup = roster.CreateGroup( theItem.Groups[n] );
				if ( ! theGroup.HTMLHeader.className )
					theGroup.Draw();
				if ( theGroup.Items.Exists( jid ) )
				{
					if ( theItem.HTMLElements.Exists( theGroup.Name ) )
						theItem.Update( theGroup );
					else if ( theGroup.ShowOffline )
						theItem.Draw( theGroup );
				}
				else
				{
					theGroup.Items.Add( jid, null );
					if ( theGroup.ShowOffline )
						theItem.Draw( theGroup );
				}
				if ( theGroup.ShowAll )
					theGroup.HTMLShowAll.style.display = 'inline';
			}
		}
		/* Update conversation window
		 */
		if ( roster.Items.Exists( jid ) )
			roster.Items.Item( jid ).UpdateTracker();
	}

	/* Parses roster-updates
	 */
	function ReceiveSetFromIQ ( iq )
	{
		var contacts = iq.XMLDOM.selectNodes( '/iq/query/item[@jid]' );
		for ( var i = 0; i < contacts.length; i++ )
		{
			var contact = contacts.item(i);
			var jid = contact.getAttribute( 'jid' ).toLowerCase();
			if ( contact.getAttribute( 'subscription' ) != 'remove' && ( ! jid.length || jid.indexOf( '/' ) != -1 || external.globals( 'ClientServices' ).PendingDisco.Exists( jid ) || ( external.globals( 'ClientServices' ).Services.Exists( jid ) && ( external.globals( 'ClientServices' ).Services.Item( jid ).Options & 0x001 == 0x001 ) ) ) )
				external.globals( 'ClientServices' ).FromIQRoster( contact );
			else
				ParseXMLItem( this, jid, contact );
		}
	}

	/* Update the roster according to the new jabber:iq:roster result
	 */
	function ReloadFromIQ ( iq )
	{
		var oldjids = ( new VBArray( this.Items.Keys() ) ).toArray();
		var newjids = new ActiveXObject( 'Scripting.Dictionary' );
		var contacts = iq.XMLDOM.selectNodes( '/iq/query/item[@jid]' );
		for ( var i = 0; i < contacts.length; i++ )
		{
			var jid = contacts.item(i).getAttribute( 'jid' ).toLowerCase();
			if ( ! jid.length || jid.indexOf( '/' ) != -1 || external.globals( 'ClientServices' ).PendingDisco.Exists( jid ) || ( external.globals( 'ClientServices' ).Services.Exists( jid ) && ( external.globals( 'ClientServices' ).Services.Item( jid ).Options & 0x001 == 0x001 ) ) )
			{
				external.globals( 'ClientServices' ).FromIQRoster( contacts.item(i) );
				continue;
			}
			ParseXMLItem( this, jid, contacts.item(i) );
			if ( ! newjids.Exists( jid ) )
				newjids.Add( jid, null );
		}
		for ( var j = 0; j < oldjids.length; ++j )
			if ( ! newjids.Exists( oldjids[j] ) )
				this.Items.Item( oldjids[j] ).Clear();
	}

	/* Parses IQ packets
	 * Returns true or false, depending on whether the packet contained a valid roster
	 */
	function FromIQ ( iq )
	{
		var items = iq.XMLDOM.selectNodes( '/iq/query/item[@jid]' );
		for ( var i = 0; i < items.length; ++i )
		{
			var contact = items.item( i );
			var jid = contact.getAttribute( 'jid' ).toLowerCase();
			if ( ! jid.length || jid.indexOf( '/' ) != -1 || external.globals( 'ClientServices' ).PendingDisco.Exists( jid ) || ( external.globals( 'ClientServices' ).Services.Exists( jid ) && ( external.globals( 'ClientServices' ).Services.Item( jid ).Options & 0x001 == 0x001 ) ) )
			{
				external.globals( 'ClientServices' ).FromIQRoster( contact );
				continue;
			}
			/* Get a list of all groups this contact is in, or add to default group
			 */
			var groupnodes = contact.selectNodes( 'group' );
			var groups = new ActiveXObject( 'Scripting.Dictionary' );
			for ( var j = 0; j < groupnodes.length; ++j )
			{
				var groupname = groupnodes.item( j ).text;
				if ( groupname == 'Groupless' || groupname == 'General' || groupname == 'Unfiled' || groupname == 'Contacts' )
					groupname = external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' );
				if ( groupname.length && ! groups.Exists( groupname ) )
					groups.Add( groupname, null );
			}
			groups = ( new VBArray( groups.Keys() ) ).toArray();
			if ( ! groups.length )
				groups.push( external.globals( 'Translator' ).Translate( 'main', 'cl_default_group' ) );
			/* Create the necessary ClientRosterGroup(s) and ClientRosterItem(s)
			 */
			var name = contact.getAttribute( 'name' );
			if ( ! name || name == jid )
			{
				name = ( new XMPPAddress( jid ) ).CleanAddress();
				/* Retrieve the nickname from the JUD
				 */
				if ( ! this.LoadingVcard.Exists( jid ) )
				{
					this.LoadingVcard.Add( jid, null );
					var hook		= new XMPPHookIQ();
					hook.Window		= external.wnd;
					hook.Callback	= 'ClientRosterVcard';
					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<iq type="get"><vCard xmlns="vcard-temp"/></iq>' );
					dom.documentElement.setAttribute( 'id', hook.Id );
					dom.documentElement.setAttribute( 'to', jid );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}
			}
			var subscription = contact.getAttribute( 'subscription' );
			var ask = contact.getAttribute( 'ask' );
			for ( var j = 0; j < groups.length; ++j )
				with ( this.CreateGroup( groups[j] ).CreateItem( jid ) )
				{
					Name = name;
					Subscription = subscription;
					Ask = ask;

					var GroupExists = false;
					for ( var k = 0; k < Groups.length; ++k )
						if ( Groups[k] == groups[j] )
						{
							GroupExists = true;
							break;
						}
					if ( ! GroupExists )
						Groups.push( groups[j] );
				}
		}
		/* Show the roster in the HTML
		 */
		var groupnames = ( new VBArray( this.Groups.Keys() ) ).toArray().sort( function(a,b){var c=a.toLowerCase();var d=b.toLowerCase();return c==d?0:c<d?-1:1} );
		for ( var i = 0; i < groupnames.length; ++i )
			this.Groups.Item( groupnames[i] ).Draw();

		return this.Items.Count > 0;
	}

	/* Return a new ClientRosterGroup object
	 * If there is already a group with that name, the current ClientRosterGroup object is returned
	 */
	function CreateGroup ( name )
	{
		if ( ! this.Groups.Exists( name ) )
			this.Groups.Add( name, new ClientRosterGroup( this, name ) );
		return this.Groups.Item( name );
	}
}

/* Holds a set of items from the contact list
 */
function ClientRosterGroup ( roster, name )
{
	this.Name			= name;
	this.Items			= new ActiveXObject( 'Scripting.Dictionary' );
	this.Roster			= roster;
	this.ShowAll		= true;
	this.ShowOffline	= false;

	this.Draw			= Draw;
	this.Clear			= Clear;
	this.SetName		= SetName;
	this.ChangeName		= ChangeName;
	this.CreateItem		= CreateItem;
	this.ToggleOnline	= ToggleOnline;
	this.ToggleOffline	= ToggleOffline;

	this.HTMLHeader		= document.createElement( 'DIV' );
	this.HTMLOnline		= document.createElement( 'DIV' );
	this.HTMLOffline	= document.createElement( 'DIV' );
	this.HTMLShowAll	= document.createElement( 'IMG' );

	/* Remove this group
	 */
	function Clear ()
	{
		this.Items.RemoveAll();
		this.HTMLShowAll.removeNode( true );
		this.HTMLHeader.removeNode( true );
		this.HTMLOnline.removeNode( true );
		this.HTMLOffline.removeNode( true );
	}

	/* Draw the group header and all contained items
	 */
	function Draw ()
	{
		/* Header
		 */
		this.HTMLHeader.className	= 'roster-group-header';
		this.HTMLHeader.title		= external.globals( 'Translator' ).Translate( 'main', 'tt-group-hide' );
		this.HTMLHeader.innerText	= this.Name;
		this.HTMLHeader.attachEvent(
			'onmouseover',
			function ()
			{
				if ( external.globals( 'ClientRoster' ).Search.Query.length )
					return;

				event.srcElement.style.color = external.globals( 'ColorRosterGroupHeader' );
			}
		);
		this.HTMLHeader.attachEvent(
			'onmouseout',
			function ()
			{
				event.srcElement.style.color = '';
			}
		);
		this.HTMLHeader.GroupName = this.Name;
		this.HTMLHeader.attachEvent(
			'onclick',
			function ()
			{
				if ( external.globals( 'ClientRoster' ).Search.Query.length )
					return;

				external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).ToggleOnline();
			}
		);
		this.HTMLHeader.attachEvent(
			'oncontextmenu',
			function ()
			{
				if ( external.globals( 'ClientRoster' ).Search.Query.length )
					return;

				event.cancelBubble	= true;
				var connected		= external.globals( 'XMPPConnected' );
				var cfg				= external.globals( 'cfg' );
				var list			= cfg( 'contactlistdisplay' ) == 'detailed';
				var Group			= external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName );

				var none			= ! Group.ShowAll;
				var on				= Group.ShowAll && ! Group.ShowOffline;
				var off				= Group.ShowAll && Group.ShowOffline;

				var menu			= external.newPopupMenu;
				var submenu_display	= external.newPopupMenu;
				var submenu_merge	= external.newPopupMenu;
				var submenu_rooms	= external.newPopupMenu;

				submenu_display.AddItem( true, false, list,		false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_detailed' ),		10 );
				submenu_display.AddItem( true, false, ! list,	false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_compact' ),		11 );
				submenu_display.AddSeparator();
				submenu_display.AddItem( true, false, none,		false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_group_display_none' ),	12 );
				submenu_display.AddItem( true, false, on,		false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_group_display_online' ),	13 );
				submenu_display.AddItem( true, false, off,		false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_group_display_all' ),		14 );

				var groups = new Array();
				var keys = ( new VBArray( external.globals( 'ClientRoster' ).Groups.Keys() ) ).toArray();
				for ( var i = 0; i < keys.length; i++ )
					if ( event.srcElement.GroupName != keys[i] )
						groups.push( keys[i] );
				groups = groups.sort();

				for ( var i = 0; i < groups.length; ++i )
					submenu_merge.AddItem( true, false, false, false, 0, groups[i].substr( 0, 30 ), 201 + i );

				submenu_rooms.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_room_create' ), 100 );
				var rooms = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray().sort();
				if ( rooms.length )
					submenu_rooms.AddSeparator();
				for ( var i = 0; i < rooms.length; ++i )
					submenu_rooms.AddItem( true, false, false, false, 0, rooms[i].substr( 0, 30 ), 101 + i );

				menu.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_group_message' ), 2 );
				menu.AddItem( connected && external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).HTMLOnline.children.length > 1, false, false, false, submenu_rooms.Handle, external.globals( 'Translator' ).Translate( 'main', 'cl_room_invite' ), 0 );
				menu.AddSeparator();
				menu.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_group_rename' ), 3 );
				menu.AddItem( connected && groups.length, false, false, false, submenu_merge.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_group_merge' ), 0 );
				menu.AddSeparator();
				menu.AddItem( true, false, false, false, submenu_display.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_view' ), 1 );

				menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );

				if ( ! external.globals( 'ClientRoster' ).Groups.Exists( event.srcElement.GroupName ) )
					return;

				switch ( menu.Choice )
				{
					case 10: // detailed
						if ( cfg( 'contactlistdisplay' ) == 'compact' )
						{
							cfg( 'contactlistdisplay' ) = 'detailed';
							var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
							for ( var i = 0; i < TrackerNames.length; ++i )
								external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
							external.globals( 'ClientRoster' ).RefreshAll();
						}
						MenuBarUpdate( 'tools' );
					break;
					case 11: // compact
						if ( cfg( 'contactlistdisplay' ) == 'detailed' )
						{
							cfg( 'contactlistdisplay' ) = 'compact';
							var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
							for ( var i = 0; i < TrackerNames.length; ++i )
								external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
							external.globals( 'ClientRoster' ).RefreshAll();
						}
						MenuBarUpdate( 'tools' );
					break;
					case 12:
						if ( Group.ShowAll )
							Group.ToggleOnline();
					break;
					case 13:
						if ( Group.ShowOffline )
							Group.ToggleOffline();
						if ( ! Group.ShowAll )
							Group.ToggleOnline();
					break;
					case 14:
						if ( ! Group.ShowOffline )
							Group.ToggleOffline();
						if ( ! Group.ShowAll )
							Group.ToggleOnline();
					break;
					case 2:
						dial_groupmessage( event.srcElement.GroupName );
					break;
					case 3:
						external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).ChangeName();
					break;
					case 100:
						dial_conference_create( '', event.srcElement.GroupName );
					break;
					default:
						if ( menu.Choice > 100 && menu.Choice - 101 < rooms.length )
						{
							if ( external.globals( 'ConferenceSessionPool' ).Trackers.Exists( rooms[ menu.Choice - 101 ] ) )
								dial_conference_invite( external.globals( 'ConferenceSessionPool' ).Trackers.Item( rooms[ menu.Choice - 101 ] ), '', event.srcElement.GroupName );
						}
						else if ( menu.Choice > 200 && menu.Choice - 201 < groups.length )
							external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).SetName( groups[ menu.Choice - 201 ] );
				}
			}
		);
		this.HTMLHeader.attachEvent(
			'ondragenter',
			function ()
			{
				if ( external.globals( 'ClientRoster' ).Items.Exists( event.dataTransfer.getData( 'Text' ) ) )
				{
					event.returnValue = false;
					event.dataTransfer.dropEffect = 'move';
					external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).HTMLHeader.style.color = 'orange';
				}
			}
		);
		this.HTMLHeader.attachEvent(
			'ondragover',
			function ()
			{
				if ( external.globals( 'ClientRoster' ).Items.Exists( event.dataTransfer.getData( 'Text' ) ) )
				{
					event.returnValue = false;
					event.dataTransfer.dropEffect = event.ctrlKey ? 'copy' : 'move';
				}
			}
		);
		this.HTMLHeader.attachEvent(
			'ondragleave',
			function ()
			{
				external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).HTMLHeader.style.color = '';
			}
		);
		this.HTMLHeader.attachEvent(
			'ondrop',
			function ()
			{
				if ( external.globals( 'ClientRoster' ).Items.Exists( event.dataTransfer.getData( 'Text' ) ) )
				{
					if ( external.globals( 'ClientRoster' ).DragNDropOrigin != event.srcElement.GroupName )
						external.globals( 'ClientRoster' ).Items.Item( event.dataTransfer.getData( 'Text' ) ).CC( ( event.ctrlKey ? '' : external.globals( 'ClientRoster' ).DragNDropOrigin ), event.srcElement.GroupName );
				}
			}
		);

		/* ShowAll
		 */
		this.HTMLShowAll.src = '..\\images\\misc\\alpha-1x1.gif';
		this.HTMLShowAll.className = 'roster-group-toggle-expand';
		this.HTMLShowAll.title = external.globals( 'Translator' ).Translate( 'main', 'cl_group_expand' );
		this.HTMLShowAll.GroupName = this.Name;
		this.HTMLShowAll.style.display = this.Items.Count ? 'inline' : 'none';
		this.HTMLShowAll.attachEvent(
			'onclick',
			function ()
			{
				external.globals( 'ClientRoster' ).Groups.Item( event.srcElement.GroupName ).ToggleOffline();
				event.cancelBubble = true;
			}
		);
		this.HTMLHeader.insertAdjacentElement( 'afterBegin', this.HTMLShowAll );
		/* Empty group
		 */
		this.HTMLOnline.innerHTML = '<NOBR class="roster-group-empty">' + external.globals( 'Translator' ).Translate( 'main', 'cl_group_empty' ) + '</NOBR>';
		/* Hide offline contacts
		 */
		this.HTMLOffline.style.display = 'none';
		/* Find the correct location to display the group
		 */
		var list = this.Roster.HTMLElement.children;
		var factor = 3;
		var a = 0;
		var z = list.length / factor;
		if ( z == 0 || this.Name > list.item( ( z - 1 ) * factor ).GroupName )
		{
			var node = this.Roster.HTMLElement;
			node.insertAdjacentElement( 'beforeEnd', this.HTMLHeader );
			node.insertAdjacentElement( 'beforeEnd', this.HTMLOnline );
			node.insertAdjacentElement( 'beforeEnd', this.HTMLOffline );
		}
		else if ( this.Name < list.item( a * factor ).GroupName )
		{
			var node = this.Roster.HTMLElement;
			node.insertAdjacentElement( 'afterBegin', this.HTMLOffline );
			node.insertAdjacentElement( 'afterBegin', this.HTMLOnline );
			node.insertAdjacentElement( 'afterBegin', this.HTMLHeader );
		}
		else
		{
			while ( z > 1 )
			{
				var p = a + Math.floor( z / 2 );
				if ( this.Name < list.item( p * factor ).GroupName )
					z = p - a;
				else
				{
					a = p;
					z = Math.round( z / 2 );
				}
			}
			var node = list.item( ( a * factor ) + factor - 1 );
			node.insertAdjacentElement( 'afterEnd', this.HTMLOffline );
			node.insertAdjacentElement( 'afterEnd', this.HTMLOnline );
			node.insertAdjacentElement( 'afterEnd', this.HTMLHeader );
		}
		/* Restore toggle state
		 */
		var groupnames = external.globals( 'cfg' ).Item( 'hiddengroupsoffline' ).split( '\n' );
		for ( var i = 0; i < groupnames.length; ++i )
			if ( groupnames[i] == this.Name )
			{
				this.ToggleOffline();
				continue;
			}
		groupnames = external.globals( 'cfg' ).Item( 'hiddengroups' ).split( '\n' );
		for ( var i = 0; i < groupnames.length; ++i )
			if ( groupnames[i] == this.Name )
			{
				this.ToggleOnline();
				continue;
			}
	}

	/* Show or hide all the contacts in this group
	 */
	function ToggleOnline ()
	{
		if ( this.ShowAll )
		{
			/* Collapse everything
			 */
			this.HTMLOnline.style.display = 'none';
			this.HTMLShowAll.style.display = 'none';
			this.HTMLOffline.style.display = 'none';
		}
		else
		{
			/* Expand what is enabled
			 */
			this.HTMLOnline.style.display = 'block';
			if ( this.ShowOffline )
				this.HTMLOffline.style.display = 'block';
			/* Display the "show all" button
			 */
			var itemnames = ( new VBArray( this.Items.Keys() ) ).toArray();
			var i;
			for ( i = 0; i < itemnames.length; ++i )
				if ( ! this.Roster.Items.Item( itemnames[i] ).Resources.Count )
					break;
			if ( i < itemnames.length )
				this.HTMLShowAll.style.display = 'inline';
		}
		this.ShowAll = ! this.ShowAll;
		this.HTMLHeader.title = external.globals( 'Translator' ).Translate( 'main', 'tt-group-' + ( this.ShowAll ? 'hide' : 'show' ) );
	}

	/* Show or hide the offline contacts in this group
	 */
	function ToggleOffline ()
	{
		if ( this.ShowOffline )
		{
			var jids = ( new VBArray( this.Items.Keys() ) ).toArray();
			for ( var i = 0; i < jids.length; ++i )
				if ( ! this.Roster.Items.Item( jids[i] ).Resources.Count )
					this.Roster.Items.Item( jids[i] ).Hide( this );
			this.HTMLShowAll.className = 'roster-group-toggle-expand';
			this.HTMLShowAll.title = external.globals( 'Translator' ).Translate( 'main', 'cl_group_expand' );
		}
		else
		{
			var jids = ( new VBArray( this.Items.Keys() ) ).toArray().sort( function(a,b){var c=external.globals( 'ClientRoster' ).Items.Item(a).Name.toLowerCase();var d=external.globals( 'ClientRoster' ).Items.Item(b).Name.toLowerCase();return c==d?0:c<d?-1:1} );
			for ( var i = 0; i < jids.length; ++i )
				if ( ! this.Roster.Items.Item( jids[i] ).Resources.Count )
					this.Roster.Items.Item( jids[i] ).Draw( this );
			this.HTMLShowAll.className = 'roster-group-toggle-compact';
			this.HTMLShowAll.title = external.globals( 'Translator' ).Translate( 'main', 'cl_group_compact' );
		}
		this.ShowOffline = ! this.ShowOffline;
		this.HTMLOffline.style.display = this.ShowOffline ? 'block' : 'none';
	}

	/* Ask the user for a new name
	 */
	function ChangeName ()
	{
		if ( external.windows.Exists( this.Name + '/grouprename' ) )
			return external.windows( this.Name + '/grouprename' ).focus();
		else
			with ( external.createWindow( this.Name + '/grouprename', external.globals( 'cwd' ) + 'rename_group.html', this.Name ) )
			{
				setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_group_rename' ) );
				setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
				resizeable( false );
				MinHeight = MinWidth = 0;
				setSize( 320, 80 );
				setPos( ( screen.availWidth - 260 ) / 2, ( screen.availHeight - 80 ) / 2 );
			}
	}

	/* Rename the group
	 * Most clients do not support multiple <item> elements in the same IQ so the server will break them up in multiple single-item IQs
	 */
	function SetName ( newname )
	{
		if ( newname != this.Name )
		{
			var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
			dom.loadXML( '<iq type="set"><query/></iq>' );
			dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
			var query = dom.documentElement.firstChild;
			var contacts = ( new VBArray( this.Items.Keys() ) ).toArray();

			/* Non-empty group: update all roster items on the server (changes are pushed back to the client)
			 */
			if ( contacts.length )
			{
				for ( var i = 0; i < contacts.length; ++i )
				{
					var contact = this.Roster.Items.Item( contacts[i] );
					var item = dom.createElement( 'item' );
					item.setAttribute( 'jid', contacts[i] );
					if ( contact.Name.length && contact.Name != contact.JID )
						item.setAttribute( 'name', contact.Name );
					for ( var j = contact.Groups.length - 1; j >= 0; --j )
					{
						if ( contact.Groups[j] == newname )
							continue;
						var group = dom.createElement( 'group' );
						group.text = contact.Groups[j] == this.Name ? newname : contact.Groups[j];
						item.appendChild( group );
					}
					query.appendChild( item );
				}
				query.setAttribute( 'xmlns', 'jabber:iq:roster' );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}

			/* Empty group: create a new group object, and delete the current group object
			 */
			else
			{
				var group = this.Roster.CreateGroup( newname );
				if ( ! group.HTMLHeader.className )
					group.Draw();
				this.Clear();
				this.Roster.Groups.Remove( this.Name );
			}
		}
	}

	/* Create a new roster item in this group
	 * If there is already a contact with that address, the current ClientRosterItem object is returned
	 */
	function CreateItem ( jid )
	{
		if ( ! this.Roster.Items.Exists( jid ) )
			this.Roster.Items.Add( jid, new ClientRosterItem( this.Roster, jid ) );
		if ( ! this.Items.Exists( jid ) )
			this.Items.Add( jid, null );
		return this.Roster.Items.Item( jid );
	}
}

/* A collection of resources
 */
function ClientRosterItem ( roster, jid )
{
	this.JID			= jid;
	this.Address		= new XMPPAddress( jid );
	this.Ask			= '';
	this.Name			= '';
	this.Groups			= new Array();
	this.Roster			= roster;
	this.Status			= '';
	this.Resources		= new ActiveXObject( 'Scripting.Dictionary' );
	this.HTMLElements	= new ActiveXObject( 'Scripting.Dictionary' );
	this.Subscription	= '';

	this.CC				= CC;
	this.Draw			= Draw;
	this.Hide			= Hide;
	this.Clear			= Clear;
	this.Purge			= Purge;
	this.Update			= Update;
	this.SetName		= SetName;
	this.ReRequest		= ReRequest;
	this.ChangeName		= ChangeName;
	this.RefreshAll		= RefreshAll;
	this.UpdateTracker	= UpdateTracker;
	this.ReceivePresence = ReceivePresence;

	/* Update the conversation window
	 */
	function UpdateTracker ()
	{
		if ( external.globals( 'ChatSessionPool' ).GetTracker( this.Address ) ) {
			external.globals( 'ChatSessionPool' ).GetTracker( this.Address ).DrawContainerInfo();
			external.globals( 'ChatSessionPool' ).GetTracker( this.Address ).DiscoInfo();
		}
	}

	/* Redraws the item/resources in all groups
	 */
	function RefreshAll ()
	{
		/* Check address in case a new transport has been loaded
		 */
		if ( this.Name == this.JID && external.globals( 'ClientServices' ).Services.Exists( this.Address.Host ) )
			this.Name = this.Address.CleanAddress();
		/* Redraw() the resources and Update() the items
		 */
		for ( var i = 0; i < this.Groups.length; ++i )
		{
			var theGroup = this.Roster.Groups.Item( this.Groups[i] );
			var resources = ( new VBArray( this.Resources.Keys() ) ).toArray();
			for ( var j = 0; j < resources.length; ++j )
				this.Resources.Item( resources[j] ).Redraw( theGroup );
			if ( this.HTMLElements.Exists( this.Groups[i] ) )
				this.Update( theGroup );
		}
	}

	/* This is used for both copying or moving contacts to other groups
	 */
	function CC ( oldGroupName, newGroupName )
	{
		var newGroups = new ActiveXObject( 'Scripting.Dictionary' );
		newGroups.Add( newGroupName, null );
		for ( var i = 0; i < this.Groups.length; ++i )
			if ( this.Groups[i] != oldGroupName && ! newGroups.Exists( this.Groups[i] ) )
				newGroups.Add( this.Groups[i], null );
		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.loadXML( '<iq type="set"><query><item/></query></iq>' );
		dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
		var groupnames = ( new VBArray( newGroups.Keys() ) ).toArray();
		for ( var j = 0; j < groupnames.length; ++j )
		{
			var node = dom.createElement( 'group' );
			node.text = groupnames[j];
			dom.firstChild.firstChild.firstChild.appendChild( node );
		}
		if ( this.Name != this.JID && this.Name.length )
			dom.firstChild.firstChild.firstChild.setAttribute( 'name', this.Name );
		dom.firstChild.firstChild.firstChild.setAttribute( 'jid', this.JID );
		dom.firstChild.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Remove this item and its resources
	 */
	function Clear ()
	{
		for ( var i = 0; i < this.Groups.length; ++i )
		{
			if ( this.HTMLElements.Exists( this.Groups[i] ) )
				this.HTMLElements.Item( this.Groups[i] ).removeNode( true );
			this.Roster.Groups.Item( this.Groups[i] ).Items.Remove( this.JID );
			if ( this.Roster.Groups.Item( this.Groups[i] ).Items.Count )
			{
				/* Hide the "display offline contacts" button
				 */
				var itemnames = ( new VBArray( this.Roster.Groups.Item( this.Groups[i] ).Items.Keys() ) ).toArray();
				var j;
				for ( j = 0; j < itemnames.length; ++j )
					if ( ! this.Roster.Items.Item( itemnames[j] ).Resources.Count )
						break;
				if ( j >= itemnames.length )
					this.Roster.Groups.Item( this.Groups[i] ).HTMLShowAll.style.display = 'none';
			}
			else
			{
				/* Delete the empty group
				 */
				this.Roster.Groups.Item( this.Groups[i] ).Clear();
				this.Roster.Groups.Remove( this.Groups[i] );
			}
		}
		this.HTMLElements.RemoveAll();
		var resources = ( new VBArray( this.Resources.Keys() ) ).toArray();
		for ( var i = 0; i < resources.length; ++i )
			this.Resources.Item( resources[i] ).Clear();
		this.Resources.RemoveAll();
		this.Roster.Items.Remove( this.JID );
		this.UpdateTracker();
	}

	/* Parse the Presence object some more
	 */
	function ReceivePresence ( jid, resource, presence )
	{
		/* Going offline
		 */
		if ( presence.Type == 'unavailable' )
		{
			this.Status = presence.Status;
			if ( this.Resources.Exists( resource ) )
			{
				for ( var i = 0; i < this.Groups.length; ++i )
				{
					/* Delete the resource from the group
					 */
					this.Resources.Item( resource ).Hide( this.Roster.Groups.Item( this.Groups[i] ) );
					/* If there are no more resources, add the item to the offline section
					 */
					if ( this.Resources.Count == 1 && this.Roster.Groups.Item( this.Groups[i] ).ShowOffline )
						this.Draw( this.Roster.Groups.Item( this.Groups[i] ) );
					/* Show the "no one is online" text
					 */
					if ( this.Roster.Groups.Item( this.Groups[i] ).HTMLOnline.children.length < 2 )
						this.Roster.Groups.Item( this.Groups[i] ).HTMLOnline.firstChild.style.display = 'block';
					/* Show the "display offline contacts" button
					 */
					if ( this.Roster.Groups.Item( this.Groups[i] ).ShowAll && this.Resources.Count == 1 )
						this.Roster.Groups.Item( this.Groups[i] ).HTMLShowAll.style.display = 'inline';
				}
				this.Resources.Remove( resource );
			}
		}
		/* Coming online or changing status
		 */
		else
		{
			var userresource = this.Resources.Exists( resource ) ? this.Resources.Item( resource ) : new ClientRosterResource( this, resource );

			this.Status = '';

			var Hash = '';
			var Node = null;

			if ( Node = presence.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "jabber:x:avatar"]/hash' ) )
				Hash = Node.text.replace( /[^0-9a-zA-Z]/gm, '' );
			else if ( Node = presence.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "vcard-temp:x:update"]/photo' ) )
				Hash = Node.text.replace( /[^0-9a-zA-Z]/gm, '' );

			if ( Hash.length == 40 )
			{
				if ( external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + Hash ) )
					userresource.Avatar = Hash;
				else if ( presence.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "vcard-temp:x:update"]/photo' ) )
				{
					if ( userresource.LoadingAvatarHash != Hash )
					{
						var hook			= new XMPPHookIQ();
						hook.Window			= external.wnd;
						hook.Callback		= 'ClientRosterAvatarVCard';
						userresource.LoadingAvatarHash	= Hash;
						userresource.LoadingAvatarId	= hook.Id;
						var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
						dom.loadXML( '<iq type="get"><vCard xmlns="vcard-temp"/></iq>' );
						dom.documentElement.setAttribute( 'id', hook.Id );
						dom.documentElement.setAttribute( 'to', this.JID );
						warn( 'SENT: ' + dom.xml );
						external.XMPP.SendXML( dom );
					}
					if ( ! userresource.Avatar.length )
						userresource.Avatar = this.Roster.GetAvatar( resource, this.JID );
				}
				else
				{
					if ( userresource.LoadingAvatarHash != Hash )
					{
						var hook			= new XMPPHookIQ();
						hook.Window			= external.wnd;
						hook.Callback		= 'ClientRosterAvatarStorage';
						userresource.LoadingAvatarHash	= Hash;
						userresource.LoadingAvatarId	= hook.Id;
						var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
						dom.loadXML( '<iq type="get"><query xmlns="storage:client:avatar"/></iq>' );
						dom.documentElement.setAttribute( 'id', hook.Id );
						dom.documentElement.setAttribute( 'to', this.JID );
						warn( 'SENT: ' + dom.xml );
						external.XMPP.SendXML( dom );
					}
					if ( ! userresource.Avatar.length )
						userresource.Avatar = this.Roster.GetAvatar( resource, this.JID );
				}
			}
			else
			{
				var res_avatar = this.Roster.GetAvatar( resource, this.JID );
				if ( res_avatar.length )
					userresource.Avatar = res_avatar;
				if ( ! userresource.Avatar.length && ! userresource.LoadingVersion )
				{
					userresource.LoadingVersion	= true;
					var hook					= new XMPPHookIQ();
					hook.Window					= external.wnd;
					hook.Callback				= 'ClientRosterVersion';
					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:version"/></iq>' );
					dom.documentElement.setAttribute( 'id', hook.Id );
					dom.documentElement.setAttribute( 'to', this.JID + '/' + resource );
					warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}
			}

			userresource.Status		= presence.Status.replace( /[\n\r]+/mg, ' - ' );
			userresource.Show		= presence.Show;
			userresource.Priority	= presence.Priority;
			userresource.Attention	= false;

			if ( ! presence.Status.length || ( userresource.Show == 'away' && userresource.Status.toLowerCase() == 'away' ) || ( userresource.Show == 'dnd' && userresource.Status.toLowerCase() == 'busy' ) )
				userresource.Status = external.globals( 'Translator' ).Translate( 'main', 'cl_status_empty' );

			if ( this.Resources.Exists( resource ) )
			{
				for ( var i = 0; i < this.Groups.length; ++i )
					userresource.Redraw( this.Roster.Groups.Item( this.Groups[i] ) );
			}
			else
			{
				this.Resources.Add( resource, userresource )
				for ( var i = 0; i < this.Groups.length; ++i )
				{
					var theGroup = this.Roster.Groups.Item( this.Groups[i] );
					this.Hide( theGroup );
					this.Resources.Item( resource ).Draw( theGroup );
					/* Hide the "no one is online" text
					 */
					theGroup.HTMLOnline.firstChild.style.display = 'none';
					/* Hide the "display offline contacts" button
					 */
					var itemnames = ( new VBArray( theGroup.Items.Keys() ) ).toArray();
					var j;
					for ( j = 0; j < itemnames.length; ++j )
						if ( ! this.Roster.Items.Item( itemnames[j] ).Resources.Count )
							break;
					if ( j >= itemnames.length )
						theGroup.HTMLShowAll.style.display = 'none';
				}
			}
			if ( external.globals( 'ClientServices' ).Services.Exists( this.JID ) )
				this.Clear();
		}

		this.UpdateTracker();
	}

	/* Remove the offline item from the specified group
	 */
	function Hide ( group )
	{
		if ( ! this.HTMLElements.Exists( group.Name ) )
			return;
		this.HTMLElements.Item( group.Name ).removeNode( true );
		this.HTMLElements.Remove( group.Name );
	}

	/* Refreshes the displayed name and status message in the specified group
	 */
	function Update ( group )
	{
		this.HTMLElements.Item( group.Name ).style.paddingBottom = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : '5px';
		this.HTMLElements.Item( group.Name ).style[ external.globals( 'Translator' ).Direction ? 'marginRight' : 'marginLeft' ] = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '29px' : '21px';
		this.HTMLElements.Item( group.Name ).title = external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip_offline', [ this.Address.CleanAddress() ] );
		if ( this.Status.length )
			this.HTMLElements.Item( group.Name ).title += '\n' + this.Status;
		with ( this.HTMLElements.Item( group.Name ).children )
		{
			item(0).className = external.globals( 'block' ).Exists( this.JID ) ? 'roster-item-offline-name-blocked' : 'roster-item-offline-name';
			item(0).innerText = this.Name + '\n';
			item(1).className = 'roster-item-offline-msg';
			item(1).style.display = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'none';
			if ( this.Subscription == 'to' || this.Subscription == 'both' )
			{
				item(1).innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_offline' );
				if ( this.Status.length )
					item(1).innerText += ' - ' + this.Status;
			}
			else if ( ( this.Subscription == 'none' || this.Subscription == 'from' ) && this.Ask == 'subscribe' )
				item(1).innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_awaiting' );
			else
				item(1).innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_unknown' );
		}
	}

	/* Show the offline item in the specified group
	 */
	function Draw ( group )
	{
		/* <NOBR>
		 *   <SPAN class="roster-item-offline-name|blocked">Nickname</SPAN>
		 *   <SPAN class="roster-item-offline-msg">Offline|Awaiting|Unknown</SPAN>
		 * </NOBR>
		 */
		if ( this.HTMLElements.Exists( group.Name ) )
			return;
		var elem = document.createElement( 'NOBR' );
		elem.className = 'roster-item-offline';
		elem.style.paddingBottom = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : '5px';
		elem.style[ external.globals( 'Translator' ).Direction ? 'marginRight' : 'marginLeft' ] = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '29px' : '21px';
		elem.JID = this.JID;
		elem.GroupName = group.Name;
		elem.attachEvent(
			'onmouseover',
			function ()
			{
				if ( event.srcElement.tagName == 'BR' )
					event.srcElement.parentNode.parentNode.firstChild.style.textDecoration = 'underline';
				else if ( event.srcElement.tagName != 'NOBR' )
					event.srcElement.parentNode.firstChild.style.textDecoration = 'underline';
				else
					event.srcElement.firstChild.style.textDecoration = 'underline';
			}
		);
		elem.attachEvent(
			'onmouseout',
			function ()
			{
				if ( event.srcElement.tagName == 'BR' )
					event.srcElement.parentNode.parentNode.firstChild.style.textDecoration = 'underline';
				else if ( event.srcElement.tagName != 'NOBR' )
					event.srcElement.parentNode.firstChild.style.textDecoration = 'none';
				else
					event.srcElement.firstChild.style.textDecoration = 'none';
			}
		);
		elem.attachEvent(
			'onclick',
			function ()
			{
				var obj = event.srcElement;
				while ( obj.tagName != 'NOBR' )
					obj = obj.parentNode;
				external.globals( 'ClientRoster' ).Search.Close();
				dial_chat( obj.JID );
			}
		);
		elem.attachEvent(
			'oncontextmenu',
			function ()
			{
				var obj = event.srcElement;
				while ( obj.tagName != 'NOBR' )
					obj = obj.parentNode;
				mousemenu( obj.JID, '', obj.GroupName );
			}
		);
		var name = document.createElement( 'SPAN' );
		name.className = external.globals( 'block' ).Exists( this.JID ) ? 'roster-item-offline-name-blocked' : 'roster-item-offline-name';
		name.innerText = this.Name + '\n';
		elem.insertAdjacentElement( 'beforeEnd', name );
		var msg = document.createElement( 'SPAN' );
		msg.className = 'roster-item-offline-msg';
		if ( this.Subscription == 'to' || this.Subscription == 'both' )
		{
			msg.innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_offline' );
			if ( this.Status.length )
				msg.innerText += ' - ' + this.Status;
		}
		else if ( ( this.Subscription == 'none' || this.Subscription == 'from' ) && this.Ask == 'subscribe' )
			msg.innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_awaiting' );
		else
			msg.innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_unknown' );
		msg.style.display = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'none';
		elem.insertAdjacentElement( 'beforeEnd', msg );
		elem.title = external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip_offline', [ this.Address.CleanAddress() ] );
		if ( this.Status.length )
			elem.title += '\n' + this.Status;
		this.HTMLElements.Add( group.Name, elem );
		group.HTMLOffline.insertAdjacentElement( 'beforeEnd', elem );
	}

	/* Ask the user for a new name
	 */
	function ChangeName ()
	{
		if ( external.windows.Exists( this.JID + '/rename' ) )
			return external.windows( this.JID + '/rename' ).focus();
		else
			with ( external.createWindow( this.JID + '/rename', external.globals( 'cwd' ) + 'rename_user.html', this.JID ) )
			{
				setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_contact_rename' ) );
				setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
				resizeable( false );
				MinHeight = MinWidth = 0;
				setSize( 320, 80 );
				setPos( ( screen.availWidth - 260 ) / 2, ( screen.availHeight - 80 ) / 2 );
			}
	}

	/* Rename the item
	 */
	function SetName ( newname )
	{
		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.loadXML( '<iq type="set"><query><item/></query></iq>' );
		dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
		with ( dom.documentElement.firstChild.firstChild )
		{
			for ( var i = 0; i < this.Groups.length; ++i )
			{
				var node = dom.createElement( 'group' );
				node.text = this.Groups[i];
				appendChild( node );
			}
			setAttribute( 'jid', this.JID );
			if ( newname.length && newname != this.JID )
				setAttribute( 'name', newname );
		}
		dom.firstChild.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Send another subscription request
	 */
	function ReRequest ()
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<presence type="subscribe"/>' );
		dom.documentElement.setAttribute( 'to', this.JID );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Delete the item from one or all groups in the roster
	 */
	function Purge ( override, groupname )
	{
		if ( this.Roster.Groups.Exists( groupname ) && this.Roster.Groups.Item( groupname ).Items.Exists( this.JID ) && this.Groups.length > 1 )
		{
			/* Take it out of this group but leave it in the other ones
			 */
			if ( override || external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'cl_contact_remove_group' ), external.globals( 'softwarename' ), 4 | 48 ) == 6 )
			{
				var newGroups = new ActiveXObject( 'Scripting.Dictionary' );
				for ( var i = 0; i < this.Groups.length; ++i )
					if ( this.Groups[i] != groupname && ! newGroups.Exists( this.Groups[i] ) )
						newGroups.Add( this.Groups[i], null );
				var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
				dom.loadXML( '<iq type="set"><query><item/></query></iq>' );
				dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
				var groupnames = ( new VBArray( newGroups.Keys() ) ).toArray();
				for ( var j = 0; j < groupnames.length; ++j )
				{
					var node = dom.createElement( 'group' );
					node.text = groupnames[j];
					dom.firstChild.firstChild.firstChild.appendChild( node );
				}
				if ( this.Name != this.JID && this.Name.length )
					dom.firstChild.firstChild.firstChild.setAttribute( 'name', this.Name );
				dom.firstChild.firstChild.firstChild.setAttribute( 'jid', this.JID );
				dom.firstChild.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		}
		else
		{
			/* Completely gone!
			 */
			if ( override || external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'cl_contact_remove_list' ), external.globals( 'softwarename' ), 4 | 48 ) == 6 )
			{
				if ( external.globals( 'block' ).Exists( this.JID ) )
					dial_block( this.Address );
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<iq type="set"><query xmlns="jabber:iq:roster"><item subscription="remove"/></query></iq>' );
				dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
				dom.documentElement.firstChild.firstChild.setAttribute( 'jid', this.JID );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		}
	}
}

/* An identifier of a contact who sent us presence
 * Jabber supports multiple, simultaneously logins on the same address
 */
function ClientRosterResource ( item, name )
{
	this.Show				= '';
	this.Avatar				= '';
	this.Status				= '';
	this.Priority			= 0;
	this.RosterItem			= item;
	this.HTMLElements		= new ActiveXObject( 'Scripting.Dictionary' );
	this.ResourceName		= name;
	this.LoadingVersion		= false;
	this.LoadingAvatarId	= '';
	this.LoadingAvatarHash	= '';

	this.Draw				= Draw;
	this.Hide				= Hide;
	this.Clear				= Clear;
	this.Redraw				= Redraw;

	/* Remove these resources
	 */
	function Clear ()
	{
		var groupnames = ( new VBArray( this.HTMLElements.Keys() ) ).toArray();
		for ( var i = 0; i < groupnames.length; ++i )
			this.HTMLElements.Item( groupnames[i] ).removeNode( true );
		this.HTMLElements.RemoveAll();
	}

	/* Remove the resource from the specified group
	 */
	function Hide ( group )
	{
		this.HTMLElements.Item( group.Name ).removeNode( true );
		this.HTMLElements.Remove( group.Name );
	}

	/* Update the resource's information in the specified group
	 */
	function Redraw ( group )
	{
		this.HTMLElements.Item( group.Name ).style.paddingBottom = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : '5px';
		this.HTMLElements.Item( group.Name ).title = this.ResourceName.length ? external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip_online', [ this.RosterItem.Address.CleanAddress(), this.ResourceName ] ) : external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip_offline', [ this.RosterItem.Address.CleanAddress() ] );
		if ( this.Status.length )
			this.HTMLElements.Item( group.Name ).title += '\n' + this.Status;
		with ( this.HTMLElements.Item( group.Name ).children )
		{
			var img = item(0).tagName == 'IMG' ? item(0) : this.HTMLElements.Item( group.Name ).insertAdjacentElement( 'afterBegin', document.createElement( 'IMG' ) );
			img.className = 'roster-resource-avatar';
			img.align = external.globals( 'Translator' ).Direction ? 'right' : 'left';
			if ( this.Avatar.length == 0 )
				img.src = '..\\images\\clients\\unknown-soldier.png';
			else if ( this.Avatar.length == 40 )
				img.src = external.globals( 'usersdir' ) + 'Avatars\\' + this.Avatar;
			else if ( external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'compact' && external.FileExists( external.globals( 'cwd' ) + '..\\images\\clients\\small\\' + this.Avatar ) )
				img.src = external.globals( 'cwd' ) + '..\\images\\clients\\small\\' + this.Avatar;
			else
				img.src = external.globals( 'cwd' ) + '..\\images\\clients\\' + this.Avatar;
			img.height = img.width = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? 24 : 16;

			switch ( this.Show )
			{
				case 'dnd': item(2).className = 'roster-resource-busy'; item(2).innerText = ' - ' + external.globals( 'Translator' ).Translate( 'main', 'cl_busy' ) + '\n'; break;
				case 'xa':
				case 'away': item(2).className = 'roster-resource-away'; item(2).innerText = ' - ' + external.globals( 'Translator' ).Translate( 'main', 'cl_away' ) + '\n'; break;
				default: item(2).innerText = '\n';
			}
			item(1).className = external.globals( 'block' ).Exists( this.RosterItem.JID ) ? 'roster-resource-name-blocked' : 'roster-resource-name';
			item(1).innerText = this.RosterItem.Name + ( this.RosterItem.Resources.Count > 1 ? ' - ' + this.ResourceName : '' );
			item(1).style.fontWeight = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'normal';

			item(3).className = 'roster-resource-msg';
			item(3).innerText = '';
			
			DrawModeMessage ( item(3) ,  this.Status, 'roster-resource-link' );

			item(3).style.display = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'none';
		}
		if ( this.HTMLElements.Item( group.Name ).UserName != this.RosterItem.Name )
		{
			this.HTMLElements.Item( group.Name ).UserName = this.RosterItem.Name;
			group.HTMLOffline.insertAdjacentElement( 'beforeEnd', this.HTMLElements.Item( group.Name ) );
			/* Find the correct location to display the resource in this group
			 */
			var name = this.HTMLElements.Item( group.Name ).UserName.toLowerCase();
			var list = group.HTMLOnline.children;
			var a = 0;
			var z = list.length;
			while ( z > 1 )
			{
				var p = a + Math.floor( z / 2 );
				if ( name < list.item( p ).UserName.toLowerCase() )
					z = p - a;
				else
				{
					a = p;
					z = Math.round( z / 2 );
				}
			}
			var node = list.item( a );
			node.insertAdjacentElement( 'afterEnd', this.HTMLElements.Item( group.Name ) );
		}
	}

	/* Show the resource in the specified group
	 */
	function Draw ( group )
	{
		/* <NOBR>
		 *   <IMG/>  <SPAN>Nickname</SPAN>  <SPAN>Away|Busy</SPAN>
		 *           <SPAN>Status</SPAN>
		 * </NOBR>
		 */
		var elem = document.createElement( 'NOBR' );
		elem.className = 'roster-resource';
		elem.style.paddingBottom = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : '5px';
		elem.GroupName = group.Name;
		elem.UserName = this.RosterItem.Name;
		elem.Resource = this.ResourceName;
		elem.JID = this.RosterItem.JID;
		elem.attachEvent(
			'onmouseover',
			function ()
			{
				var elem = event.toElement;
				while ( elem.tagName != 'NOBR' )
					elem = elem.parentNode;
				elem.children( elem.firstChild.tagName == 'IMG' ? 1 : 0 ).style.textDecoration = 'underline';
			}
		);
		elem.attachEvent(
			'onmouseout',
			function ()
			{
				var elem = event.fromElement;
				while ( elem.tagName != 'NOBR' )
					elem = elem.parentNode;
				elem.children( elem.firstChild.tagName == 'IMG' ? 1 : 0 ).style.textDecoration = 'none';
			}
		);
		elem.attachEvent(
			'onclick',
			function ()
			{
				var obj = event.srcElement;
				while ( obj.tagName != 'NOBR' )
					obj = obj.parentNode;
				external.globals( 'ClientRoster' ).Search.Close();
				dial_chat( obj.JID + '/' + obj.Resource );
			}
		);
		elem.attachEvent(
			'oncontextmenu',
			function ()
			{
				var obj = event.srcElement;
				while ( obj.tagName != 'NOBR' )
					obj = obj.parentNode;
				mousemenu( obj.JID, obj.Resource, obj.GroupName );
			}
		);
		elem.attachEvent(
			'ondragstart',
			function ()
			{
				var obj = event.srcElement;
				while ( obj.tagName != 'NOBR' )
					obj = obj.parentNode;
				external.globals( 'ClientRoster' ).DragNDropOrigin = obj.GroupName;
				event.dataTransfer.setData( 'Text', obj.JID );
				event.dataTransfer.setData( 'URL', 'xmpp:' + obj.JID );
				event.dataTransfer.effectAllowed = 'all';
				event.cancelBubble = true;
			}
		);

		var img = document.createElement( 'IMG' );
		img.className = 'roster-resource-avatar';
		img.align = external.globals( 'Translator' ).Direction ? 'right' : 'left';
		if ( this.Avatar.length == 0 )
			img.src = '..\\images\\clients\\unknown-soldier.png';
		else if ( this.Avatar.length == 40 )
			img.src = external.globals( 'usersdir' ) + 'Avatars\\' + this.Avatar;
		else if ( external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'compact' && external.FileExists( external.globals( 'cwd' ) + '..\\images\\clients\\small\\' + this.Avatar ) )
			img.src = external.globals( 'cwd' ) + '..\\images\\clients\\small\\' + this.Avatar;
		else
			img.src = external.globals( 'cwd' ) + '..\\images\\clients\\' + this.Avatar;
		img.height = img.width = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? 24 : 16;
		elem.insertAdjacentElement( 'beforeEnd', img );

		var name = document.createElement( 'SPAN' );
		name.className = external.globals( 'block' ).Exists( this.RosterItem.JID ) ? 'roster-resource-name-blocked' : 'roster-resource-name';
		name.style.fontWeight = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'normal';
		name.innerText = this.RosterItem.Name + ( this.RosterItem.Resources.Count > 1 ? ' - ' + this.ResourceName : '' );
		elem.insertAdjacentElement( 'beforeEnd', name );
		switch ( this.Show )
		{
			case 'dnd': elem.insertAdjacentHTML( 'beforeEnd', '<SPAN class=roster-resource-busy>&nbsp;- ' + external.globals( 'Translator' ).Translate( 'main', 'cl_busy' ) + '<BR></SPAN>' ); break;
			case 'xa':
			case 'away': elem.insertAdjacentHTML( 'beforeEnd', '<SPAN class=roster-resource-away>&nbsp;- ' + external.globals( 'Translator' ).Translate( 'main', 'cl_away' ) + '<BR></SPAN>' ); break;
			default: elem.insertAdjacentHTML( 'beforeEnd', '<SPAN class=roster-resource-away><BR></SPAN>' );
		}
		var msg = document.createElement( 'SPAN' );
		msg.className = 'roster-resource-msg';
		
		DrawModeMessage ( msg, this.Status , 'roster-resource-link' );
		
		msg.style.display = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'none';
		elem.title = this.ResourceName.length ? external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip_online', [ this.RosterItem.Address.CleanAddress(), this.ResourceName ] ) : external.globals( 'Translator' ).Translate( 'main', 'cl_tooltip_offline', [ this.RosterItem.Address.CleanAddress() ] );
		if ( this.Status.length )
			elem.title += '\n' + this.Status;
		elem.insertAdjacentElement( 'beforeEnd', msg );
		this.HTMLElements.Add( group.Name, elem );
		/* Find the correct location to display the resource in this group
		 */
		var name = elem.UserName.toLowerCase();
		var list = group.HTMLOnline.children;
		var a = 0;
		var z = list.length;
		while ( z > 1 )
		{
			var p = a + Math.floor( z / 2 );
			if ( name < list.item( p ).UserName.toLowerCase() )
				z = p - a;
			else
			{
				a = p;
				z = Math.round( z / 2 );
			}
		}
		var node = list.item( a );
		node.insertAdjacentElement( 'afterEnd', elem );
	}
}
