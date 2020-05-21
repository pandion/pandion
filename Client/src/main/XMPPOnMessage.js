function XMPPOnMessage ( ReceivedXML )
{
	if ( external.globals( 'debug' ) )
		warn( 'RECV: ' + ReceivedXML.xml );

	var Message = new XMPPMessage();
	Message.FromDOM( ReceivedXML );

	/* Default to our server if missing from address
	 */
	if ( ! Message.FromAddress.Host.length )
		Message.FromAddress.Parse( external.globals( 'cfg' ).Item( 'server' ) );

	/* Plug-in event
	 */
	var context = new ActiveXObject( 'Scripting.Dictionary' );
	context.Add( 'name', 'onXMPPMessage' );
	context.Add( 'message', Message );
	context.Add( 'xmldom', ReceivedXML );
	if ( external.globals( 'ClientPluginContainer' ).DispatchEvent( context ) )
		return;

	var ShortAddress = Message.FromAddress.ShortAddress();

	/* Pass the message to the conference session pool
	 */
	if ( external.globals( 'ConferenceSessionPool' ).Trackers.Exists( ShortAddress ) || external.globals( 'ConferenceSessionPool' ).TrackersLoading.Exists( ShortAddress ) )
	{
		var Event = new ConferenceSessionEvent();
		Event.Type = 'message';
		Event.Address = Message.FromAddress;
		Event.Payload = Message;
		external.globals( 'ConferenceSessionPool' ).AddEvent( Event );
		return;
	}

	/* Pass the message to the chat session pool
	 */
	else if ( Message.Type == 'error' )
	{
		if ( external.globals( 'ChatSessionPool' ).GetTracker( Message.FromAddress ) )
		{
			var Event = new ChatSessionEvent();
			Event.Type = 'message';
			Event.Address = Message.FromAddress;
			Event.Payload = Message;
			external.globals( 'ChatSessionPool' ).GetTracker( Message.FromAddress ).DispatchEvent( Event );
		}
		return;
	}

	/* Message blocking
	 */
	else if ( external.globals( 'block' ).Exists( ShortAddress ) || ( ! external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) && external.globals( 'cfg' ).Item( 'msgnotinlist' ).toString() == 'false' ) )
	{
		if ( Message.Body.length )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<message type="error"><error code="405"/></message>' );
			dom.documentElement.firstChild.text = external.globals( 'Translator' ).Translate( 'main', 'msg_blocked' );
			dom.documentElement.setAttribute( 'to', Message.From );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
		return;
	}

	/* Acknowledge message
	 */
	else if ( Message.WantsDelivered )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<message><x xmlns="jabber:x:event"><delivered/><id/></x></message>' );
		dom.selectSingleNode( '/message/x/id' ).text = Message.Id;
		dom.documentElement.setAttribute( 'to', Message.From );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Reroute service messages to the headline window
	 */
	if ( ! Message.FromAddress.User.length )
		Message.Type = 'headline';

	/* Respond to conference room invitation
	 */
	if ( Message.MUCInvitationFrom.length )
	{
		var FromName = new XMPPAddress( Message.MUCInvitationFrom );
		FromName = external.globals( 'ClientRoster' ).Items.Exists( FromName.ShortAddress() ) ? external.globals( 'ClientRoster' ).Items.Item( FromName.ShortAddress() ).Name + ' <' + FromName.ShortAddress() + '>': FromName.ShortAddress();
		if ( external.globals( 'ClientServices' ).Services.Exists( Message.MUCInvitationFrom ) && external.globals( 'ClientServices' ).Services.Item( Message.MUCInvitationFrom ).Options & 0x0020 )
		{
			dial_conference( ShortAddress, Message.MUCInvitationPassword );
		}
		else
		{
			if ( ! external.wnd.isActive() )
			{
				external.wnd.hide( false );
				external.wnd.restore();
				external.wnd.focus();
			}

			if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'conference_invitation', [ FromName, ShortAddress ] ), external.globals( 'softwarename' ), 4 | 48 ) )
			{
				dial_conference( ShortAddress, Message.MUCInvitationPassword );
			}
			else
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<message><x xmlns="http://jabber.org/protocol/muc#user"><decline/></x></message>' );
				dom.documentElement.setAttribute( 'to', Message.From );
				dom.documentElement.firstChild.firstChild.setAttribute( 'to', Message.MUCInvitationFrom );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		}
	}

	/* Respond to conference room invitation
	 */
	else if ( Message.ConferenceInvitation.length )
	{
		if ( ! external.wnd.isActive() )
		{
			external.wnd.hide( false );
			external.wnd.restore();
			external.wnd.focus();
		}
		var FromName = ShortAddress;
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
			FromName = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name + ' <' + ShortAddress + '>';
		if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'conference_invitation', [ FromName, Message.ConferenceInvitation ] ), external.globals( 'softwarename' ), 4 | 48 ) )
			dial_conference( Message.ConferenceInvitation );
	}
	
	/* Message Attention
	*/			
	else if ( Message.Type == 'headline' && Message.Attention == 'urn:xmpp:attention:0' ) 
	{
		Message.Body = (Message.Body.length ? Message.Body : external.globals( 'Translator' ).Translate( 'chat-container', 'receive_attention' ));
		var Host							= Message.FromAddress.Host;
		var Resource					= Message.FromAddress.Resource;
		var Toaster 					= new Headline();
		Toaster.Message				= Message.Body;
		Toaster.Address				= Message.FromAddress;
		Toaster.ShowPreferences		= true;
		Toaster.PreferenceSection	= 'privacy';
		Toaster.OpenConversation 	= Message.FromAddress;
		Toaster.CloseOnOptions     = false;
		Toaster.AutoClose				= false;
		Toaster.Archive				= false;
		Toaster.HtmlFile				= 'attention.html'
		
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			Toaster.Title = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name;

			if ( external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Resources.Exists( Resource ) )
				Toaster.Icon = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Resources.Item( Resource ).Avatar;
			else
				Toaster.Icon = 'unknown-soldier.png';
		}
		else
		{
			Toaster.Icon = external.globals( 'ClientRoster' ).GetAvatar( Resource.toLowerCase(), ShortAddress );
			if ( ! Toaster.Icon.length )
				Toaster.Icon = 'unknown-soldier.png';

			if ( external.globals( 'ClientServices' ).Services.Exists( Host ) && external.globals( 'ClientServices' ).Services.Item( Host ).Options & 0x0001 )
				Toaster.Title = external.globals( 'ClientServices' ).Services.Item( Host ).Name;
			if ( ! Toaster.Title.length )
				Toaster.Title = ShortAddress;
		}
		
		Toaster.Show();
		
		history_add( ShortAddress, Message.Stamp, Message.Body, true );
		var Event = new ChatSessionEvent();
		Event.Type = 'message';
		Event.Address = Message.FromAddress;
		Event.Payload = Message;
		external.globals( 'ChatSessionPool' ).AddEvent( Event );
	}
	
	/* Toast it!
	 */
	else if ( Message.Type == 'headline' && ( Message.Body.length || Message.Subject.length ) )
	{
		var Host			= Message.FromAddress.Host;
		var Resource		= Message.FromAddress.Resource;
		var Toaster			= new Headline();
		Toaster.Message		= Message.Body.length ? Message.Body : Message.Subject;
		Toaster.Address		= Message.FromAddress;

		if ( Message.OOB.length )
			Toaster.OpenHyperlink = Message.OOB[0];
		else
			Toaster.OpenArchive = true;

		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			Toaster.Title = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name;

			if ( external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Resources.Exists( Resource ) )
				Toaster.Icon = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Resources.Item( Resource ).Avatar;
			else
				Toaster.Icon = 'unknown-soldier.png';
		}
		else
		{
			Toaster.Icon = external.globals( 'ClientRoster' ).GetAvatar( Resource.toLowerCase(), ShortAddress );
			if ( ! Toaster.Icon.length )
				Toaster.Icon = 'unknown-soldier.png';

			if ( external.globals( 'ClientServices' ).Services.Exists( Host ) && external.globals( 'ClientServices' ).Services.Item( Host ).Options & 0x0001 )
				Toaster.Title = external.globals( 'ClientServices' ).Services.Item( Host ).Name;
			if ( ! Toaster.Title.length )
				Toaster.Title = ShortAddress;
		}

		if ( Message.WantsDisplayed )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<message><x xmlns="jabber:x:event"><displayed/><id/></x></message>' );
			if ( Message.Id.length )
				dom.selectSingleNode( '/message/x/id' ).text = Message.Id;
			dom.documentElement.setAttribute( 'to', Message.From );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		Toaster.Show();
	}

	/* Receive all URLs
	 */
	else if ( Message.OOB.length )
	{
		if ( Message.WantsDisplayed )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<message><x xmlns="jabber:x:event"><displayed/><id/></x></message>' );
			if ( Message.Id.length )
				dom.selectSingleNode( '/message/x/id' ).text = Message.Id;
			dom.documentElement.setAttribute( 'to', Message.From );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		var Host		= Message.FromAddress.Host;
		var Resource	= Message.FromAddress.Resource;
		var Title		= '';
		var Icon		= '';

		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			Title = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Name;

			if ( external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Resources.Exists( Resource ) )
				Icon = external.globals( 'ClientRoster' ).Items.Item( ShortAddress ).Resources.Item( Resource ).Avatar;
			else
				Icon = 'unknown-soldier.png';
		}
		else
		{
			Icon = external.globals( 'ClientRoster' ).GetAvatar( Resource.toLowerCase(), ShortAddress );
			if ( ! Icon.length )
				Icon = 'unknown-soldier.png';

			if ( external.globals( 'ClientServices' ).Services.Exists( Host ) && external.globals( 'ClientServices' ).Services.Item( Host ).Options & 0x0001 )
				Title = external.globals( 'ClientServices' ).Services.Item( Host ).Name;
			if ( ! Title.length )
				Title = ShortAddress;
		}

		while ( Message.OOB.length )
		{
			var url  = Message.OOB.shift();
			var desc = Message.OOB.shift();
			if ( url.substr( 0, 7 ) == 'http://' || url.substr( 0, 8 ) == 'https://' || url.substr( 0, 7 ) == 'mailto:' || url.substr( 0, 5 ) == 'xmpp:' )
			{
				history_add( ShortAddress, Message.Stamp, encodeURI( url ) + ( desc == url ? '' : '\n' + desc ), true );
				var Toaster				= new Headline();
				Toaster.Message			= desc.length ? desc : url;
				Toaster.OpenHyperlink	= url;
				Toaster.Title			= Title;
				if ( Icon.length )
					Toaster.Icon = Icon;
				Toaster.Show();
			}
		}
	}

	/* Pass the message to the chat session pool and save in the history
	 */
	else if ( Message.Type == 'chat' || Message.Type == 'normal' || Message.Type == '' )
	{
		history_add( ShortAddress, Message.Stamp, Message.Body, true );
		var Event = new ChatSessionEvent();
		Event.Type = 'message';
		Event.Address = Message.FromAddress;
		Event.Payload = Message;
		external.globals( 'ChatSessionPool' ).AddEvent( Event );
	}
}
