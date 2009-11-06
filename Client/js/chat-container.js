var gContainer = null;

function Begin ()
{
	external.globals( 'Translator' ).TranslateWindow( 'chat-container', document );
	external.wnd.setCloseHandler( 'Close' );

	/* Event handlers for text selecting, input area resizing, and text input focus
	 */
    document.attachEvent( 'onselectionchange', DisableButton );
	document.attachEvent( 'onselectstart', SelectionFilter );
	document.attachEvent( 'ondragstart', SelectionFilter );
	window.attachEvent( 'onresize', ResizeCheck );
	external.wnd.setActivationHandler( 'OnWindowActivate' );

	/* Initialize the container
	 */
	var SessionPool = external.globals( 'ChatSessionPool' );
	gContainer = new SessionContainer( SessionPool );
	gContainer.Name = external.wnd.params[1];
	gContainer.InputUpdate();
	SessionPool.AddContainer( gContainer );

	/* Set up the menu bar
	 */
	external.wnd.setMenuHandler( 'MenuBarSelect' );
	MenuBarUpdate();

	/* Restore the size and position of the window
	 */
	var cfg = external.globals( 'cfg' );
	var TabbedWindows = cfg( 'tabbedchat' ).toString() == 'true';
	cfg( 'user_dialog_width' ) = parseInt( cfg( 'user_dialog_width' ), 10 );
	cfg( 'user_dialog_height' ) = parseInt( cfg( 'user_dialog_height' ), 10 );
	cfg( 'user_dialog_left' ) = parseInt( cfg( 'user_dialog_left' ), 10 ) + ( TabbedWindows ? 0 : 20 );
	cfg( 'user_dialog_top' ) = parseInt( cfg( 'user_dialog_top' ), 10 ) + ( TabbedWindows ? 0 : 20 );

	external.wnd.setPos( cfg( 'user_dialog_left' ), cfg( 'user_dialog_top' ) );
	if ( cfg( 'user_dialog_maximized' ).toString() == 'true' )
		external.wnd.Maximize();
	else
	{
		cfg( 'user_dialog_width' ) = Math.max( 250, cfg( 'user_dialog_width' ) );
		cfg( 'user_dialog_height' ) = Math.max( 210, cfg( 'user_dialog_height' ) );
		external.wnd.setSize( cfg( 'user_dialog_width' ), cfg( 'user_dialog_height' ) );
	}

	/* Restore the size of the input area
	 */
	document.getElementById( 'send-text' ).parentNode.parentNode.height = isNaN( parseInt( external.globals( 'cfg' )( 'textinputheight' ), 10 ) ) ? 60 : Math.max( 60, parseInt( external.globals( 'cfg' )( 'textinputheight' ), 10 ) );

	/* Load pending trackers
	 */
	var TrackerNames = ( new VBArray( SessionPool.TrackersLoading.Keys() ) ).toArray().sort();
	if ( external.globals( 'cfg' )( 'tabbedchat' ).toString() == 'true' )
		for ( var i = SessionPool.TrackersLoading.Count - SessionPool.ContainersLoading.Count - 1; i >= 0; i-- )
		{
			var Address = SessionPool.TrackersLoading( TrackerNames[i] );
			gContainer.CreateTracker( Address );
		}
	else
	{
		var Address = SessionPool.TrackersLoading( TrackerNames[ SessionPool.ContainersLoading.Count ] );
		gContainer.CreateTracker( Address );
	}
}

function End ()
{
	/* Stop receiving new trackers and events
	 */
	gContainer.Clear();

	/* Remember window size and position
	 */
	var cfg = external.globals( 'cfg' );
	cfg( 'user_dialog_maximized' ) = external.wnd.isMaximized ? 'true' : 'false';
	cfg( 'user_dialog_left' ) = Math.max( 0, external.wnd.left );
	cfg( 'user_dialog_top' ) = Math.max( 0, external.wnd.top );
	if ( external.wnd.width > 100 )
		cfg( 'user_dialog_width' ) = external.wnd.width;
	if ( external.wnd.height > 100 )
		cfg( 'user_dialog_height' ) = external.wnd.height;
	external.wnd.params[0].SettingsSave();
}

/* Check for recently received messages or multiple tabs
 */
function Close ()
{
	if ( gContainer.Trackers.Count == 1 )
		gContainer.Trackers( gContainer.ActiveTrackerAddress ).Close();
	else if ( ! gContainer.Trackers.Count || external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'chat-container', 'close_many_tabs' ), external.globals( 'softwarename' ), 4 | 48 ) == 6 )
		external.wnd.close();
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
	this.PurgeHistory = PurgeHistory;
	this.TrackerCreated = TrackerCreated;

	/* Create a tracker object, draw a tab
	 * TrackerCreated will be fired when the tracker is ready to receive events
	 */
	function CreateTracker ( Address )
	{
		var ShortAddress = Address.ShortAddress();
		var Tracker = new SessionTracker( Address );
		Tracker.Container = this;
		Tracker.HTMLArea = document.createElement( 'IFRAME' );

		var BackgroundPath = external.globals( 'usersdir' ) + 'Backgrounds\\' + Tracker.Background + '\\index.html';
		if ( ! external.FileExists( BackgroundPath ) )
		{
			external.globals( 'cfg' )( 'background' ) = '';
			Tracker.Background = '';
			BackgroundPath = 'chat-messages.html';
		}

		with ( Tracker.HTMLArea )
		{
			id = 'iframe-' + ShortAddress;
			style.display = 'none';
			height = '100%';
			width = '100%';
			allowTransparency = true;
			src = BackgroundPath;
		}
		Tracker.HTMLArea.SessionTracker = Tracker;
		this.HTMLTabArea.insertAdjacentElement( 'beforeEnd', Tracker.HTMLArea );
		document.frames( 'iframe-' + ShortAddress ).SessionTracker = Tracker;

		Tracker.DrawButton();
		Tracker.DrawContainerInfo();
		Tracker.DrawTyping();
	}

	/* Register the tracker with the session pool to receive all queued events
	 */
	function TrackerCreated ( Tracker )
	{
		if ( ! ( external.wnd.isActive() && this.Trackers.Count ) )
		{
			if ( this.SessionPool.Events.Exists( Tracker.Address.ShortAddress() ) && ! external.globals( 'ClientRoster' ).IsFlashing )
				Tracker.Activate( false );
			else
				Tracker.Activate( true );
		}
		this.Trackers.Add( Tracker.Address.ShortAddress(), Tracker );
		this.SessionPool.AddTracker( Tracker );
		document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).document.body.title = external.globals( 'Translator' ).Translate( 'chat-container', 'tt-messages' );
	}

	/* Get out of the session pool and remove all trackers from this container
	 */
	function Clear ()
	{
		this.SessionPool.DeleteContainer( this );

		var TrackerNames = ( new VBArray( this.Trackers.Keys() ) ).toArray();
		for ( var i = 0; i < TrackerNames.length; ++i )
			this.Trackers( TrackerNames[i] ).Clear();
	}

	/* Reload the user interface translation
	 */
	function LanguageUpdate ()
	{
		external.globals( 'Translator' ).TranslateWindow( 'chat-container', document );
		MenuBarUpdate();
		if ( this.Trackers.Exists( this.ActiveTrackerAddress ) )
		{
			this.Trackers( this.ActiveTrackerAddress ).DrawContainerInfo();
			this.Trackers( this.ActiveTrackerAddress ).DrawTyping();
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
			fontStyle = cfg( 'user_dialog_italic' );
			fontWeight = cfg( 'user_dialog_bold' );
			textDecorationLineThrough = cfg( 'user_dialog_strike' ).toString() == 'true';
			textDecorationUnderline = cfg( 'user_dialog_underline' ).toString() == 'true';
		}

		if ( external.globals( 'XMPPConnected' ) )
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

	/* Delete the message cache file
	 */
	function PurgeHistory ( Address )
	{
		var FilePath = external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) + '\\Message Cache\\' + ( new MD5() ).digest( Address.ShortAddress() ) + '.buffer';
		if ( external.FileExists( FilePath ) )
			external.File( FilePath ).Delete();
		external.File( FilePath ).Close();
	}
}

