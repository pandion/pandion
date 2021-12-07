/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
var gContainer = null;

function Begin ()
{
	external.globals( 'Translator' ).TranslateWindow( 'conference-container', document );

	/* Event handlers for text selecting and input area resizing
	 */
	document.attachEvent( 'onselectionchange', DisableButton );
	document.attachEvent( 'onselectstart', SelectionFilter );
	document.attachEvent( 'ondragstart', SelectionFilter );
	window.attachEvent( 'onresize', ResizeCheck );
	external.wnd.setActivationHandler( 'OnWindowActivate' );

	/* Initialize the container
	 */
	var SessionPool = external.globals( 'ConferenceSessionPool' );
	gContainer = new SessionContainer( SessionPool );
	gContainer.Name = external.wnd.params[1];
	gContainer.InputUpdate();
	SessionPool.AddContainer( gContainer );

	/* Set up the menu bar
	 */
	external.wnd.setMenuHandler( 'MenuBarSelect' );
	MenuBarUpdate();
	
	/* Spell Check
	*/
	gContainer.SpellCheck( );

	/* Restore the size and position of the window
	 */
	var cfg = external.globals( 'cfg' );
	var TabbedWindows = cfg( 'tabbedchat' ).toString() == 'true';
	cfg( 'conference_width' ) = Math.max( 180, parseInt( cfg( 'conference_width' ), 10 ) );
	cfg( 'conference_height' ) = Math.max( 210, parseInt( cfg( 'conference_height' ), 10 ) );
	cfg( 'conference_left' ) = parseInt( cfg( 'conference_left' ), 10 ) + ( TabbedWindows ? 0 : 20 );
	cfg( 'conference_top' ) = parseInt( cfg( 'conference_top' ), 10 ) + ( TabbedWindows ? 0 : 20 );

	external.wnd.setPos( cfg( 'conference_left' ), cfg( 'conference_top' ) );
	external.wnd.setSize( cfg( 'conference_width' ), cfg( 'conference_height' ) );

	if ( ! external.IsRectOnMonitor(
		cfg( 'conference_top' ),
		cfg( 'conference_left' ) + cfg( 'conference_width' ),
		cfg( 'conference_top' ) + cfg( 'conference_height' ),
		cfg( 'conference_left' )
	) )
		external.wnd.setPos( 200, 150 );

	if ( cfg( 'conference_maximized' ).toString() == 'true' )
		external.wnd.Maximize();

	/* Restore the size of the input area
	 */
	document.getElementById( 'send-text' ).parentNode.parentNode.height = isNaN( parseInt( external.globals( 'cfg' ).Item( 'textinputheight' ), 10 ) ) ? 60 : Math.max( 60, parseInt( external.globals( 'cfg' ).Item( 'textinputheight' ), 10 ) );

	/* Load all pending trackers
	 */
	var TrackerNames = ( new VBArray( SessionPool.TrackersLoading.Keys() ) ).toArray();
	for ( var i = SessionPool.TrackersLoading.Count - SessionPool.ContainersLoading.Count - 1; i >= 0; i-- )
		gContainer.CreateTracker( SessionPool.TrackersLoading.Item( TrackerNames[i] ) );
}

function End ()
{
	/* Stop receiving new trackers and events
	 */
	gContainer.Clear();

	/* Remember window size and position
	 */
	var cfg = external.globals( 'cfg' );
	cfg( 'conference_maximized' ) = !!external.wnd.isMaximized;
	cfg( 'conference_left' ) = Math.max( 0, external.wnd.left );
	cfg( 'conference_top' ) = Math.max( 0, external.wnd.top );
	if ( external.wnd.width > 100 && ! external.wnd.isMaximized )
		cfg( 'conference_width' ) = external.wnd.width;
	if ( external.wnd.height > 100 && ! external.wnd.isMaximized )
		cfg( 'conference_height' ) = external.wnd.height;
	external.wnd.params[0].SettingsSave();
}

function SessionContainer ( SessionPool )
{
	this.ActiveTrackerAddress = '';
	this.DOM = window;
	this.HTMLTabArea = document.getElementById( 'tab-area' );
	this.HTMLTabBar = document.getElementById( 'tab-bar' );
	this.Name = '';
	this.SessionPool = SessionPool;
	this.Trackers = new ActiveXObject( 'Scripting.Dictionary' );

	this.Clear = Clear;
	this.CreateTracker = CreateTracker;
	this.InputUpdate = InputUpdate;
	this.LanguageUpdate = LanguageUpdate;
	this.MenuBarUpdate = MenuBarUpdate;
	this.SpellCheck = SpellCheck;
	this.TrackerCreated = TrackerCreated;

	/* Create a tracker object, draw a tab
	 * TrackerCreated will be fired when the tracker is ready to receive events
	 */
	function CreateTracker ( room )
	{
		var ShortAddress = room.address.ShortAddress();
		var Tracker = new SessionTracker( room.address );
		Tracker.Container = this;
		Tracker.HTMLArea = document.createElement( 'IFRAME' );
		Tracker.InviteGroup = room.inviteGroup
		Tracker.InviteJid = room.inviteJid;
		Tracker.Password = room.password;

		var BackgroundPath = external.globals( 'usersdir' ) + 'Backgrounds\\' + Tracker.Background + '\\';
		if ( external.FileExists( BackgroundPath + 'index.html' ) )
			BackgroundPath += 'index.html';
		else
		{
			external.globals( 'cfg' ).Item( 'background' ) = '';
			Tracker.Background = '';
			BackgroundPath = 'chat-messages.html';
		}

		with ( Tracker.HTMLArea )
		{
			id = 'iframe-' + ShortAddress;
			style.display = 'none';
			height = '100%';
			width = '100%';
			src = BackgroundPath;
		}
		Tracker.HTMLArea.SessionTracker = Tracker;
		this.HTMLTabArea.insertAdjacentElement( 'beforeEnd', Tracker.HTMLArea );
		document.frames( 'iframe-' + ShortAddress ).SessionTracker = Tracker;

		Tracker.DrawButton();
		Tracker.DrawContainerInfo();
	}

	/* Register the tracker with the session pool to receive all queued events
	 */
	function TrackerCreated ( Tracker )
	{
		this.Trackers.Add( Tracker.Address.ShortAddress(), Tracker );
		this.SessionPool.AddTracker( Tracker );
		Tracker.Activate();
		document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).document.body.title = external.globals( 'Translator' ).Translate( 'conference-container', 'tt-messages' );

		/* Join the conference by sending initial presence
		 */
		Tracker.SendPresence( external.globals( 'cfg' ).Item( 'lastmode' ), external.globals( 'cfg' ).Item( 'lastmsg' ) );
	}

	/* Get out of the session pool and remove all trackers from this container
	 */
	function Clear ()
	{
		this.SessionPool.DeleteContainer( this );

		var TrackerNames = ( new VBArray( this.Trackers.Keys() ) ).toArray();
		for ( var i = 0; i < TrackerNames.length; ++i )
			this.Trackers.Item( TrackerNames[i] ).Clear();
	}

	/* Reload the user interface translation
	 */
	function LanguageUpdate ()
	{
		external.globals( 'Translator' ).TranslateWindow( 'conference-container', document );
		MenuBarUpdate();
		if ( this.Trackers.Exists( this.ActiveTrackerAddress ) )
		{
			this.Trackers.Item( this.ActiveTrackerAddress ).DrawContainerInfo();
			this.Trackers.Item( this.ActiveTrackerAddress ).RefreshOccupants();
		}
	}
	
	/* Enable/Disable Spell Check 
	*/
	function SpellCheck( )
	{
		if('spellcheck' in document.createElement('textarea')) //Verify is supported
		{
			var cfg = external.globals( 'cfg' );
			var spell = cfg( 'spellcheck' ).toString() == 'true';
			
			var sendtext = document.getElementById( "send-text" );
			sendtext.spellcheck = spell;
			sendtext.placeholder = "spellcheck='" + spell + "'";
		}
	}

	/* Load the configured font settings for the text input field
	 * Disable input field if not connected to IM server
	 */
	function InputUpdate ()
	{
		var cfg = external.globals( 'cfg' );
		with ( document.getElementById( 'send-text' ).style )
		{
			color = cfg( 'user_dialog_color' );
			fontFamily = cfg( 'user_dialog_face' );
			fontSize = cfg( 'user_dialog_size' );
			fontWeight = cfg( 'user_dialog_bold' );
			fontStyle = cfg( 'user_dialog_italic' );
			textDecorationLineThrough = cfg( 'user_dialog_strike' ).toString() == 'true';
			textDecorationUnderline = cfg( 'user_dialog_underline' ).toString() == 'true';
		}
		if ( external.globals( 'XMPPConnected' ) && this.Trackers.Exists( this.ActiveTrackerAddress ) && this.Trackers.Item( this.ActiveTrackerAddress ).Occupants.Exists( this.Trackers.Item( this.ActiveTrackerAddress ).Address.Resource ) )
		{
			document.getElementById( 'send-text' ).disabled = false;
			document.getElementById( 'btn-send' ).disabled = ( /^\s*$/ ).test( document.getElementById( 'send-text' ).value );
		}
		else
		{
			document.getElementById( 'send-text' ).disabled = true;
			document.getElementById( 'btn-send' ).disabled = true;
		}
	}
}

/* Tracker parses the events and manages its tabbed conversation
 */