/* Tracker parses the events and manages its tabbed conversation
 */
function SessionTracker ( Address )
{
	this.Address = Address;
	this.Avatar = '';
	this.AvatarPath = '';
	this.Background = external.globals( 'cfg' )( 'background' );
	this.BackgroundLoading = false;
	this.ChatState = '';
	this.Container = null;
	this.HookIQ = null;
	this.IsActive = false;
	this.IsComposing = false;
	this.IsFlashing = false;
	this.HTMLArea = false;
	this.HTMLButton = false;
	this.LastMessage = 0;
	this.LastOnline = '';
	this.MessageId = '';
	this.Name = Address.Name;
	this.Reloading = false;
	this.Show = '';
	this.Status = '';
	this.Text = '';
	this.TypingTimeout = null;
	this.WantsComposing = false;

	this.Activate = Activate;
	this.BackgroundUpdate = BackgroundUpdate;
	this.Clear = Clear;
	this.Close = Close;
	this.Deactivate = Deactivate;
	this.DispatchEvent = DispatchEvent;
	this.DrawButton = DrawButton;
	this.DrawContainerInfo = DrawContainerInfo;
	this.DrawTyping = DrawTyping;
	this.Flash = Flash;
	this.SendMessage = SendMessage;

	/* Handle events from the SessionPool
	 */
	function DispatchEvent ( Event )
	{
		/* Error message
		 */
		if ( Event.Type == 'message' && Event.Payload.Type == 'error' && Event.Payload.ErrorMessage.length )
		{
			/* Forward the event to the message viewer
			 */
			try
			{
				document.frames( 'iframe-' + this.Address.ShortAddress() ).onError( Event.Payload );
			}
			catch ( e )
			{
				external.wnd.params[0].warn( 'ERRO: onError failed for tracker ' + this.Address.ShortAddress() + ' - ' + e.description );
			}

			/* Notify the user of this message
			 */
			this.Flash( 4 );
		}
		/* Message cache
		 */
		else if ( Event.Type == 'history' && Event.Payload.length )
		{
			/* Forward the event to the message viewer
			 */
			external.globals( 'soundallowed' ) = false;
			try
			{
				document.frames( 'iframe-' + this.Address.ShortAddress() ).onHistory( Event.Payload );
			}
			catch ( e )
			{
				external.wnd.params[0].warn( 'ERRO: onHistory failed for tracker ' + this.Address.ShortAddress() + ' - ' + e.description );
				for ( var i = 0; i < Event.Payload.length; ++i )
				{
					try
					{
						document.frames( 'iframe-' + this.Address.ShortAddress() ).onMessage( Event.Payload[i] );
					}
					catch ( e )
					{
						external.wnd.params[0].warn( 'ERRO: onMessage failed for tracker ' + this.Address.ShortAddress() + ' - ' + e.description );
					}
				}
			}
			external.globals( 'soundallowed' ) = true;
		}
		/* Normal message
		 */
		else if ( Event.Type == 'message' && ( Event.Payload.Type == 'chat' || Event.Payload.Type == 'normal' || Event.Payload.Type == '' ) )
		{
			/* Change the addressee
			 */
			if ( this.Address.Resource != Event.Payload.FromAddress.Resource )
			{
				this.Address = Event.Payload.FromAddress;
				this.DrawContainerInfo();
			}

			/* Chat state
			 */
			if ( Event.Payload.ChatState.length )
				this.ChatState = Event.Payload.ChatState;

			/* Started typing
			 */
			if ( Event.Payload.IsComposing )
				this.IsComposing = true;

			/* Stopped typing
			 */
			else if ( Event.Payload.DropComposing )
				this.IsComposing = false;

			/* Display message
			 */
			else if ( Event.Payload.Body.length )
			{
				this.LastMessage = ( new Date() ).getTime();
				this.MessageId = Event.Payload.Id;
				this.IsComposing = false;
				this.MessageThread = Event.Payload.Thread;
				this.WantsComposing = Event.Payload.WantsComposing;

				/* Forward the event to the message viewer
				 */
				try
				{
					document.frames( 'iframe-' + this.Address.ShortAddress() ).onMessage( Event.Payload );
				}
				catch ( e )
				{
					external.wnd.params[0].warn( 'ERRO: onMessage failed for tracker ' + this.Address.ShortAddress() + ' - ' + e.description );
				}

				/* Notify the user of this message
				 */
				this.Flash( 4 );

				/* Confirm that the message was displayed
				 */
				if ( Event.Payload.WantsDisplayed )
				{
					var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
					dom.loadXML( '<message><x xmlns="jabber:x:event"><displayed/><id/></x></message>' );
					if ( Event.Payload.Id.length )
						dom.selectSingleNode( '/message/x/id' ).text = Event.Payload.Id;
					dom.documentElement.setAttribute( 'to', Event.Payload.From );
					external.wnd.params[0].warn( 'SENT: ' + dom.xml );
					external.XMPP.SendXML( dom );
				}
			}

			/* Reset the contact's typing indicator
			 */
			this.DrawTyping();
		}
	}

	/* Use audio-visual signals to attract attention
	 */
	function Flash ( Times )
	{
		/* Play sound
		 */
		if ( ( ! external.wnd.isActive() || ! this.IsActive ) && external.globals( 'cfg' )( 'soundmessage' ).toString() == 'true' && ( external.globals( 'cfg' )( 'lastmode' ) < 2 || external.globals( 'cfg' )( 'lastmode' ) == 5 ) )
			external.wnd.params[0].sound_play( external.globals( 'cfg' )( 'soundmessagefile' ), false );
		/* Flash window
		 */
		if ( ! external.wnd.isActive() )
		{
			if ( ! this.IsActive )
				this.Activate( true );
			external.wnd.flash( 6 );
		}
		/* Blink tab icon
		 */
		if ( ! this.IsActive && ! this.IsFlashing )
			FlashTab( this.Address.ShortAddress(), Times * 2 );
	}

	/* Show or hide the typing indicator
	 */
	function DrawTyping ()
	{
		if ( this.IsActive )
			document.getElementById( 'user-typing-area' ).style.display = ( this.IsComposing || this.ChatState == 'composing' ) ? 'block' : 'none';
		try
		{
			if ( document.frames( 'iframe-' + this.Address.ShortAddress() ).onComposingRemote( this.IsComposing || this.ChatState == 'composing' ) )
				document.getElementById( 'user-typing-area' ).style.display = 'none';
		}
		catch ( e )
		{
		}
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
				this.Container.SessionPool.TrackersLoading.Add( ShortAddress, this.Address );
				this.Container.SessionPool.DeleteTracker( this );
				this.Container.Trackers.Remove( ShortAddress );

				this.Deactivate();
				this.Reloading = true;
				this.HTMLArea.src = 'about:blank';
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
			this.Container.SessionPool.TrackersLoading.Add( ShortAddress, this.Address );
			this.Container.SessionPool.DeleteTracker( this );
			this.Container.Trackers.Remove( ShortAddress );

			this.Deactivate();
			this.Reloading = true;
			this.HTMLArea.src = 'about:blank';
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
		var ShortAddress = this.Address.ShortAddress();
		if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
		{
			var RosterItem = external.globals( 'ClientRoster' ).Items( ShortAddress );
			this.Name = RosterItem.Name;
			/* Get the resource that sent the last message
			 */
			if ( RosterItem.Resources.Exists( this.Address.Resource ) )
			{
				if ( this.HookIQ )
				{
					this.HookIQ.Destroy();
					this.HookIQ = null;
				}
				this.LastOnline = '';
				var RosterResource = RosterItem.Resources( this.Address.Resource );
				this.Show = RosterResource.Show;
				this.Avatar = RosterResource.Avatar.length ? RosterResource.Avatar : 'unknown-soldier.gif';
				this.Status = RosterResource.Status;
			}
			/* Get the resource with the highest priority
			 */
			else if ( RosterItem.Resources.Count )
			{
				if ( this.HookIQ )
				{
					this.HookIQ.Destroy();
					this.HookIQ = null;
				}
				this.LastOnline = '';
				var Priority = Number.NEGATIVE_INFINITY;
				var ResourceNames = ( new VBArray( RosterItem.Resources.Keys() ) ).toArray();
				for ( var i = 0; i < ResourceNames.length; ++i )
				{
					var RosterResource = RosterItem.Resources( ResourceNames[i] );
					if ( RosterResource.Priority > Priority )
					{
						this.Address.Resource = RosterResource.ResourceName;
						this.Show = RosterResource.Show;
						this.Avatar = RosterResource.Avatar.length ? RosterResource.Avatar : 'unknown-soldier.gif';
						this.Status = RosterResource.Status;
					}
				}
			}
			else
			{
				/* Offline|Awaiting|Unknown contact item
				 */
				this.Address.Resource = '';
				this.Show = 'offline';
				this.Avatar = 'unknown-soldier.gif';
				this.Status = RosterItem.Status;
				if ( RosterItem.Subscription == 'to' || RosterItem.Subscription == 'both' )
					this.Show = 'offline';
				else if ( ( RosterItem.Subscription == 'none' || RosterItem.Subscription == 'from' ) && RosterItem.Ask == 'subscribe' )
					this.Show = 'awaiting';
				else
					this.Show = 'unknown';

				/* Reset typing indicators
				 */
				this.ChatState = '';
				this.IsComposing = false;
				this.WantsComposing = false;
				this.DrawTyping();
			}
		}
		else
		{
			/* Not in our contact list
			 */
			this.LastOnline = '';
			this.Name = this.Address.User.length ? this.Address.User : this.Address.Host;
			this.Show = 'unknown';
			this.Status = '';

			var Mapped = external.globals( 'ClientRoster' ).GetAvatar( this.Address.Resource, ShortAddress );
			this.Avatar = Mapped.length ? Mapped : 'unknown-soldier.gif';

			/* Name of a service or transport
			 */
			var ServiceAddresses = ( new VBArray( external.globals( 'ClientServices' ).Services.Keys() ) ).toArray();
			for ( var i = 0; i < ServiceAddresses.length; ++i )
				if ( ShortAddress == ServiceAddresses[i] )
					this.Name = external.globals( 'ClientServices' ).Services( ServiceAddresses[i] ).Name;
		}

		/* Retrieve last online time
		 */
		if ( this.Show == 'offline' && external.globals( 'XMPPConnected' ) && ! this.HookIQ && ! this.LastOnline )
		{
			this.HookIQ = new XMPPHookIQ();
			this.HookIQ.Window = external.wnd;
			this.HookIQ.Callback = 'LastOnline';
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:last"/></iq>' );
			dom.documentElement.setAttribute( 'id', this.HookIQ.Id );
			dom.documentElement.setAttribute( 'to', this.Address.ShortAddress() );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		if ( this.Avatar.length == 40 )
			this.AvatarPath = external.globals( 'usersdir' ) + 'Avatars\\';
		else
			this.AvatarPath = external.globals( 'cwd' ) + '..\\images\\clients\\';

		if ( ! external.FileExists( this.AvatarPath + this.Avatar ) )
		{
			this.AvatarPath = external.globals( 'cwd' ) + '..\\images\\clients\\';
			this.Avatar = 'unknown-soldier.gif';
		}

		this.HTMLButton.children.item(1).innerText = this.Name;

		if ( external.FileExists( this.AvatarPath + 'small\\' + this.Avatar ) )
			this.HTMLButton.children.item(0).src = this.AvatarPath + 'small\\' + this.Avatar;
		else
			this.HTMLButton.children.item(0).src = this.AvatarPath + this.Avatar;

		if ( this.IsActive )
		{
			document.getElementById( 'user-typing' ).innerText = external.globals( 'Translator' ).Translate( 'chat-container', 'msg_typing', [ this.Name ] );
			MenuBarUpdate( 'actions' );
			if ( this.Avatar.length == 40 )
			{
				document.getElementById( 'mode-avatar' ).src = this.AvatarPath + this.Avatar;
				document.getElementById( 'mode-avatar' ).style.backgroundImage = 'none';
				document.getElementById( 'mode-avatar' ).title = external.globals( 'Translator' ).Translate( 'chat-container', 'tt-avatar' );
			}
			else
			{
				document.getElementById( 'mode-avatar' ).src = external.globals( 'cwd' ) + '..\\images\\misc\\alpha-1x1.gif';
				document.getElementById( 'mode-avatar' ).style.backgroundImage = 'url( ' + this.AvatarPath + this.Avatar + ' )';
				document.getElementById( 'mode-avatar' ).title = '';
			}
			var ModeMessage = '';
			switch ( this.Show )
			{
				case 'awaiting':
				case 'unknown': ModeMessage = external.globals( 'Translator' ).Translate( 'chat-container', 'unknown', [ this.Name ] ); break;
				case 'offline': ModeMessage = external.globals( 'Translator' ).Translate( 'main', 'cl_status_offline' ); break;
				case 'chat': ModeMessage = external.globals( 'Translator' ).Translate( 'main', 'cl_ffc' ); break;
				case 'dnd': ModeMessage = external.globals( 'Translator' ).Translate( 'main', 'cl_busy' ); break;
				case 'xa': ModeMessage = external.globals( 'Translator' ).Translate( 'main', 'cl_xaway' ); break;
				case 'away': ModeMessage = external.globals( 'Translator' ).Translate( 'main', 'cl_away' ); break;
				default: ModeMessage = external.globals( 'Translator' ).Translate( 'main', 'cl_available' ); break;
			}
			if ( this.Status.length )
				ModeMessage += ' - ' + this.Status.replace( /[\n\r]+/mg, ' - ' );
			if ( this.LastOnline.length )
				ModeMessage += ' - ' + external.globals( 'Translator' ).Translate( 'chat-container', 'lastonline', [ this.LastOnline ] );
			DrawModeMessage( document.getElementById( 'mode-message' ), ModeMessage, 'mode-message-link' );
			document.getElementById( 'mode-name' ).innerText = this.Name;
			document.getElementById( 'mode-address' ).innerText = '<' + this.Address.CleanAddress() + '>';
			document.getElementById( 'mode-address' ).style.display = this.Name == this.Address.CleanAddress() ? 'none' : 'inline';
			external.wnd.SetTitle( external.globals( 'Translator' ).Translate( 'chat-container', 'windowtitle', [ this.Name ] ) );
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
			this.HTMLButton.insertAdjacentElement( 'beforeEnd', Picture );

			var Name = document.createElement( 'NOBR' );
			Name.className = this.IsActive ? 'tab-bar-text-active' : 'tab-bar-text';
			Name.innerText = this.Address.User;
			this.HTMLButton.insertAdjacentElement( 'beforeEnd', Name );

			var Close = document.createElement( 'IMG' );
			Close.title = external.globals( 'Translator' ).Translate( 'chat-container', 'tt-close' );
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

			this.HTMLButton.attachEvent(
				'onmousedown',
				function ()
				{
					if ( event.button != 1 )
						return;
					else if ( event.srcElement.tagName == 'SPAN' )
						event.srcElement.SessionTracker.Activate( true );
					else
						event.srcElement.parentNode.SessionTracker.Activate( true );
				}
			);
			this.HTMLButton.attachEvent(
				'onmouseup',
				function ()
				{
					if ( event.button == 1 )
					{
						if ( ! document.getElementById( 'send-text' ).disabled )
							document.getElementById( 'send-text' ).focus();
					}
					else if ( event.button == 4 )
					{
						if ( event.srcElement.tagName == 'SPAN' )
							event.srcElement.SessionTracker.Close();
						else
							event.srcElement.parentNode.SessionTracker.Close();
					}
				}
			);
			this.Container.HTMLTabBar.insertAdjacentElement( 'beforeEnd', this.HTMLButton );
			if ( external.globals( 'cfg' )( 'tabbedchat' ).toString() != 'true' )
				this.Container.HTMLTabBar.style.display = this.Container.HTMLTabBar.childNodes.length ? 'inline' : 'none';
		}
	}

	/* Close the tracker and clear up after it
	 */
	function Close ()
	{
		if ( ! this.LastMessage || ( new Date() ).getTime() - this.LastMessage > 1000 || external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'chat-container', 'close_recent_message' ), external.globals( 'softwarename' ), 4 | 48 ) == 6 )
		{
			this.Clear();
			var TrackerNames = ( new VBArray( this.Container.Trackers.Keys() ) ).toArray();
			if ( TrackerNames.length )
				this.Container.Trackers( TrackerNames[ TrackerNames.length - 1 ] ).Activate( true );
			else
				setTimeout( 'external.wnd.close()', 0 );
		}
	}

	/* Get out of the session pool, remove the tab, cancel typing events, and stop waiting for last online time
	 */
	function Clear ()
	{
		if ( this.IsActive )
			this.Deactivate();

		if ( this.ChatState )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<message type="chat"><gone xmlns="http://jabber.org/protocol/chatstates"/></message>' );
			dom.documentElement.setAttribute( 'to', this.Address.LongAddress() );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		if ( this.HookIQ )
			this.HookIQ.Destroy();
		this.Container.SessionPool.DeleteTracker( this );
		this.Container.Trackers.Remove( this.Address.ShortAddress() );
		if ( this.Container.ActiveTrackerAddress == this.Address.ShortAddress() )
			this.Container.ActiveTrackerAddress = '';
		this.HTMLArea.src = 'about:blank';
		this.HTMLArea.removeNode( true );
		this.HTMLButton.removeNode( true );
		if ( external.globals( 'cfg' )( 'tabbedchat' ).toString() != 'true' )
			this.Container.HTMLTabBar.style.display = this.Container.HTMLTabBar.childNodes.length ? 'inline' : 'none';
	}

	/* Turn off the tab button, hide the conversation, and store the message being written
	 */
	function Deactivate ()
	{
		document.getElementById( 'send-text' ).detachEvent( 'onpropertychange', Typing );

		if ( this.TypingTimeout )
		{
			clearTimeout( this.TypingTimeout );
			this.TypingTimeout = null;

			if ( this.ChatState )
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<message type="chat"><active xmlns="http://jabber.org/protocol/chatstates"/></message>' );
				dom.documentElement.setAttribute( 'to', this.Address.LongAddress() );
				external.wnd.params[0].warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}

			if ( this.WantsComposing )
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<message><x xmlns="jabber:x:event"><id/></x></message>' );
				dom.selectSingleNode( '/message/x/id' ).text = this.MessageId;
				dom.documentElement.setAttribute( 'to', this.Address.LongAddress() );
				external.wnd.params[0].warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}

			try
			{
				document.frames( 'iframe-' + this.Address.ShortAddress() ).onComposingLocal( false );
			}
			catch ( e )
			{
			}
		}

		this.IsActive = false;
		this.DrawButton();
		this.HTMLArea.style.display = 'none';
		this.Text = document.getElementById( 'send-text' ).value;
		document.getElementById( 'send-text' ).value = '';

		try
		{
			document.frames( 'iframe-' + this.Address.ShortAddress() ).onDeactivated();
		}
		catch ( e )
		{
		}
	}

	/* Deactivate te previously active tab, turn on the button active, show the conversation, and restore the message being written
	 */
	function Activate ( StealFocus )
	{
		if ( StealFocus )
		{
			external.wnd.hide( false );
			external.wnd.focus();
			if ( external.wnd.isHidden || external.wnd.isMinimized )
				if ( external.wnd.width >= screen.availWidth || external.wnd.height >= screen.availHeight )
					external.wnd.Maximize();
				else
					external.wnd.restore();
		}
		else
		{
			if ( ! ( external.wnd.isActive() || external.windows( 'MainWindow' ).isActive() ) && ! this.Container.ActiveTrackerAddress )
				external.wnd.Minimize();
			external.wnd.hide( false );
			external.wnd.flash( 6 );
		}

		if ( ! this.IsActive )
		{
			/* Hide the Add Tab display
			 */
			TabAddDeactivate();

			 /* Deactivate old tab
			  */
			if ( this.Container.Trackers.Exists( this.Container.ActiveTrackerAddress ) )
				this.Container.Trackers( this.Container.ActiveTrackerAddress ).Deactivate();

			/* Activate new tab
			 */
			this.Container.ActiveTrackerAddress = this.Address.ShortAddress();
			this.IsActive = true;
			this.DrawButton();
			this.DrawContainerInfo();
			this.DrawTyping();
			this.HTMLArea.style.display = 'block';
			this.HTMLButton.children.item(1).style.fontStyle = '';
			ResizeStopTracking();

			document.getElementById( 'user-typing' ).innerText = external.globals( 'Translator' ).Translate( 'chat-container', 'msg_typing', [ this.Name ] );

			/* The TEXTAREA.value must change otherwise the next OnPropertyChange event is not fired
			 */
			document.getElementById( 'send-text' ).value = Math.random();
			document.getElementById( 'send-text' ).value = this.Text;
			this.Text = '';

			if ( ! document.getElementById( 'send-text' ).disabled )
				document.getElementById( 'send-text' ).focus();

			document.getElementById( 'send-text' ).attachEvent( 'onpropertychange', Typing );

			try
			{
				document.frames( 'iframe-' + this.Address.ShortAddress() ).onActivated();
			}
			catch ( e )
			{
			}
		}
	}
}