function SessionTracker ( Address )
{
	this.Address = Address;
	this.Autoconnect = true;
	this.Background = external.globals( 'cfg' ).Item( 'background' );
	this.Container = null;
	this.HookIQ = null;
	this.HTMLArea = false;
	this.HTMLButton = false;
	this.HTMLModerators = document.createElement( 'DIV' );
	this.HTMLParticipants = document.createElement( 'DIV' );
	this.HTMLVisitors = document.createElement( 'DIV' );
	this.InviteGroup = '';
	this.InviteJid = '';
	this.IsActive = false;
	this.IsFlashing = false;
	this.Name = Address.User;
	this.Occupants = new ActiveXObject( 'Scripting.Dictionary' );
	this.Password = '';
	this.Reloading = false;
	this.Subject = external.globals( 'Translator' ).Translate( 'conference-container', 'default_subject' );
	this.Text = '';

	this.Activate = Activate;
	this.BackgroundUpdate = BackgroundUpdate;
	this.Clear = Clear;
	this.Close = Close;
	this.Deactivate = Deactivate;
	this.DispatchEvent = DispatchEvent;
	this.DrawButton = DrawButton;
	this.DrawContainerInfo = DrawContainerInfo;
	this.Flash = Flash;
	this.RefreshOccupants = RefreshOccupants;
	this.SendInvite = SendInvite;
	this.SendMessage = SendMessage;
	this.SendPresence = SendPresence;
	this.AppendText = AppendText;
	
	document.getElementById( 'txt-visitors' ).insertAdjacentElement( 'afterEnd', this.HTMLVisitors );
	document.getElementById( 'txt-moderators' ).insertAdjacentElement( 'afterEnd', this.HTMLModerators );
	document.getElementById( 'txt-participants' ).insertAdjacentElement( 'afterEnd', this.HTMLParticipants );

	/* Default user name
	 */
	if ( ! external.globals( 'cfg' ).Item( 'conferencenick' ).length )
		external.globals( 'cfg' ).Item( 'conferencenick' ) = external.globals( 'cfg' ).Item( 'nick' );
	if ( ! this.Address.Resource.length )
		this.Address.Resource = external.globals( 'cfg' ).Item( 'conferencenick' );

	/* Handle events from the SessionPool
	 */
	function DispatchEvent ( Event )
	{
		/* Message
		 */
		if ( Event.Type == 'message' )
		{
			/* Error
			 */
			if ( Event.Payload.Type == 'error' )
			{
				if ( Event.Payload.ErrorMessage.length )
				{
					/* Forward the event to the message viewer
					 */
					document.frames( 'iframe-' + this.Address.ShortAddress() ).onError( Event.Payload );

					/* Notify the user of this message
					 */
					this.Flash( 4 );
				}
			}
			else
			{
				/* Update room subject
				 */
				if ( Event.Payload.Subject.length && Event.Payload.Type == 'groupchat' )
				{
					this.Subject = Event.Payload.Subject;
					this.DrawContainerInfo();
				}

				/* Display message
				 */
				if ( Event.Payload.Body.length )
				{
					/* Forward the event to the message viewer
					 */
					document.frames( 'iframe-' + this.Address.ShortAddress() ).onMessage( Event.Payload );

					/* Notify the user of this message
					 */
					this.Flash( 4 );
				}
			}
		}
		/* Presence
		 */
		else if ( Event.Type == 'presence' )
		{
			/* Error
			 */
			if ( Event.Payload.Type == 'error' )
			{
				if ( Event.Payload.FromAddress.Resource == this.Address.Resource )
				{
					var Node = null;
					if ( Node = Event.Payload.XMLDOM.selectSingleNode( '/presence/error[@code]' ) )
					{
						var ErrorCode = Node.getAttribute( 'code' );
						var ErrorDescription = Node.text;
						var ErrorNode = Node.selectSingleNode( '*[@xmlns = "urn:ietf:params:xml:ns:xmpp-stanzas"]' );

						if ( ErrorNode && ErrorNode.selectSingleNode( 'text[@xmlns = "urn:ietf:params:xml:ns:xmpp-streams"]' ) )
							ErrorDescription = ErrorNode.selectSingleNode( 'text[@xmlns = "urn:ietf:params:xml:ns:xmpp-streams"]' ).text;
						if ( ErrorNode && ! ErrorDescription.length )
							ErrorDescription = external.globals( 'Translator' ).Translate( 'conference-container', 'error-' + ErrorNode.tagName );
						if ( ! ErrorDescription.length )
							ErrorDescription = external.globals( 'Translator' ).Translate( 'conference-container', 'unrecognized-error' );

						var MessageDom = new ActiveXObject( 'Msxml2.DOMDocument' );
						MessageDom.loadXML( '<message><error/></message>' );
						MessageDom.documentElement.setAttribute( 'to', external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
						MessageDom.documentElement.setAttribute( 'from', this.Address.ShortAddress() );
						MessageDom.documentElement.setAttribute( 'type', 'error' );
						MessageDom.documentElement.firstChild.setAttribute( 'code', ErrorCode );
						MessageDom.documentElement.firstChild.text = ErrorDescription;

						var Message = new XMPPMessage();
						Message.FromDOM( MessageDom );
						document.frames( 'iframe-' + this.Address.ShortAddress() ).onError( Message );

						if ( ErrorCode == '401' || ( ErrorNode && ErrorNode.tagName == 'not-authorized' ) )
							dial_conference_connect_password( this );
						else if ( ErrorCode == '409' || ( ErrorNode && ErrorNode.tagName == 'conflict' ) )
							dial_conference_connect_name( this );
					}
				}
			}
			/* Remove
			 */
			else if ( Event.Payload.Type == 'unavailable' )
			{
				if ( this.Occupants.Exists( Event.Payload.FromAddress.Resource ) )
				{
					/* Banned
					 */
					if ( Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/status[@code = "301"]' ) )
					{
						var Actor = Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/item/actor[@jid]' );
						if ( Actor )
						{
							Actor = Actor.getAttribute( 'jid' ).toLowerCase();
							if ( external.globals( 'ClientRoster' ).Items.Exists( Actor ) )
								Actor = external.globals( 'ClientRoster' ).Items.Item( Actor ).Name;
							else if ( Actor == external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) )
								Actor = external.globals( 'cfg' ).Item( 'nick' );
							ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'banned-by', [ Event.Payload.FromAddress.Resource, Actor ] ) );
						}
						else
							ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'banned', [ Event.Payload.FromAddress.Resource ] ) );
					}
					/* Changing nickname
					 */
					else if ( Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/status[@code = "303"]' ) )
					{
						var Actor = Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/item[@nick]' );
						if ( Actor )
						{
							var OldName = Event.Payload.FromAddress.Resource;
							var NewName = Actor.getAttribute( 'nick' );
							ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'renaming', [ OldName, NewName ] ) );
						}
					}
					/* Kicked
					 */
					else if ( Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/status[@code = "307"]' ) )
					{
						var Actor = Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/item/actor[@jid]' );
						if ( Actor )
						{
							Actor = Actor.getAttribute( 'jid' ).toLowerCase();
							if ( external.globals( 'ClientRoster' ).Items.Exists( Actor ) )
								Actor = external.globals( 'ClientRoster' ).Items.Item( Actor ).Name;
							else if ( Actor == external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) )
								Actor = external.globals( 'cfg' ).Item( 'nick' );
							ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'kicked-by', [ Event.Payload.FromAddress.Resource, Actor ] ) );
						}
						else
							ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'kicked', [ Event.Payload.FromAddress.Resource ] ) );
					}
					/* Leaving
					 */
					else
						ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'leaving', [ Event.Payload.FromAddress.Resource ] ) );

					/* Clear occupant
					 */
					this.Occupants.Item( Event.Payload.FromAddress.Resource ).Clear();
					this.Occupants.Remove( Event.Payload.FromAddress.Resource );
				}
			}
			/* Update
			 */
			else if ( this.Occupants.Exists( Event.Payload.FromAddress.Resource ) )
			{
				this.Occupants.Item( Event.Payload.FromAddress.Resource ).Update( Event.Payload );
			}
			/* Add
			 */
			else if ( Event.Payload.FromAddress.Resource.length )
			{
				this.Occupants.Add( Event.Payload.FromAddress.Resource, new ConferenceParticipant( this, Event.Payload ) );
				if ( Event.Payload.FromAddress.Resource == this.Address.Resource )
				{
					ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'connected' ) );
					this.SendInvite( [ this.InviteJid ], this.InviteGroup );
				}
				else
					ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'joining', [ Event.Payload.FromAddress.Resource ] ) );
			}

			/* Enable or disable input area, update menubar
			 */
			if ( Event.Payload.FromAddress.Resource == this.Address.Resource )
			{
				if ( Event.Payload.Type == 'unavailable' )
				{
					this.Autoconnect = false;
					var OccupantNames = ( new VBArray( this.Occupants.Keys() ) ).toArray();
					for ( var i = 0; i < OccupantNames.length; ++i )
						this.Occupants.Item( OccupantNames[i] ).Clear();
					this.Occupants.RemoveAll();
					ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'disconnected' ) );
				}

				/* Initial room configuration
				 */
				if ( Event.Payload.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/status[@code = "201"]' ) )
				{
					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<iq type="set"><query xmlns="http://jabber.org/protocol/muc#owner"><x xmlns="jabber:x:data" type="submit"/></query></iq>' );
					dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
					dom.documentElement.setAttribute( 'to', this.Address.ShortAddress() );
					external.wnd.params[0].warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}

				/* Update the user interface
				 */
				if ( this.IsActive )
				{
					MenuBarUpdate( 'actions' );
					this.Container.InputUpdate();
					if ( ! document.getElementById( 'send-text' ).disabled && external.wnd.isActive() )
						document.getElementById( 'send-text' ).focus();
				}
			}
		}
	}
	
	/* Append Text
	*/
	function AppendText( text ) 
	{		
		var sendtext = document.getElementById( 'send-text' );
	
		sendtext.value = ( sendtext.value.length ? sendtext.value + '\n' : '' ) + ( text.length ? text : "" );
				
		if ( ! sendtext.disabled )
		{
			var range;
			var caretPos = sendtext.value.length
			if (sendtext.createTextRange) 
			{
				range = sendtext.createTextRange();
				range.move('character', caretPos);
				range.select();
			} else 
			{
				sendtext.focus();
				if (sendtext.selectionStart !== undefined) 
				{
					sendtext.setSelectionRange(caretPos, caretPos);
				}
			}
		}
	}
	
	/* Use audio-visual signals to attract attention
	 */
	function Flash ( Times )
	{
		/* Play sound
		 */
		if ( ( ! external.wnd.isActive() || ! this.IsActive ) && external.globals( 'cfg' ).Item( 'soundmessage' ).toString() == 'true' && ( external.globals( 'cfg' ).Item( 'lastmode' ) < 2 || external.globals( 'cfg' ).Item( 'lastmode' ) == 5 ) )
			external.wnd.params[0].sound_play( external.globals( 'cfg' ).Item( 'soundmessagefile' ), false );
		/* Flash window
		 */
		if ( ! external.wnd.isActive() )
		{
			if ( ! this.IsActive )
				this.Activate();
			external.wnd.flash( 4 );
		}
		/* Blink tab icon
		 */
		if ( ! this.IsActive && ! this.IsFlashing )
			FlashTab( this.Address.ShortAddress(), Times * 2 );
	}

	/* Apply the automatically downloaded background
	 * Turn off background if uninstalled
	 */
	function BackgroundUpdate ()
	{
		var Path = external.globals( 'usersdir' ) + 'Backgrounds\\' + this.Background + '\\';
		if ( this.BackgroundLoading )
		{
			if ( external.FileExists( Path + 'index.html' ) )
			{
				this.BackgroundLoading = false;
				var ShortAddress = this.Address.ShortAddress();
				this.Container.SessionPool.TrackersLoading.Add( ShortAddress, { 'address': this.Address, 'password': this.Password } );
				this.Container.SessionPool.DeleteTracker( this );
				this.Container.Trackers.Remove( ShortAddress );

				this.Deactivate();
				this.Reloading = true;
				this.HTMLArea.removeNode( true );
				this.HTMLArea = document.createElement( 'IFRAME' );
				with ( this.HTMLArea )
				{
					id = 'iframe-' + ShortAddress;
					style.display = 'none';
					height = '100%';
					width = '100%';
					src = Path + 'index.html';
				}
				this.HTMLArea.SessionTracker = this;
				this.Container.HTMLTabArea.insertAdjacentElement( 'beforeEnd', this.HTMLArea );
				document.frames( 'iframe-' + ShortAddress ).SessionTracker = this;
			}
		}
		else if ( this.Background.length && ! external.FileExists( Path + 'index.html' ) )
		{
			this.Background = '';
			this.BackgroundLoading = false
			var ShortAddress = this.Address.ShortAddress();
			this.Container.SessionPool.TrackersLoading.Add( ShortAddress, { 'address': this.Address } );
			this.Container.SessionPool.DeleteTracker( this );
			this.Container.Trackers.Remove( ShortAddress );

			this.Deactivate();
			this.Reloading = true;
			this.HTMLArea.removeNode( true );
			this.HTMLArea = document.createElement( 'IFRAME' );
			with ( this.HTMLArea )
			{
				id = 'iframe-' + ShortAddress;
				style.display = 'none';
				height = '100%';
				width = '100%';
				src = 'chat-messages.html';
			}
			this.HTMLArea.SessionTracker = this;
			this.Container.HTMLTabArea.insertAdjacentElement( 'beforeEnd', this.HTMLArea );
			document.frames( 'iframe-' + ShortAddress ).SessionTracker = this;
		}
	}

	/* Update the user interface
	 */
	function DrawContainerInfo ()
	{
		if ( this.IsActive )
		{
			MenuBarUpdate( 'actions' );
			DrawModeMessage( document.getElementById( 'mode-message' ), this.Subject, 'mode-message-link' );
			this.HTMLButton.children.item(1).innerText = this.Name;
			document.getElementById( 'mode-name' ).innerText = this.Name;
			document.getElementById( 'mode-address' ).innerText = '<' + this.Address.ShortAddress() + '>';
			document.getElementById( 'mode-address' ).style.display = this.Name == this.Address.ShortAddress() ? 'none' : 'inline';
			external.wnd.SetTitle( external.globals( 'Translator' ).Translate( 'conference-container', 'windowtitle', [ this.Name ] ) );
		}
	}

	/* Show or refresh the button
	 */
	function DrawButton ()
	{
		if ( this.HTMLButton )
		{
			/* Update button
			 */
			this.HTMLButton.className = this.IsActive ? 'tab-bar-button-active' : 'tab-bar-button';
			this.HTMLButton.children.item(1).className = this.IsActive ? 'tab-bar-text-active' : 'tab-bar-text';
			this.HTMLButton.children.item(2).style.display = this.IsActive ? 'inline' : 'none';
		}
		else
		{
			/* Create button
			 */
			this.HTMLButton = document.createElement( 'SPAN' );
			this.HTMLButton.className = this.IsActive ? 'tab-bar-button-active' : 'tab-bar-button';
			this.HTMLButton.SessionTracker = this;

			var Picture = document.createElement( 'IMG' );
			Picture.className = 'tab-avatar';
			Picture.src = '..\\images\\conference\\contacts.png';
			this.HTMLButton.insertAdjacentElement( 'beforeEnd', Picture );

			var Name = document.createElement( 'NOBR' );
			Name.className = this.IsActive ? 'tab-bar-text-active' : 'tab-bar-text';
			Name.innerText = this.Address.User;
			this.HTMLButton.insertAdjacentElement( 'beforeEnd', Name );

			var Close = document.createElement( 'IMG' );
			Close.title = external.globals( 'Translator' ).Translate( 'conference-container', 'tt-close' );
			Close.style.display = this.IsActive ? 'inline' : 'none';
			Close.src = '..\\images\\misc\\tab-bar-close-lite.gif';
			Close.className = 'tab-close';
			Close.attachEvent(
				'onmouseover',
				function ()
				{
					event.srcElement.src = '..\\images\\misc\\tab-bar-close-over-lite.gif';
				}
			);
			Close.attachEvent(
				'onmouseout',
				function ()
				{
					event.srcElement.src = '..\\images\\misc\\tab-bar-close-lite.gif';
				}
			);
			Close.attachEvent(
				'onclick',
				function ()
				{
					event.cancelBubble = true;
					if ( event.srcElement.tagName == 'SPAN' )
						event.srcElement.SessionTracker.Close();
					else
						event.srcElement.parentNode.SessionTracker.Close();
				}
			);
			Close.attachEvent(
				'onmousedown',
				function ()
				{
					event.cancelBubble = true;
				}
			);
			Close.attachEvent(
				'onmouseup',
				function ()
				{
					event.cancelBubble = true;
				}
			);
			this.HTMLButton.insertAdjacentElement( 'beforeEnd', Close );

			this.HTMLButton.attachEvent("onmousedown", function () {
				if (event.button !== 1)
					return;
				else if (event.srcElement.tagName === "SPAN")
					event.srcElement.SessionTracker.Activate(true);
				else
					event.srcElement.parentNode.SessionTracker.Activate(true);
			});
			this.HTMLButton.attachEvent("onmouseup", function () {
				if (event.button === 1) {
					if (!document.getElementById("send-text").disabled) {
						document.getElementById("send-text").blur();
						document.getElementById("send-text").focus();
					}
				} else if (event.button === 4) {
					if (event.srcElement.tagName === "SPAN")
						event.srcElement.SessionTracker.Close();
					else
						event.srcElement.parentNode.SessionTracker.Close();
				}
			});

			this.Container.HTMLTabBar.insertAdjacentElement( 'beforeEnd', this.HTMLButton );
			if ( external.globals( 'cfg' ).Item( 'tabbedchat' ).toString() != 'true' )
				this.Container.HTMLTabBar.style.display = this.Container.HTMLTabBar.childNodes.length ? 'inline' : 'none';
		}
	}

	/* Close the tracker and clear up after it
	 */
	function Close ()
	{
		this.Clear();
		var TrackerNames = ( new VBArray( this.Container.Trackers.Keys() ) ).toArray();
		if ( TrackerNames.length )
			this.Container.Trackers.Item( TrackerNames[ TrackerNames.length - 1 ] ).Activate();
		else
			setTimeout( 'external.wnd.close()', 0 );
	}

	/* Get out of the session pool, send unavailable presence, remove the tab, and stop waiting for last online time
	 */
	function Clear ()
	{
		if ( external.globals( 'XMPPConnected' ) && this.Occupants.Exists( this.Address.Resource ) )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence type="unavailable"/>' );
			dom.documentElement.setAttribute( 'to', this.Address.LongAddress() );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		if ( external.windows.Exists( 'conference_invite/' + this.Address.ShortAddress() ) )
			external.windows( 'conference_invite/' + this.Address.ShortAddress() ).close();

		if ( external.windows.Exists( 'conference_subject/' + this.Address.ShortAddress() ) )
			external.windows( 'conference_subject/' + this.Address.ShortAddress() ).close();

		if ( external.windows.Exists( 'conference_configuration/' + this.Address.ShortAddress() ) )
			external.windows( 'conference_configuration/' + this.Address.ShortAddress() ).close();

		if ( external.windows.Exists( 'conference_password/' + this.Address.ShortAddress() ) )
			external.windows( 'conference_password/' + this.Address.ShortAddress() ).close();

		if ( external.windows.Exists( 'conference_name/' + this.Address.ShortAddress() ) )
			external.windows( 'conference_name/' + this.Address.ShortAddress() ).close();

		if ( this.IsActive )
			this.Deactivate();

		if ( this.HookIQ )
			this.HookIQ.Destroy();

		var OccupantNames = ( new VBArray( this.Occupants.Keys() ) ).toArray();
		for ( var i = 0; i < OccupantNames.length; ++i )
			this.Occupants.Item( OccupantNames[i] ).Clear();

		this.Container.SessionPool.DeleteTracker( this );
		this.Container.Trackers.Remove( this.Address.ShortAddress() );
		if ( this.Container.ActiveTrackerAddress == this.Address.ShortAddress() )
			this.Container.ActiveTrackerAddress = '';

		this.HTMLArea.removeNode( true );
		this.HTMLButton.removeNode( true );
		this.HTMLVisitors.removeNode( true );
		this.HTMLModerators.removeNode( true );
		this.HTMLParticipants.removeNode( true );
		this.Occupants.RemoveAll();
		if ( external.globals( 'cfg' ).Item( 'tabbedchat' ).toString() != 'true' )
			this.Container.HTMLTabBar.style.display = this.Container.HTMLTabBar.childNodes.length > 1 ? 'inline' : 'none';
	}

	/* Turn off the tab button, hide the conversation, and store the message being written
	 */
	function Deactivate ()
	{
		this.IsActive = false;
		this.DrawButton();
		this.HTMLArea.style.display = 'none';
		this.HTMLVisitors.style.display = 'none';
		this.HTMLModerators.style.display = 'none';
		this.HTMLParticipants.style.display = 'none';
		document.getElementById( 'txt-visitors' ).style.display = 'none';
		document.getElementById( 'txt-moderators' ).style.display = 'none';
		document.getElementById( 'txt-participants' ).style.display = 'none';
		this.Text = document.getElementById( 'send-text' ).value;
		document.getElementById( 'send-text' ).value = '';
	}

	/* Deactivate te previously active tab, turn on the button active, show the conversation, and restore the message being written
	 */
	function Activate ()
	{
		external.wnd.focus();
		if ( external.wnd.width >= screen.availWidth || external.wnd.height >= screen.availHeight )
			external.wnd.Maximize();
		else
			external.wnd.restore();

		if ( ! this.IsActive )
		{
			/* Deactivate old tab
			 */
			if ( this.Container.Trackers.Exists( this.Container.ActiveTrackerAddress ) )
				this.Container.Trackers.Item( this.Container.ActiveTrackerAddress ).Deactivate();

			/* Activate new tab
			 */
			this.Container.ActiveTrackerAddress = this.Address.ShortAddress();
			this.IsActive = true;
			this.DrawButton();
			this.DrawContainerInfo();
			this.HTMLArea.style.display = 'block';
			this.HTMLVisitors.style.display = 'block';
			this.HTMLModerators.style.display = 'block';
			this.HTMLParticipants.style.display = 'block';
			this.HTMLButton.children.item(1).style.fontStyle = '';
			if ( this.HTMLVisitors.children.length )
				document.getElementById( 'txt-visitors' ).style.display = '';
			if ( this.HTMLModerators.children.length )
				document.getElementById( 'txt-moderators' ).style.display = '';
			if ( this.HTMLParticipants.children.length )
				document.getElementById( 'txt-participants' ).style.display = '';
			ResizeStopTracking();

			/* The TEXTAREA.value must change otherwise the next OnPropertyChange event is not fired
			 */
			document.getElementById( 'send-text' ).value = Math.random();
			document.getElementById( 'send-text' ).value = this.Text;
			this.Text = '';

			this.Container.InputUpdate();
			if ( ! document.getElementById( 'send-text' ).disabled )
				document.getElementById( 'send-text' ).focus();
		}
	}

	/* Send presence to the chatroom
	 */
	function SendPresence ( Mode, Status )
	{
		if ( ! document.getElementById( 'send-text' ).disabled || this.Autoconnect )
		{
			var Show = '', Type = '';

			switch ( Mode )
			{
				case 1: Show = 'chat'; break;
				case 2: Show = 'away'; break;
				case 3: Show = 'xa'; break;
				case 4: Show = 'dnd'; break;
				case 5: Type = 'invisible'; break;
			}

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence><x xmlns="jabber:x:avatar"><hash/></x></presence>' );
			dom.selectSingleNode( '/presence/x/hash' ).text = external.globals( 'cfg' ).Item( 'avatar' );
			dom.documentElement.setAttribute( 'to', this.Address.LongAddress() );

			/* Entering conference
			 */
			if ( document.getElementById( 'send-text' ).disabled )
			{
				/* Declare MUC support
				 */
				var tag = dom.createElement( 'x' );

				/* Add password
				 */
				if ( this.Password.length )
				{
					var pass = dom.createElement( 'password' );
					pass.text = this.Password;
					tag.appendChild( pass );
				}

				tag.setAttribute( 'xmlns', 'http://jabber.org/protocol/muc' );
				dom.documentElement.insertBefore( tag, dom.documentElement.firstChild );

				ServerMessage( this, external.globals( 'Translator' ).Translate( 'conference-container', 'connecting' ) );
			}

			if ( Status.length )
			{
				var tag = dom.createElement( 'status' );
				tag.text = Status;
				dom.documentElement.appendChild( tag );
			}

			if ( Show.length )
			{
				var tag = dom.createElement( 'show' );
				tag.text = Show;
				dom.documentElement.appendChild( tag );
			}

			if ( Type.length )
			{
				dom.documentElement.setAttribute( 'type', Type );
			}

			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
	}

	/* Send MUC invitation messages
	 */
	function SendInvite ( addresses, group )
	{
		if ( external.globals( 'ClientRoster' ).Groups.Exists( group ) )
		{
			var jids = ( new VBArray( external.globals( 'ClientRoster' ).Groups.Item( group ).Items.Keys() ) ).toArray();
			for ( var i in jids )
				if ( external.globals( 'ClientRoster' ).Items.Item( jids[i] ).Resources.Count > 0 )
					addresses.push( jids[i] );
		}
		for ( var i in addresses )
			if ( addresses[i] != '' )
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<message><x><invite/></x></message>' );
				dom.documentElement.setAttribute( 'to', this.Address.ShortAddress() );
				dom.documentElement.firstChild.firstChild.setAttribute( 'to', addresses[i] );
				dom.documentElement.firstChild.setAttribute( 'xmlns', 'http://jabber.org/protocol/muc#user' );
				external.wnd.params[0].warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
	}

	/* Update the list of occupants
	 */
	function RefreshOccupants ()
	{
		var OccupantNames = ( new VBArray( this.Occupants.Keys() ) ).toArray();
		for ( var i = 0; i < OccupantNames.length; ++i )
			this.Occupants.Item( OccupantNames[i] ).Update();
	}
}