/* Transmit the message
 */
function SendMessage ()
{
	var Tracker = gContainer.Trackers( gContainer.ActiveTrackerAddress );

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	if ( Tracker.WantsComposing )
		dom.loadXML( '<message type="chat"><body/><html><body/></html><x xmlns="jisp:x:jep-0038"><name/></x><x xmlns="jabber:x:event"><composing/></x></message>' );
	else
		dom.loadXML( '<message type="chat"><body/><html><body/></html><x xmlns="jisp:x:jep-0038"><name/></x><active xmlns="http://jabber.org/protocol/chatstates"/></message>' );

	dom.selectSingleNode( '/message/x[@xmlns="jisp:x:jep-0038"]/name' ).text = external.globals( 'cfg' )( 'emoticonset' );
	dom.documentElement.firstChild.text = document.getElementById( 'send-text' ).value;
	dom.documentElement.setAttribute( 'xml:lang', external.globals( 'language' ) );
	dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
	dom.documentElement.setAttribute( 'to', Tracker.Address.LongAddress() );
	dom.documentElement.setAttribute( 'from', external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) + '/' + external.globals( 'cfg' )( 'resource' ) );

	var HTMLBody = dom.documentElement.selectSingleNode( '/message/html/body' );
	var TextLines = document.getElementById( 'send-text' ).value.split( '\n' );
	HTMLBody.setAttribute( 'style', document.getElementById( 'send-text' ).style.cssText.toLowerCase() );
	HTMLBody.appendChild( dom.createTextNode( TextLines[0] ) );
	for ( var i = 1; i < TextLines.length; ++i )
	{
		HTMLBody.appendChild( dom.createElement( 'br' ) );
		HTMLBody.appendChild( dom.createTextNode( TextLines[i] ) );
	}
	dom.documentElement.selectSingleNode( '/message/html' ).setAttribute( 'xmlns', 'http://jabber.org/protocol/xhtml-im' );
	dom.documentElement.selectSingleNode( '/message/html/body' ).setAttribute( 'xmlns', 'http://www.w3.org/1999/xhtml' );

	if ( Tracker.MessageThread )
	{
		var Thread = dom.createElement( 'thread' );
		Thread.text = Tracker.MessageThread;
		dom.documentElement.appendChild( Thread );
	}

	try
	{
		document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onSendingMessage( dom );
	}
	catch ( e )
	{
	}

	external.wnd.params[0].warn( 'SENT: ' + dom.xml );
	external.XMPP.SendXML( dom );

	var Message = new XMPPMessage();
	Message.FromDOM( dom );
	document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onMessage( Message );
	external.wnd.params[0].history_add( Tracker.Address.ShortAddress(), Message.Stamp, Message.Body, false );

	document.getElementById( 'send-text' ).detachEvent( 'onpropertychange', Typing );
	document.getElementById( 'send-text' ).value = '';
	if ( ! document.getElementById( 'send-text' ).disabled )
		document.getElementById( 'send-text' ).focus();

	if ( Tracker.TypingTimeout )
	{
		clearTimeout( Tracker.TypingTimeout );
		Tracker.TypingTimeout = null;
		try
		{
			document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onComposingLocal( false );
		}
		catch ( e )
		{
		}
	}
	document.getElementById( 'send-text' ).attachEvent( 'onpropertychange', Typing );
}