/* Store and display information about a person in this conference room
 */
function ConferenceParticipant ( Tracker, Presence )
{
	this.Show = '';
	this.Name = Presence.FromAddress.Resource;
	this.Role = '';
	this.Avatar = '';
	this.Status = '';
	this.Address = '';
	this.Tracker = Tracker;
	this.AvatarHook = null;
	this.Affiliation = '';
	this.HTMLElement = document.createElement( 'NOBR' );

	this.Clear = Clear;
	this.Update = Update;

	/* Create the DOM structure for this participant
	 * <NOBR>
	 *   <IMG/>  <SPAN>Nickname</SPAN>  <SPAN>Away|Busy</SPAN>
	 *           <SPAN>Status</SPAN>
	 * </NOBR>
	 */
	this.HTMLElement.className = 'roster-resource';

	this.HTMLElement.attachEvent(
		'onmouseover',
		function ()
		{
			var Element = event.toElement;
			while ( Element.tagName != 'NOBR' )
				Element = Element.parentNode;
			Element.children(1).style.textDecoration = 'underline';
		}
	);

	this.HTMLElement.attachEvent(
		'onmouseout',
		function ()
		{
			var Element = event.fromElement;
			while ( Element.tagName != 'NOBR' )
				Element = Element.parentNode;
			Element.children(1).style.textDecoration = 'none';
		}
	);

	this.HTMLElement.attachEvent(
		'onclick',
		function ()
		{
			var Element = event.srcElement;
			while ( Element.tagName != 'NOBR' )
				Element = Element.parentNode;
			var Occupant = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Occupants.Item( Element.children(1).innerText );
			if ( Occupant.Address.length )
				external.wnd.params[0].dial_chat( Occupant.Address );
			else
			{
				document.getElementById( 'send-text' ).focus();
				document.getElementById( 'send-text' ).value = '/' + Occupant.Name + ': ';
			}
		}
	);

	this.HTMLElement.attachEvent(
		'oncontextmenu',
		function ()
		{
			var Element = event.srcElement;
			while ( Element.tagName != 'NOBR' )
				Element = Element.parentNode;
			var Tracker = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress );
			var Name = Element.children(1).innerText;
			var Occupant = Tracker.Occupants.Item( Name );
			OccupantContextMenu( Occupant, event.screenX, event.screenY );
		}
	);

	var Avatar = document.createElement( 'IMG' );
	Avatar.className = 'roster-resource-avatar';
	Avatar.align = external.globals( 'Translator' ).Direction ? 'right' : 'left';
	this.HTMLElement.insertAdjacentElement( 'beforeEnd', Avatar );

	var Name = document.createElement( 'SPAN' );
	Name.className = 'roster-resource-name';
	Name.innerText = this.Name;
	this.HTMLElement.insertAdjacentElement( 'beforeEnd', Name );

	var Show = document.createElement( 'SPAN' );
	this.HTMLElement.insertAdjacentElement( 'beforeEnd', Show );

	var Status = document.createElement( 'SPAN' );
	Status.className = 'roster-resource-msg';
	this.HTMLElement.insertAdjacentElement( 'beforeEnd', Status );

	/* Find the correct location to display the room occupant
	 */
	document.getElementById( 'txt-participants' ).style.display = 'block';
	var NameLC = this.Name.toLowerCase();
	var List = this.Tracker.HTMLParticipants.children;
	var a = 0;
	var z = List.length;
	if ( z == 0 || NameLC > List.item( z - 1 ).children(1).innerText.toLowerCase() )
		this.Tracker.HTMLParticipants.insertAdjacentElement( 'beforeEnd', this.HTMLElement );
	else if ( NameLC < List.item(0).children(1).innerText.toLowerCase() )
		this.Tracker.HTMLParticipants.insertAdjacentElement( 'afterBegin', this.HTMLElement );
	else
	{
		while ( z > 1 )
		{
			var p = a + Math.floor( z / 2 );
			if ( NameLC < List.item( p ).children(1).innerText.toLowerCase() )
				z = p - a;
			else
			{
				a = p;
				z = Math.round( z / 2 );
			}
		}
		var Node = List.item( a );
		Node.insertAdjacentElement( 'afterEnd', this.HTMLElement );
	}

	/* Draw the HTML
	 */
	this.Update( Presence );

	/* Remove this participant from the list
	 */
	function Clear ()
	{
		if ( this.AvatarHook )
			this.AvatarHook.Destroy();
		this.HTMLElement.removeNode( true );
		if ( external.windows.Exists( 'conference_accesslevel/' + this.Tracker.Address.ShortAddress() + '/' + this.Name ) )
			external.windows( 'conference_accesslevel/' + this.Tracker.Address.ShortAddress() + '/' + this.Name ).close();
		if ( this.Tracker.IsActive )
		{
			if ( ! this.Tracker.HTMLVisitors.children.length )
				document.getElementById( 'txt-visitors' ).style.display = 'none';
			if ( ! this.Tracker.HTMLModerators.children.length )
				document.getElementById( 'txt-moderators' ).style.display = 'none';
			if ( ! this.Tracker.HTMLParticipants.children.length )
				document.getElementById( 'txt-participants' ).style.display = 'none';
		}
	}

	/* Refresh this participant's information and redraw the HTML
	 */
	function Update ( Presence )
	{
		if ( Presence )
		{
			this.Avatar = '';
			this.Show = Presence.Show;
			this.Status = Presence.Status.replace( /[\n\r]+/mg, ' - ' );

			var Hash = Presence.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "jabber:x:avatar"]/hash' );
			if ( Hash )
			{
				Hash = Hash.text.replace( /[^0-9a-zA-Z]/gm, '' );
				if ( Hash.length == 40 )
				{
					if ( external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + Hash ) )
						this.Avatar = Hash;
					else if ( ! this.AvatarHook )
					{
						this.AvatarHook = new XMPPHookIQ();
						this.AvatarHook.Window = external.wnd;
						this.AvatarHook.Callback = 'ReceiveAvatar';
						var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
						dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:avatar"/></iq>' );
						dom.documentElement.setAttribute( 'id', this.AvatarHook.Id );
						dom.documentElement.setAttribute( 'to', this.Tracker.Address.ShortAddress() + '/' + this.Name );
						external.wnd.params[0].warn( 'SENT: ' + dom.xml );
						external.XMPP.SendXML( dom );
					}
				}
			}

			var OriginalRole = this.Role;
			this.Role = Presence.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/item[@role and @affiliation]' );
			if ( this.Role )
			{
				this.Affiliation = this.Role.getAttribute( 'affiliation' );
				this.Role = this.Role.getAttribute( 'role' );
			}
			else
			{
				this.Affiliation = '';
				this.Role = '';
			}
			if ( OriginalRole != this.Role )
			{
				var HTMLList = this.Role == 'moderator' ? this.Tracker.HTMLModerators : this.Role == 'visitor' ? this.Tracker.HTMLVisitors : this.Tracker.HTMLParticipants;
				var NameLC = this.Name.toLowerCase();
				var List = HTMLList.children;
				var a = 0;
				var z = List.length;
				if ( z == 0 || NameLC > List.item( z - 1 ).children(1).innerText.toLowerCase() )
					HTMLList.insertAdjacentElement( 'beforeEnd', this.HTMLElement );
				else if ( NameLC < List.item(0).children(1).innerText.toLowerCase() )
					HTMLList.insertAdjacentElement( 'afterBegin', this.HTMLElement );
				else
				{
					while ( z > 1 )
					{
						var p = a + Math.floor( z / 2 );
						if ( NameLC < List.item( p ).children(1).innerText.toLowerCase() )
							z = p - a;
						else
						{
							a = p;
							z = Math.round( z / 2 );
						}
					}
					var Node = List.item( a );
					Node.insertAdjacentElement( 'afterEnd', this.HTMLElement );
				}
				if ( this.Tracker.IsActive )
				{
					document.getElementById( 'txt-visitors' ).style.display = this.Tracker.HTMLVisitors.children.length ? 'block' : 'none';
					document.getElementById( 'txt-moderators' ).style.display = this.Tracker.HTMLModerators.children.length ? 'block' : 'none';
					document.getElementById( 'txt-participants' ).style.display = this.Tracker.HTMLParticipants.children.length ? 'block' : 'none';
				}
			}

			this.Address = Presence.XMLDOM.selectSingleNode( '/presence/x[@xmlns = "http://jabber.org/protocol/muc#user"]/item[@jid]' );
			this.Address = this.Address ? this.Address.getAttribute( 'jid' ) : '';
		}

		var Children = this.HTMLElement.children;

		if ( ! this.Status.length || ( this.Show == 'away' && this.Status.toLowerCase() == 'away' ) || ( this.Show == 'dnd' && this.Status.toLowerCase() == 'busy' ) )
			this.Status = external.globals( 'Translator' ).Translate( 'main', 'cl_status_empty' );

		this.HTMLElement.title = this.Name + '\n' + ( this.Address.length ? this.Address + '\n' : '' ) + this.Status;
		this.HTMLElement.style.paddingBottom = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : '5px';
		Children.item(0).height = Children.item(0).width = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? 24 : 16;
		Children.item(1).style.fontWeight = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'normal';
		Children.item(3).style.display = external.globals( 'cfg' ).Item( 'contactlistdisplay' ) == 'detailed' ? '' : 'none';

		if ( this.Avatar.length )
			Children.item(0).src = external.globals( 'usersdir' ) + 'Avatars\\' + this.Avatar;
		else
			Children.item(0).src = '..\\images\\clients\\unknown-soldier.png';

		DrawModeMessage( Children.item(3), this.Status, 'roster-resource-link' );

		switch ( this.Show )
		{
			case 'dnd':
				Children.item(2).className = 'roster-resource-busy';
				Children.item(2).innerHTML = '&nbsp;- ' + external.globals( 'Translator' ).Translate( 'main', 'cl_busy' ) + '<BR>';
			break;
			case 'xa':
			case 'away':
				Children.item(2).className = 'roster-resource-away';
				Children.item(2).innerHTML = '&nbsp;- ' + external.globals( 'Translator' ).Translate( 'main', 'cl_away' ) + '<BR>';
			break;
			default:
				Children.item(2).className = 'roster-resource-away';
				Children.item(2).innerHTML = '<BR>';
		}

		if ( external.windows.Exists( 'conference_accesslevel/' + this.Tracker.Address.ShortAddress() + '/' + this.Name ) )
			external.windows( 'conference_accesslevel/' + this.Tracker.Address.ShortAddress() + '/' + this.Name ).Do( 'Refresh', null );
	}
}

/* Save and display the custom avatar
 */
function ReceiveAvatar ( iq )
{
	if ( iq.XMLDOM.selectSingleNode( '/iq[@type = "result"]/query[@xmlns = "jabber:iq:avatar"]/data' ) )
	{
		var ShortAddress = iq.FromAddress.ShortAddress();
		if ( gContainer.Trackers.Exists( ShortAddress ) )
		{
			var Tracker = gContainer.Trackers.Item( ShortAddress );
			if ( Tracker.Occupants.Exists( iq.FromAddress.Resource ) )
			{
				var Occupant = Tracker.Occupants.Item( iq.FromAddress.Resource );
				if ( Occupant.Avatar.length == 40 && ! external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + Occupant.Avatar ) )
				{
					Occupant.AvatarHook.Destroy();
					Occupant.AvatarHook = null;
					external.File( external.globals( 'usersdir' ) + 'Avatars\\' + Occupant.Avatar ).WriteBase64( iq.XMLDOM.selectSingleNode( '/iq[@type = "result"]/query[@xmlns = "jabber:iq:avatar"]/data' ).text );
					Occupant.Update();
				}
			}
		}
	}
}

/* Transmit the message
 */
function SendMessage ()
{
	var Tracker = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress );
	var Input = document.getElementById( 'send-text' ).value;

	/* Change nickname
	 */
	if ( Input.match( /\/name \S/ ) )
	{
		external.globals( 'cfg' ).Item( 'conferencenick' ) = Input.substr( 6 );
		var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
		for ( var i = 0; i < TrackerNames.length; ++i )
		{
			external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).Address.Resource = external.globals( 'cfg' ).Item( 'conferencenick' );
			external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).SendPresence( external.globals( 'cfg' ).Item( 'lastmode' ), external.globals( 'cfg' ).Item( 'lastmsg' ) );
		}
	}

	/* Group or personal message
	 */
	else
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<message><body/><html><body/></html><x xmlns="jisp:x:jep-0038"><name/></x></message>' );
		dom.documentElement.setAttribute( 'xml:lang', external.globals( 'language' ) );
		dom.documentElement.setAttribute( 'from', external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
		dom.selectSingleNode( '/message/x[@xmlns="jisp:x:jep-0038"]/name' ).text = external.globals( 'cfg' ).Item( 'emoticonset' );

		if ( Input.match( /^\/([^\:]+)\: \S/ ) && Tracker.Occupants.Exists( RegExp.$1 ) )
		{
			Input = Input.substr( RegExp.$1.length + 3 );
			dom.documentElement.setAttribute( 'type', 'chat' );
			dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() + '/' + RegExp.$1 );
		}
		else
		{
			dom.documentElement.setAttribute( 'type', 'groupchat' );
			dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() );
		}

		dom.documentElement.firstChild.text = Input;

		var HTMLBody = dom.documentElement.selectSingleNode( '/message/html/body' );
		var TextLines = Input.split( '\n' );
		HTMLBody.setAttribute( 'style', document.getElementById( 'send-text' ).style.cssText.toLowerCase() );
		HTMLBody.appendChild( dom.createTextNode( TextLines[0] ) );
		for ( var i = 1; i < TextLines.length; ++i )
		{
			HTMLBody.appendChild( dom.createElement( 'br' ) );
			HTMLBody.appendChild( dom.createTextNode( TextLines[i] ) );
		}
		dom.documentElement.selectSingleNode( 'html' ).setAttribute( 'xmlns', 'http://jabber.org/protocol/xhtml-im' );
		dom.documentElement.selectSingleNode( 'html' ).firstChild.setAttribute( 'xmlns', 'http://www.w3.org/1999/xhtml' );

		if ( dom.documentElement.getAttribute( 'type' ) == 'chat' )
		{
			var Message = new XMPPMessage();
			Message.FromDOM( dom );
			document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onMessage( Message );
		}

		external.wnd.params[0].warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Reset input field
	 */
	document.getElementById( 'send-text' ).value = '';
	if ( ! document.getElementById( 'send-text' ).disabled )
		document.getElementById( 'send-text' ).focus();
}