/* Send typing indicator
 */
function Typing ()
{
	var Tracker = gContainer.Trackers( gContainer.ActiveTrackerAddress );

	if ( Tracker.TypingTimeout )
		clearTimeout( Tracker.TypingTimeout );
	else
	{
		if ( Tracker.ChatState )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<message type="chat"><composing xmlns="http://jabber.org/protocol/chatstates"/></message>' );
			dom.documentElement.setAttribute( 'to', Tracker.Address.LongAddress() );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		if ( Tracker.WantsComposing )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<message><x xmlns="jabber:x:event"><composing/><id/></x></message>' );
			dom.selectSingleNode( '/message/x/id' ).text = Tracker.MessageId;
			dom.documentElement.setAttribute( 'to', Tracker.Address.LongAddress() );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		try
		{
			document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onComposingLocal( true );
		}
		catch ( e )
		{
		}
	}

	Tracker.TypingTimeout = setTimeout( 'StopTyping()', 4000 );
}

/* Cancel typing indicator if the user stopped typing
 */
function StopTyping ()
{
	var Tracker = gContainer.Trackers( gContainer.ActiveTrackerAddress );

	Tracker.TypingTimeout = null;

	if ( Tracker.ChatState )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<message type="chat"><active xmlns="http://jabber.org/protocol/chatstates"/></message>' );
		dom.documentElement.setAttribute( 'to', Tracker.Address.LongAddress() );
		external.wnd.params[0].warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	if ( Tracker.WantsComposing )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<message><x xmlns="jabber:x:event"><id/></x></message>' );
		dom.selectSingleNode( '/message/x/id' ).text = Tracker.MessageId;
		dom.documentElement.setAttribute( 'to', Tracker.Address.LongAddress() );
		external.wnd.params[0].warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	try
	{
		document.frames( 'iframe-' + Tracker.Address.ShortAddress() ).onComposingLocal( false );
	}
	catch ( e )
	{
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
		file.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_file_received' ), 10 );
		file.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_file_history' ), 11 );
		file.AddSeparator();
		file.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_file_close' ), 12 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'chat-container', 'menu_file' ), 0, 1, file );
	}

	if ( ! section || section == 'actions' )
	{
		external.wnd.menuBar.RemItem( 2 );

		var Connected = external.globals( 'XMPPConnected' );
		var InRoster = external.globals( 'ClientRoster' ).Items.Exists( gContainer.ActiveTrackerAddress );
		var IsBlocked = external.globals( 'block' ).Exists( gContainer.ActiveTrackerAddress );
		var IsOnline = gContainer.ActiveTrackerAddress.length
			&& external.globals( 'ClientRoster' ).Items.Exists( gContainer.ActiveTrackerAddress )
			&& external.globals( 'ClientRoster' ).Items( gContainer.ActiveTrackerAddress ).Resources.Count;

		var contact = external.newPopupMenu;
		contact.AddItem( Connected && ! InRoster, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_add' ), 200 );
		contact.AddItem( Connected && InRoster, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_rename' ), 201 );
		contact.AddItem( true, IsBlocked, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_block' ), 202 );

		var actions = external.newPopupMenu;
		actions.AddItem( true, false, false, false, contact.Handle, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_contact' ), 20 );
		actions.AddSeparator();
		actions.AddItem( Connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_profile' ), 22 );
		actions.AddItem( Connected && IsOnline, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_file' ), 23 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_axn' ), 1, 2, actions );
	}

	if ( ! section || section == 'tools' )
	{
		external.wnd.menuBar.RemItem( 3 );

		var cfg = external.globals( 'cfg' );
		var aot = cfg( 'aotchat' ).toString() == 'true';
		var popup = cfg( 'autopopupmsg' ).toString() == 'true';
		var tabs = cfg( 'tabbedchat' ).toString() == 'true';
		var emo = cfg( 'emoticon' ).toString() == 'true';

		var tools = external.newPopupMenu;
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_emo_style' ), 36 );
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_font' ), 30 );
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_background' ),37 );
		tools.AddSeparator();
		tools.AddItem( true, aot, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_aot' ), 31 );
		tools.AddItem( true, popup, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_popup' ), 32 );
		tools.AddItem( true, tabs, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_tabbed' ), 33 );
		tools.AddItem( true, emo, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_emoticons' ), 34 );
		tools.AddSeparator();
		tools.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_tool_settings' ), 35 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_tool' ), 2, 3, tools );
	}

	if ( ! section || section == 'help' )
	{
		external.wnd.menuBar.RemItem( 4 );

		var help = external.newPopupMenu;
		if ( external.globals( 'helpmanual' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_manual' ), 40 );
		if ( external.globals( 'helpforum' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_forum' ), 41 );
		if ( external.globals( 'helpcontact' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_contact' ), 42 );
		if ( external.globals( 'helpmanual' ).length || external.globals( 'helpforum' ).length || external.globals( 'helpcontact' ).length )
			help.AddSeparator();
		help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_about' ), 43 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_help' ), 3, 4, help );
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
		case 10: // received files
			if ( external.Directory.Exists( external.globals( 'cfg' )( 'downloaddir' ) + '\\' ) )
				external.shellExec( 'open', external.globals( 'cfg' )( 'downloaddir' ) + '\\', '', '', 1 );
			else
				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'msg_received_files' ), external.globals( 'softwarename' ), 0 | 48 );
			break;
		case 11: // message history
			external.wnd.params[0].dial_history( gContainer.ActiveTrackerAddress );
			break;
		case 12: //	close
			gContainer.Trackers( gContainer.ActiveTrackerAddress ).Close();
			break;

		case 200: // add to list
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence type="subscribe"/>' );
			dom.documentElement.setAttribute( 'to', gContainer.ActiveTrackerAddress );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
			external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_cl_adding', [ gContainer.ActiveTrackerAddress ] ), external.globals( 'softwarename' ), 0 | 64 );
			break;
		case 201: // rename
			external.globals( 'ClientRoster' ).Items( gContainer.ActiveTrackerAddress ).ChangeName();
			break;
		case 202: // block
			external.wnd.params[0].dial_block( gContainer.ActiveTrackerAddress );
			break;
		case 22: // view profile
			external.wnd.params[0].dial_userinfo( gContainer.Trackers( gContainer.ActiveTrackerAddress ).Address );
			break;
		case 23: // send file
			external.wnd.params[0].dial_file( gContainer.Trackers( gContainer.ActiveTrackerAddress ).Address );
			break;

		case 30: // font
			external.wnd.params[0].dial_font();
			break;
		case 31: // always on top
			cfg( 'aotchat' ) = ! ( cfg( 'aotchat' ).toString() == 'true' );
			var windownames = ( new VBArray( external.windows.Keys() ) ).toArray();
			for ( var i = 0; i < windownames.length; i++ )
				if ( windownames[i].length > 10 && windownames[i].substr( windownames[i].length - 10 ) == '/container' )
					external.windows( windownames[i] ).setAOT( cfg( 'aotchat' ) );
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			break;
		case 32: // popup new messages
			cfg( 'autopopupmsg' ) = ! ( cfg( 'autopopupmsg' ).toString() == 'true' );
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Containers( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			break;
		case 33: // multiple tabs
			cfg( 'tabbedchat' ) = ! ( cfg( 'tabbedchat' ).toString() == 'true' );
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				with ( external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ) )
				{
					MenuBarUpdate( 'tools' );
					HTMLTabBar.style.display = HTMLTabBar.childNodes.length || cfg( 'tabbedchat' ) ? 'inline' : 'none';
				}
			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				with ( external.globals( 'ConferenceSessionPool' ).Containers( ContainerNames[i] ) )
				{
					MenuBarUpdate( 'tools' );
					HTMLTabBar.style.display = HTMLTabBar.childNodes.length || cfg( 'tabbedchat' ) ? 'inline' : 'none';
				}
			break;
		case 34: // display emoticons
			cfg( 'emoticon' ) = ! ( cfg( 'emoticon' ).toString() == 'true' );
			var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Containers( ContainerNames[i] ).MenuBarUpdate( 'tools' );
			break;
		case 35: // settings
			external.wnd.params[0].dial_preferences( '' );
			break;
		case 36: // emoticon style
			external.wnd.params[0].dial_emoticon_list();
			break;
		case 37: // background
			external.wnd.params[0].dial_background_list();
			break;

		case 40: // online manual
			external.wnd.params[0].dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpmanual' ) ) );
			break;
		case 41: // support forum
			external.wnd.params[0].dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpforum' ) ) );
			break;
		case 42: // contact us
			external.wnd.params[0].dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpcontact' ) ) );
			break;
		case 43: // about
			external.wnd.params[0].dial_about();
			break;

		default:
			external.wnd.params[0].warn( 'EVNT: selected menubar option ' + id );
	}
}

/* Enlarge the avatar in a borderless popup window
 */
function AvatarZoom ()
{
	if ( gContainer.Trackers( gContainer.ActiveTrackerAddress ).Avatar.length == 40 && event.button == 1 )
	{
		var W = 112;
		var H = 112;
		var Popup = window.createPopup();
		var Picture = Popup.document.createElement( 'DIV' );
		Picture.attachEvent(
			'onselectstart',
			function ()
			{
				return false;
			}
		);
		Picture.attachEvent(
			'onmousedown',
			function ()
			{
				Popup.hide();
			}
		);
		with ( Picture.style )
		{
			cursor = 'default';
			width = '100%';
			height = '100%';
			border = '1x solid threeddarkshadow';
			background = 'window no-repeat center center';
			backgroundImage = 'url( ' + gContainer.Trackers( gContainer.ActiveTrackerAddress ).AvatarPath + gContainer.Trackers( gContainer.ActiveTrackerAddress ).Avatar + ')';
		}
		Popup.document.Popup = Popup;
		Popup.document.body.insertAdjacentElement( 'beforeEnd', Picture );
		Popup.show( event.x - event.offsetX + event.srcElement.width / 2 - W / 2, event.y - event.offsetY + event.srcElement.height / 2 - H / 2, W, H, document.body );
	}
}

/* Update the last online information
 */
function LastOnline ( IQ )
{
	if ( gContainer.Trackers.Exists( IQ.FromAddress.ShortAddress() ) )
	{
		var Tracker = gContainer.Trackers( IQ.FromAddress.ShortAddress() );
		if ( Tracker.HookIQ )
		{
			Tracker.HookIQ.Destroy();
			Tracker.HookIQ = null;
		}

		/* Display last online time
		 */
		var Query = IQ.XMLDOM.selectSingleNode( '/iq[@type = "result"]/query[@xmlns = "jabber:iq:last" and @seconds]' );
		if ( Query )
		{
			var Seconds = parseInt( Query.getAttribute( 'seconds' ) );
			if ( ! isNaN( Seconds ) )
			{
				with ( new Date() )
				{
					setSeconds( getSeconds() - Seconds );
					Tracker.LastOnline = ( Seconds >= ( ( new Date() ).getHours() + 1 ) * 3600 ) ? toLocaleString() : toLocaleTimeString();
				}
				Tracker.DrawContainerInfo();
			}
		}
	}
}

/* Show context menu at the user bar
 */
function MouseMenu ()
{
	var Tracker = gContainer.Trackers( gContainer.ActiveTrackerAddress );
	var ShortAddress = gContainer.ActiveTrackerAddress;
	var Resource = Tracker.Address.Resource;
	var Connected = external.globals( 'XMPPConnected' );
	var Online = external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) && external.globals( 'ClientRoster' ).Items( ShortAddress ).Resources.Count;
	var InRoster = external.globals( 'ClientRoster' ).Items.Exists( gContainer.ActiveTrackerAddress );
	var IsMe = gContainer.ActiveTrackerAddress == external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' );
	var Blocked = external.globals( 'block' ).Exists( ShortAddress ) && ! IsMe;

	var Menu = external.newPopupMenu;
	Menu.AddItem( Online, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_filetransfer' ), 1 );

	var submenu_rooms = external.newPopupMenu;
	submenu_rooms.AddItem( Connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_room_create' ), 200 );
	var rooms = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray().sort();
	if ( rooms.length )
		submenu_rooms.AddSeparator();
	for ( var i = 0; i < rooms.length; ++i )
		submenu_rooms.AddItem( true, false, false, false, 0, rooms[i].substr( 0, 30 ), 101 + i );
	Menu.AddItem( Online && Resource.length, false, false, false, submenu_rooms.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_room_invite' ), 0 );

	var plugindata = new Array();
	var pluginname = new Array();
	var keys = ( new VBArray( external.globals( 'ClientPluginContainer' ).Plugins.Keys() ) ).toArray();
	for ( var i = 0; i < keys.length; ++i )
		if ( external.globals( 'ClientPluginContainer' ).Plugins( keys[i] ).Menus.Exists( 'roster' ) )
		{
			var pluginoptions = external.globals( 'ClientPluginContainer' ).Plugins( keys[i] ).Menus( 'roster' );
			for ( var j = 0; j < pluginoptions.length; ++j )
			{
				Menu.AddItem( pluginoptions[j].OnlineOnly ? Online : true, false, false, false, 0, pluginoptions[j].Text, 1000 + plugindata.length );
				plugindata.push( pluginoptions[j] );
				pluginname.push( external.globals( 'ClientPluginContainer' ).Plugins( keys[i] ) );
			}
		}

	Menu.AddSeparator();
	Menu.AddItem( Connected && ! InRoster && ! IsMe, false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-container', 'menu_axn_add' ), 3 );
	Menu.AddItem( Connected && InRoster, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_rename' ), 4 );
	Menu.AddItem( ! IsMe, Blocked, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_block' ), 5 );
	Menu.AddSeparator();
	Menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_history' ), 6 );
	Menu.AddItem( Connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_profile' ), 7 );

	Menu.Show( window.screenLeft + 42, window.screenTop + 48, external.globals( 'Translator' ).Direction );

	switch ( Menu.Choice )
	{
		case 1:
			external.wnd.params[0].dial_file( ShortAddress + '/' + Resource );
		break;
		case 200:
			external.wnd.params[0].dial_conference_create();
		break;
		case 3:
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence type="subscribe"/>' );
			dom.documentElement.setAttribute( 'to', gContainer.ActiveTrackerAddress );
			external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
			external.wnd.messageBox( false, external.globals( 'Translator' ).Translate( 'main', 'msg_cl_adding', [ gContainer.ActiveTrackerAddress ] ), external.globals( 'softwarename' ), 0 | 64 );
		break;
		case 4:
			external.wnd.params[0].external.globals( 'ClientRoster' ).Items( ShortAddress ).ChangeName();
		break;
		case 5:
			external.wnd.params[0].dial_block( ShortAddress );
		break;
		case 6:
			external.wnd.params[0].dial_history( ShortAddress );
		break;
		case 7:
			external.wnd.params[0].dial_userinfo( ShortAddress + '/' + Resource );
		break;

		default:
			if ( Menu.Choice > 200 && Menu.Choice - 201 < rooms.length )
			{
				if ( external.globals( 'ConferenceSessionPool' ).Trackers.Exists( rooms[ Menu.Choice - 201 ] ) )
					external.wnd.params[0].dial_conference_invite( external.globals( 'ConferenceSessionPool' ).Trackers( rooms[ Menu.Choice - 201 ] ), ShortAddress, '' );
			}
			else if ( Menu.Choice >= 1000 && Menu.Choice - 1001 < plugindata.length )
			{
				var context = new ActiveXObject( 'Scripting.Dictionary' );
				context.Add( 'address', ShortAddress );
				context.Add( 'resource', Resource );
				context.Add( 'group', '' );
				context.Add( 'name', 'onMenu' );
				context.Add( 'menuname', 'roster' );
				context.Add( 'plugin', pluginname[ Menu.Choice - 1000 ].Id );
				context.Add( 'menuvalue', plugindata[ Menu.Choice - 1000 ].Value );
				external.globals( 'ClientPluginContainer' ).DispatchEvent( context );
			}
	}
}

/* Give focus to text input area when the window is brought to the foreground
 */
function OnWindowActivate ()
{
	if ( ! document.getElementById( 'send-text' ).disabled )
		setTimeout( "if ( ! document.getElementById( 'send-text' ).disabled ) document.getElementById( 'send-text' ).focus();", 0 );
}

/* Show the new tab panel
 */
function TabAddActivate ()
{
	/* Hide chat
	 */
	if ( gContainer.Trackers.Exists( gContainer.ActiveTrackerAddress ) )
		gContainer.Trackers( gContainer.ActiveTrackerAddress ).Deactivate();
	document.getElementById( 'mode-indicator' ).style.display = 'none';
	document.getElementById( 'user-typing-area' ).style.display = 'none';
	document.getElementById( 'conversation' ).style.display = 'none';
	document.getElementById( 'toolbar' ).style.display = 'none';
	document.getElementById( 'input-bar' ).style.display = 'none';

	/* Show the new tab panel
	 */
	document.getElementById( 'new-tab' ).style.display = 'block';
	document.getElementById( 'tab-add-button' ).className = 'tab-add-button-active';
}

/* Hide the new tab panel
 */
function TabAddDeactivate ()
{
	/* Hide chat
	 */
	document.getElementById( 'mode-indicator' ).style.display = 'block';
	document.getElementById( 'user-typing-area' ).style.display = 'block';
	document.getElementById( 'conversation' ).style.display = 'block';
	document.getElementById( 'toolbar' ).style.display = 'block';
	document.getElementById( 'input-bar' ).style.display = 'block';

	/* Show the new tab panel
	 */
	document.getElementById( 'new-tab' ).style.display = 'none';
	document.getElementById( 'tab-add-button' ).className = 'tab-add-button';
}

/* Start a new chat
 */
function TabAddStart ()
{
	external.wnd.params[0].dial_chat( document.getElementById( 'new-tab-address' ).value );
}