/* Change subject
 */
function dial_conference_subject ( Tracker )
{
	var ShortAddress = Tracker.Address.ShortAddress();
	if ( external.windows.Exists( 'conference_subject/' + ShortAddress ) )
		external.windows( 'conference_subject/' + ShortAddress ).focus();
	else
		with ( external.createWindow( 'conference_subject/' + ShortAddress, external.globals( 'cwd' ) + 'conference-subject.html', [ external.wnd.params[0], Tracker ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'conference-container', 'wnd_conference_subject' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\note.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 400, 80 );
			setPos( ( screen.availWidth - 400 ) / 2, ( screen.availHeight - 80 ) / 2 );
		}
}

/* Destroy room
 */
function dial_conference_destroy ( Tracker )
{
	if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'conference-container', 'destroy' ), external.globals( 'softwarename' ), 4 | 32 ) )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="set"><query xmlns="http://jabber.org/protocol/muc#owner"><destroy/></query></iq>' );
		dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
		dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() );
		external.wnd.params[0].warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
}

/* Configure room options
 */
function dial_conference_configuration ( Tracker )
{
	var ShortAddress = Tracker.Address.ShortAddress();
	if ( external.windows.Exists( 'conference_configuration/' + ShortAddress ) )
		external.windows( 'conference_configuration/' + ShortAddress ).focus();
	else
		with ( external.createWindow( 'conference_configuration/' + ShortAddress, external.globals( 'cwd' ) + 'conference-configuration.html', [ external.wnd.params[0], Tracker ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'conference-container', 'wnd_conference_configuration' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\tools.ico' );
			setSize( 385, 382 );
			MinHeight = 382;
			MinWidth = 385;
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}

/* Connect with a password
 */
function dial_conference_connect_password ( Tracker )
{
	var ShortAddress = Tracker.Address.ShortAddress();
	if ( external.windows.Exists( 'conference_password/' + ShortAddress ) )
		external.windows( 'conference_password/' + ShortAddress ).focus();
	else
		with ( external.createWindow( 'conference_password/' + ShortAddress, external.globals( 'cwd' ) + 'conference-password.html', [ Tracker ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'conference-container', 'wnd_conference_password' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\lock.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 300, 80 );
			setPos( ( screen.availWidth - 300 ) / 2, ( screen.availHeight - 80 ) / 2 );
		}
}

/* Change name
 */
function dial_conference_connect_name ( Tracker )
{
	var ShortAddress = Tracker.Address.ShortAddress();
	if ( external.windows.Exists( 'conference_name/' + ShortAddress ) )
		external.windows( 'conference_name/' + ShortAddress ).focus();
	else
		with ( external.createWindow( 'conference_name/' + ShortAddress, external.globals( 'cwd' ) + 'conference-name.html', [ Tracker ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'conference-container', 'wnd_conference_change_name' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 300, 80 );
			setPos( ( screen.availWidth - 300 ) / 2, ( screen.availHeight - 80 ) / 2 );
		}
}

/* View or change the role and affiliation of a user
 */
function dial_conference_accesslevel ( Occupant )
{
	var ShortAddress = Occupant.Tracker.Address.ShortAddress();
	if ( external.windows.Exists( 'conference_accesslevel/' + ShortAddress + '/' + Occupant.Name ) )
		external.windows( 'conference_accesslevel/' + ShortAddress + '/' + Occupant.Name ).focus();
	else
		with ( external.createWindow( 'conference_accesslevel/' + ShortAddress + '/' + Occupant.Name, external.globals( 'cwd' ) + 'conference-accesslevel.html', [ external.wnd.params[0], Occupant ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'conference-container', 'wnd_conference_accesslevel', [ Occupant.Name ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 385, 382 );
			setPos( ( screen.availWidth - 385 ) / 2, ( screen.availHeight - 382 ) / 2 );
		}
}

/* Redraw part or all of the menubar
 */
function MenuBarUpdate ( section )
{
	if ( ! section || section == 'file' )
	{
		external.wnd.menuBar.RemItem( 1 );

		var file = external.newPopupMenu;
		file.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_file_close' ), 10 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'conference-container', 'menu_file' ), 0, 1, file );
	}

	if ( ! section || section == 'actions' )
	{
		external.wnd.menuBar.RemItem( 2 );

		var Tracker = gContainer.Trackers.Exists( gContainer.ActiveTrackerAddress ) ? gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) : false;
		var Connected = Tracker && Tracker.Occupants.Exists( Tracker.Address.Resource );
		var CanLogin = ! Connected && ( Tracker && Tracker.Autoconnect );

		var User = Connected ? Tracker.Occupants.Item( Tracker.Address.Resource ) : false;
		var Owner = User && User.Affiliation == 'owner';
		var Admin = User && ( User.Affiliation == 'admin' || Owner );
		var Moderator = User && User.Role == 'moderator';
		var Participant = User && ( User.Role == 'participant' || Moderator );

		var actions = external.newPopupMenu;
		actions.AddItem( Connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_invite' ), 25 );
		actions.AddSeparator();
		actions.AddItem( Connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_change_name' ), 20 );
		actions.AddItem( CanLogin, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_password' ), 21 );
		actions.AddSeparator();
		actions.AddItem( Participant, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_subject' ), 22 );
		actions.AddItem( Owner, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_configuration' ), 23 );
		actions.AddItem( Owner, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_destroy' ), 24 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_axn' ), 1, 2, actions );
	}

	if ( ! section || section == 'bookmarks' )
	{
		external.wnd.menuBar.RemItem( 3 );

		var connected = external.globals( 'XMPPConnected' );
		var bookmarks = external.newPopupMenu;

		bookmarks.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_bmrk_add' ), 30 );
		bookmarks.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_bookmarks_manage' ), 31 );
		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.async = false;
		dom.resolveExternals = false;
		dom.load( external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '\\bookmarks.xml' );
		if ( dom.documentElement )
		{
			var BookmarkNodes = dom.documentElement.selectNodes( '/bookmarks/room[@address]' );
			if ( BookmarkNodes.length )
				bookmarks.AddSeparator();
			for ( var i = 0; i < Math.min( BookmarkNodes.length, 999 ); ++i )
				bookmarks.AddItem( connected, false, false, false, 0, BookmarkNodes.item(i).getAttribute( 'address' ), 3000 + i );
		}

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'conference-container', 'menu_bookmarks' ), 2, 3, bookmarks );
	}

	if ( ! section || section == 'tools' )
	{
		external.wnd.menuBar.RemItem( 4 );

		var cfg = external.globals( 'cfg' );
		var tabs = cfg( 'tabbedchat' ).toString() == 'true';
		var emo = cfg( 'emoticon' ).toString() == 'true';

		var tools = external.newPopupMenu;
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_emo_style' ), 40 );
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_font' ), 41 );
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_background' ),45 );
		tools.AddSeparator();
		tools.AddItem( true, tabs, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_tabbed' ), 42 );
		tools.AddItem( true, emo, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_emoticons' ), 43 );
		tools.AddSeparator();
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_settings' ), 44 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_tool' ), 3, 4, tools );
	}

	if ( ! section || section == 'help' )
	{
		external.wnd.menuBar.RemItem( 5 );

		var help = external.newPopupMenu;
		if ( external.globals( 'helpmanual' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_manual' ), 50 );
		if ( external.globals( 'helpforum' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_forum' ), 51 );
		if ( external.globals( 'helpcontact' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_contact' ), 52 );
		if ( external.globals( 'helpmanual' ).length || external.globals( 'helpforum' ).length || external.globals( 'helpcontact' ).length )
			help.AddSeparator();
		help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_about' ), 53 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_help' ), 6, 5, help );
	}

	external.wnd.menuBar.Update();
}

/* Launch the corresponding code or plugin event
 */
function MenuBarSelect ( id )
{
	var cfg = external.globals( 'cfg' );
	switch ( id )
	{
		case 10: //	close
			gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Close();
			break;

		case 20: // connect with different name
			dial_conference_connect_name( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 21: // connect with different password
			dial_conference_connect_password( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 22: // subject
			dial_conference_subject( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 23: // configure
			dial_conference_configuration( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 24: // destroy
			dial_conference_destroy( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 25: // invite
			external.wnd.params[0].dial_conference_invite( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ), '', '' );
			break;

		case 30: // bookmark this room
			var Password = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Password;
			external.wnd.params[0].dial_bookmarks_add( gContainer.ActiveTrackerAddress, Password );
			break;
		case 31: // manage bookmarks
			external.wnd.params[0].dial_bookmarks_manage();
			break;

		case 40: // emoticon style
			external.wnd.params[0].dial_emoticon_list();
			break;
		case 41: // font
			dial_font();
			break;
		case 42: // multiple tabs
			cfg( 'tabbedchat' ) = ! ( cfg( 'tabbedchat' ).toString() == 'true' );
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				with ( external.globals( 'ChatSessionPool' ).Containers.Item( ContainerNames[i] ) )
				{
					MenuBarUpdate( 'tools' );
					HTMLTabBar.style.display = HTMLTabBar.childNodes.length > 1 || cfg( 'tabbedchat' ) ? 'inline' : 'none';
				}
			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				with ( external.globals( 'ConferenceSessionPool' ).Containers.Item( ContainerNames[i] ) )
				{
					MenuBarUpdate( 'tools' );
					HTMLTabBar.style.display = HTMLTabBar.childNodes.length > 1 || cfg( 'tabbedchat' ) ? 'inline' : 'none';
				}
			break;
		case 43: // display emoticons
			cfg( 'emoticon' ) = ! ( cfg( 'emoticon' ).toString() == 'true' );
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ChatSessionPool' ).Containers.Item( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Containers.Item( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			break;
		case 44: // settings
			external.wnd.params[0].dial_preferences( '' );
			break;
		case 45: // background
			external.wnd.params[0].dial_background_list();
			break;

		case 50: // online manual
			external.wnd.params[0].dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpmanual' ) ) );
			break;
		case 51: // support forum
			external.wnd.params[0].dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpforum' ) ) );
			break;
		case 52: // contact us
			external.wnd.params[0].dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpcontact' ) ) );
			break;
		case 53: // about
			external.wnd.params[0].dial_about();
			break;

		default:
			/* Bookmarked conference room
			 */
			if ( id >= 3000 && id < 4000 )
			{
				var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
				dom.async = false;
				dom.resolveExternals = false;
				dom.load( external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '\\bookmarks.xml' );
				if ( dom.documentElement )
				{
					var BookmarkNodes = dom.documentElement.selectNodes( '/bookmarks/room[@address]' );
					if ( BookmarkNodes.length > id - 3000 )
					{
						var Address = BookmarkNodes.item( id - 3000 ).getAttribute( 'address' );
						var Password = BookmarkNodes.item( id - 3000 ).getAttribute( 'password' );
						external.wnd.params[0].dial_conference( Address, Password );
					}
				}
			}
			else
				external.wnd.params[0].warn( 'EVNT: selected menubar option ' + id );
	}
}

/* Display right click menu for the occupants list
 */
function OccupantsContextMenu ()
{
	event.cancelBubble = true;

	var cfg = external.globals( 'cfg' );
	var ViewSize = cfg( 'contactlistdisplay' ) == 'detailed';

	var Display = external.newPopupMenu;
	Display.AddItem( true, false, ViewSize, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_detailed' ), 30 );
	Display.AddItem( true, false, ! ViewSize, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_compact' ), 31 );

	var Tracker = gContainer.Trackers.Exists( gContainer.ActiveTrackerAddress ) ? gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) : false;
	var Connected = Tracker && Tracker.Occupants.Exists( Tracker.Address.Resource );

	var Menu = external.newPopupMenu;
	Menu.AddItem( Connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_invite' ), 1 );
	Menu.AddSeparator();
	Menu.AddItem( true, false, false, false, Display.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_view' ), 3 );
	Menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );

	switch ( Menu.Choice )
	{
		case 1: // invite
			external.wnd.params[0].dial_conference_invite( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ), '', '' );
		break;
		case 30: // detailed
			if ( cfg( 'contactlistdisplay' ) == 'compact' )
			{
				cfg( 'contactlistdisplay' ) = 'detailed';
				var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
				for ( var i = 0; i < TrackerNames.length; ++i )
					external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
				external.globals( 'ClientRoster' ).RefreshAll();
			}
			external.wnd.params[0].MenuBarUpdate( 'tools' );
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
			external.wnd.params[0].MenuBarUpdate( 'tools' );
		break;
	}
}

/* Draw the context menu of room occupants
 */
function OccupantContextMenu ( Occupant, MenuX, MenuY )
{
	event.cancelBubble = true;

	var Tracker = Occupant.Tracker;
	var Address = new XMPPAddress( Occupant.Address );
	var ShortAddress = Address.ShortAddress();
	var CanAdd = ShortAddress.length && ! external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) && ShortAddress != external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' );

	var LocalOccupant = Tracker.Occupants.Item( Tracker.Address.Resource );
	var LocalOwner = LocalOccupant && LocalOccupant.Affiliation == 'owner';
	var LocalAdmin = LocalOccupant && ( LocalOccupant.Affiliation == 'admin' || LocalOwner );
	var LocalModerator = LocalOccupant && LocalOccupant.Role == 'moderator';
	var LocalParticipant = LocalOccupant && ( LocalOccupant.Role == 'participant' || LocalModerator );

	var RemoteOccupant = Occupant;
	var RemoteOwner = RemoteOccupant.Affiliation == 'owner';
	var RemoteAdmin = RemoteOccupant.Affiliation == 'admin' || RemoteOwner;
	var RemoteModerator = RemoteOccupant.Role == 'moderator';
	var RemoteParticipant = RemoteOccupant.Role == 'participant' || RemoteModerator;

	var Menu = external.newPopupMenu;
	Menu.AddItem( true,
		false, false, true, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_occupant_private' ), 1 );
	Menu.AddItem( CanAdd,
		false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_occupant_add' ), 2 );
	Menu.AddSeparator();
	Menu.AddItem( LocalModerator && ! RemoteAdmin && ! RemoteModerator,
		false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_occupant_kick' ), 3 );
//	Menu.AddItem( LocalAdmin && ShortAddress.length,
//		false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_occupant_ban' ), 4 );
	Menu.AddItem( RemoteOccupant.Role.length && RemoteOccupant.Affiliation.length,
		false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_occupant_access' ), 5 );
	Menu.AddSeparator();
	Menu.AddItem( ShortAddress.length,
		false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_occupant_profile' ), 6 );
	Menu.Show( MenuX, MenuY, external.globals( 'Translator' ).Direction );

	switch ( Menu.Choice )
	{
		case 1: // private message
			if ( Occupant.Address.length )
				external.wnd.params[0].dial_chat( Address );
			else
			{
				document.getElementById( 'send-text' ).focus();
				document.getElementById( 'send-text' ).value = '/' + Occupant.Name + ': ';
			}
		break;
		case 2: // add
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence type="subscribe"/>' );
			dom.documentElement.setAttribute( 'to', ShortAddress );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
			external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_cl_adding', [ ShortAddress ] ), external.globals( 'softwarename' ), 0 | 64 );
		break;
		case 3: // kick
			if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'conference-container', 'confirm-kick', [ Occupant.Name ] ), external.globals( 'softwarename' ), 4 | 32 ) )
			{
				var Hook = new XMPPHookIQ();
				Hook.Window = external.wnd;
				Hook.Callback = 'ReceiveKicked';
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<iq type="set"><query xmlns="http://jabber.org/protocol/muc#admin"><item role="none"/></query></iq>' );
				dom.documentElement.setAttribute( 'id', Hook.Id );
				dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() );
				dom.documentElement.firstChild.firstChild.setAttribute( 'nick', Occupant.Name );
				external.wnd.params[0].warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		break;
		case 4: // ban
			if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'conference-container', 'confirm-ban', [ Occupant.Name ] ), external.globals( 'softwarename' ), 4 | 32 ) )
			{
				var Hook = new XMPPHookIQ();
				Hook.Window = external.wnd;
				Hook.Callback = 'ReceiveBanned';
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<iq type="set"><query xmlns="http://jabber.org/protocol/muc#admin"><item affiliation="outcast"/></query></iq>' );
				dom.documentElement.setAttribute( 'id', Hook.Id );
				dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() );
				dom.documentElement.firstChild.firstChild.setAttribute( 'jid', ShortAddress );
				external.wnd.params[0].warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
		break;
		case 5: // access level
			dial_conference_accesslevel( Occupant );
		break;
		case 6: // profile
			external.wnd.params[0].dial_userinfo( Address );
		break;
	}
}

/* Show error message if kick failed
 */
function ReceiveKicked ( iq )
{
	if ( iq.Type == 'error' && gContainer.Trackers.Exists( iq.FromAddress.ShortAddress() ) )
	{
		var ErrorDescription = '', Node = null;
		if ( Node = iq.XMLDOM.selectSingleNode( '/iq[@type = "error"]/error' ) )
		{
			if ( Node = Node.selectSingleNode( '*[@xmlns = "urn:ietf:params:xml:ns:xmpp-stanzas"]' ) )
				ErrorDescription = external.globals( 'Translator' ).Translate( 'conference-container', 'error-' + Node.tagName );
		}
		else if ( Node = iq.XMLDOM.selectSingleNode( '/iq[@type = "error"]/query/item/error' ) )
		{
			ErrorDescription = Node.text;
		}

		if ( ! ErrorDescription.length )
			ErrorDescription = external.globals( 'Translator' ).Translate( 'conference-container', 'unrecognized-error' );

		var MessageDom = new ActiveXObject( 'Msxml2.DOMDocument' );
		MessageDom.loadXML( '<message type="error"><error code="0"/></message>' );
		MessageDom.documentElement.setAttribute( 'to', external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
		MessageDom.documentElement.setAttribute( 'from', iq.FromAddress.ShortAddress() );
		MessageDom.documentElement.firstChild.text = external.globals( 'Translator' ).Translate( 'conference-container', 'kick-error', [ ErrorDescription ] );

		var Message = new XMPPMessage();
		Message.FromDOM( MessageDom );
		document.frames( 'iframe-' + iq.FromAddress.ShortAddress() ).onError( Message );
	}
}

/* Show error message if ban failed
 */
function ReceiveBanned ( iq )
{
	if ( iq.Type == 'error' && gContainer.Trackers.Exists( iq.FromAddress.ShortAddress() ) )
	{
		var ErrorDescription = '', Node = null;
		if ( Node = iq.XMLDOM.selectSingleNode( '/iq[@type = "error"]/error' ) )
		{
			if ( Node = Node.selectSingleNode( '*[@xmlns = "urn:ietf:params:xml:ns:xmpp-stanzas"]' ) )
				ErrorDescription = external.globals( 'Translator' ).Translate( 'conference-container', 'error-' + Node.tagName );
		}
		else if ( Node = iq.XMLDOM.selectSingleNode( '/iq[@type = "error"]/query/item/error' ) )
		{
			ErrorDescription = Node.text;
		}

		if ( ! ErrorDescription.length )
			ErrorDescription = external.globals( 'Translator' ).Translate( 'conference-container', 'unrecognized-error' );

		var MessageDom = new ActiveXObject( 'Msxml2.DOMDocument' );
		MessageDom.loadXML( '<message type="error"><error code="0"/></message>' );
		MessageDom.documentElement.setAttribute( 'to', external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
		MessageDom.documentElement.setAttribute( 'from', iq.FromAddress.ShortAddress() );
		MessageDom.documentElement.firstChild.text = external.globals( 'Translator' ).Translate( 'conference-container', 'ban-error', [ ErrorDescription ] );

		var Message = new XMPPMessage();
		Message.FromDOM( MessageDom );
		document.frames( 'iframe-' + iq.FromAddress.ShortAddress() ).onError( Message );
	}
}

/* Fake server message
 */
function ServerMessage ( Tracker, MessageText )
{
	var MessageDom = new ActiveXObject( 'Msxml2.DOMDocument' );
	MessageDom.loadXML( '<message><body/></message>' );
	MessageDom.documentElement.setAttribute( 'xml:lang', external.globals( 'language' ) );
	MessageDom.documentElement.setAttribute( 'to', external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
	MessageDom.documentElement.setAttribute( 'from', Tracker.Address.ShortAddress() );
	MessageDom.documentElement.setAttribute( 'type', 'groupchat' );
	MessageDom.documentElement.firstChild.text = MessageText;

	var Message = new XMPPMessage();
	Message.FromDOM( MessageDom );
	document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onMessage( Message );
}

/* Show context menu at the user bar
 */
function MouseMenu ()
{
	var Tracker = gContainer.Trackers.Exists( gContainer.ActiveTrackerAddress ) ? gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) : false;
	var Connected = Tracker && Tracker.Occupants.Exists( Tracker.Address.Resource );
	var User = Connected ? Tracker.Occupants.Item( Tracker.Address.Resource ) : false;
	var Owner = User && User.Affiliation == 'owner';
	var Admin = User && ( User.Affiliation == 'admin' || Owner );
	var Moderator = User && User.Role == 'moderator';
	var Participant = User && ( User.Role == 'participant' || Moderator );

	var menu = external.newPopupMenu;
	menu.AddItem( Participant, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_subject' ), 1 );
	menu.AddItem( Owner, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_configuration' ), 2 );
	menu.AddItem( Owner, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_axn_destroy' ), 3 );
	menu.AddSeparator();
	menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'conference-container', 'menu_bmrk_add' ), 4 );
	menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_bookmarks_manage' ), 5 );

	var posx = external.globals( 'Translator' ).Direction ? (window.screenLeft + external.wnd.width - 50) : (window.screenLeft + 42);
	var posy = window.screenTop + 48;
	menu.Show( posx, posy, external.globals( 'Translator' ).Direction );

	switch ( menu.Choice )
	{
		case 1: // subject
			dial_conference_subject( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 2: // configure
			dial_conference_configuration( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 3: // destroy
			dial_conference_destroy( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ) );
			break;
		case 4: // bookmark this room
			var Password = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Password;
			external.wnd.params[0].dial_bookmarks_add( gContainer.ActiveTrackerAddress, Password );
			break;
		case 5: // manage bookmarks
			external.wnd.params[0].dial_bookmarks_manage();
			break;
	}
}

/* Give focus to text input area when the window is brought to the foreground
 */
function OnWindowActivate ()
{
	setTimeout(
		function () 
		{
			if ( external.wnd.isActive() )
			{
				try 
				{
					if (!document.getElementById("send-text").disabled) 
					{
						document.getElementById("send-text").blur();
						document.getElementById("send-text").focus();
					}
				} catch(e) {}
			}
		}
	,0);
}

/* Send :yes:
*/
function SendLike() {
  SendEmoticons(':yes:');
}

/* Send emoticon text
*/
function SendEmoticons(emoticons)
{
	var Tracker = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress );
	var Input = emoticons;

	/* Change nickname
	 */
	if ( Input.match( /\/name \S/ ) )
	{
		external.globals( 'cfg' ).Item( 'conferencenick' ) = Input.substr( 6 );
		var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
		for ( var i = 0; i < TrackerNames.length; ++i )
		{
			external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).Address.Resource = external.globals( 'cfg' ).Item( 'conferencenick' );
			external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).SendPresence( external.globals( 'cfg' ).Item( 'lastmode' ), external.globals( 'cfg' ).Item( 'lastmsg' ) );
		}
	}

	/* Group or personal message
	 */
	else
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<message><body/><html><body/></html><x xmlns="jisp:x:jep-0038"><name/></x></message>' );
		dom.documentElement.setAttribute( 'xml:lang', external.globals( 'language' ) );
		dom.documentElement.setAttribute( 'from', external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
		dom.selectSingleNode( '/message/x[@xmlns="jisp:x:jep-0038"]/name' ).text = external.globals( 'cfg' ).Item( 'emoticonset' );

		if ( Input.match( /^\/([^\:]+)\: \S/ ) && Tracker.Occupants.Exists( RegExp.$1 ) )
		{
			Input = Input.substr( RegExp.$1.length + 3 );
			dom.documentElement.setAttribute( 'type', 'chat' );
			dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() + '/' + RegExp.$1 );
		}
		else
		{
			dom.documentElement.setAttribute( 'type', 'groupchat' );
			dom.documentElement.setAttribute( 'to', Tracker.Address.ShortAddress() );
		}

		dom.documentElement.firstChild.text = Input;

		var HTMLBody = dom.documentElement.selectSingleNode( '/message/html/body' );
		var TextLines = Input.split( '\n' );
		HTMLBody.setAttribute( 'style', document.getElementById( 'send-text' ).style.cssText.toLowerCase() );
		HTMLBody.appendChild( dom.createTextNode( TextLines[0] ) );
		for ( var i = 1; i < TextLines.length; ++i )
		{
			HTMLBody.appendChild( dom.createElement( 'br' ) );
			HTMLBody.appendChild( dom.createTextNode( TextLines[i] ) );
		}
		dom.documentElement.selectSingleNode( 'html' ).setAttribute( 'xmlns', 'http://jabber.org/protocol/xhtml-im' );
		dom.documentElement.selectSingleNode( 'html' ).firstChild.setAttribute( 'xmlns', 'http://www.w3.org/1999/xhtml' );

		if ( dom.documentElement.getAttribute( 'type' ) == 'chat' )
		{
			var Message = new XMPPMessage();
			Message.FromDOM( dom );
			document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onMessage( Message );
		}

		external.wnd.params[0].warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}
}

/* Open new conferente
 */
function TabAddActivate ()
{
	external.wnd.params[0].dial_conference_create();
}

/* Browse conference
 */
function UndoClosingLastTab ()
{
	external.wnd.params[0].dial_conference_browse();
}