/* An event can be a message, presence, or iq
 */
function ChatSessionEvent ()
{
	this.Type		= '';
	this.Address	= null;
	this.Payload	= null;
}

/* Queues events until a tracker is ready to accept them
 * Holds references to container windows
 * Holds references to session trackers
 * Holds references to loading session trackers
 */
function ChatSessionPool ()
{
	this.Events				= new ActiveXObject( 'Scripting.Dictionary' );
	this.Trackers			= new ActiveXObject( 'Scripting.Dictionary' );
	this.Containers			= new ActiveXObject( 'Scripting.Dictionary' );
	this.TrackersLoading	= new ActiveXObject( 'Scripting.Dictionary' );
	this.ContainersLoading	= new ActiveXObject( 'Scripting.Dictionary' );

	this.Clear				= Clear;
	this.AddEvent			= AddEvent;
	this.AddTracker			= AddTracker;
	this.GetTracker			= GetTracker;
	this.DeleteTracker		= DeleteTracker;
	this.AddContainer		= AddContainer;
	this.CreateContainer	= CreateContainer;
	this.DeleteContainer	= DeleteContainer;

	function Clear ()
	{
		this.Events.RemoveAll();
		this.Trackers.RemoveAll();
		this.Containers.RemoveAll();
		this.TrackersLoading.RemoveAll();
		this.ContainersLoading.RemoveAll();
	}

	function AddEvent ( Event )
	{
		var ShortAddress = Event.Address.ShortAddress();

		var PopupMessages = external.globals( 'cfg' )( 'autopopupmsg' ).toString() == 'true';

		/* Parse the event right now
		 */
		if ( this.Trackers.Exists( ShortAddress ) )
			this.Trackers( ShortAddress ).DispatchEvent( Event );

		/* Queue the event
		 */
		else if ( Event.Type == 'message' && Event.Payload.Body.length )
		{
			if ( this.Events.Exists( ShortAddress ) )
				this.Events( ShortAddress ).push( Event );
			else
				this.Events.Add( ShortAddress, new Array( Event ) );

			/* Should a new tracker be created?
			 * 1. No messages may already be queued.
			 * 2. Auto-popup must be enabled.
			 * 3. Must be Available, Free For Chat, or Invisible.
			 */
			if (
					this.Events( ShortAddress ).length == 1
				&&	external.globals( 'cfg' )( 'autopopupmsg' ).toString() == 'true'
				&&	( external.globals( 'cfg' )( 'lastmode' ) < 2 || external.globals( 'cfg' )( 'lastmode' ) == 5 )
			)
				dial_chat( Event.Address );

			/* Update contact list unread messages counters
			 * Let user know an unread message has been received by using visual and auditive alerts
			 */
			else
			{
				external.globals( 'ClientRoster' ).UnreadMessages();
				if ( ! external.globals( 'ClientRoster' ).IsFlashing )
					external.globals( 'ClientRoster' ).UnreadFlash( -1 * 2 );
				if ( external.globals( 'ClientRoster' ).Items.Exists( Event.Address.ShortAddress() ) )
					external.globals( 'ClientRoster' ).Items( Event.Address.ShortAddress() ).RefreshAll();
				external.wnd.flash( 4 );
			}
			if ( external.globals( 'cfg' )( 'soundmessage' ).toString() == 'true' && ( external.globals( 'cfg' )( 'lastmode' ) < 2 || external.globals( 'cfg' )( 'lastmode' ) == 5 ) )
				sound_play( external.globals( 'cfg' )( 'soundmessagefile' ), false );
		}
	}

	function AddTracker ( Tracker )
	{
		var ShortAddress = Tracker.Address.ShortAddress();

		/* Replay last messages
		 */
		if ( external.globals( 'cfg' )( 'history_store' ).toString() == 'true' )
		{
			var Path		= external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) + '\\';
			var Filename	= ( new MD5() ).digest( ShortAddress );
			var Limit		= 10;
			var Buffer		= new Array();
			var Messages	= new Array();

			if ( this.Events.Exists( ShortAddress ) )
				Limit += this.Events( ShortAddress ).length;

			/* Load last messages from cache
			 */
			if ( external.FileExists( Path + 'Message Cache\\' + Filename + '.buffer' ) )
			{
				var File = external.File( Path + 'Message Cache\\' + Filename + '.buffer' );
				var Rewrite = false;
				while ( ! File.AtEnd )
				{
					var Line = File.ReadLine();
					if ( Line.length > 21 )
					{
						Buffer.push( Line );
						if ( Buffer.length > Limit )
						{
							Buffer.shift();
							Rewrite = true;
						}
					}
				}
				File.Close();
				/* Trim cache to only store the last messages
				 */
				if ( Rewrite )
				{
					external.File( Path + 'Message Cache\\' + Filename + '.buffer' ).Delete();
					File = external.File( Path + 'Message Cache\\' + Filename + '.buffer' );
					if ( Buffer.length )
						File.WriteLine( Buffer.join( '\n' ) );
					File.Close();
				}
			}
			/* Build cache from full log
			 */
			else if ( external.FileExists( Path + Filename + '.log' ) )
			{
				var File = external.File( Path + Filename + '.log' );
				while ( ! File.AtEnd )
				{
					var Line = File.ReadLine();
					if ( Line.length > 21 )
					{
						Buffer.push( Line );
						if ( Buffer.length > Limit )
							Buffer.shift();
					}
				}
				File.Close();
				if ( ! external.Directory.Exists( Path + 'Message Cache' ) )
					external.Directory.Create( external.globals( 'usersdir' ) + 'Message Cache' );
				File = external.File( Path + 'Message Cache\\' + Filename + '.buffer' );
				if ( Buffer.length )
					File.WriteLine( Buffer.join( '\n' ) );
				File.Close();
			}

			/* Do not show queued messages in the history
			 */
			if ( this.Events.Exists( ShortAddress ) )
				Limit = Buffer.length - this.Events( ShortAddress ).length;
			while ( Buffer.length > Limit )
				Buffer.pop();

			if ( Buffer.length )
			{
				/* Only show messages of the last day
				 */
				while ( Buffer[0].substr( 0, 8 ) != Buffer[ Buffer.length - 1 ].substr( 0, 8 ) )
					Buffer.shift();

				/* Convert history format to XMPPMessage objects
				 */
				for ( var i = 0; i < Buffer.length; ++i )
				{
					var Message		= new XMPPMessage();
					Message.Stamp	= Buffer[i].substr( 0, 17 );
					Message.Time	= new Date( Buffer[i].substr( 0, 4 ), Buffer[i].substr( 4, 2 ) - 1, Buffer[i].substr( 6, 2 ), Buffer[i].substr( 9, 2 ), Buffer[i].substr( 12, 2 ), Buffer[i].substr( 15, 2 ) );
					if ( Buffer[i].substr( 18, 3 ) == 'out' )
					{
						Message.From = external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' );
						Message.Body = unescape( Buffer[i].substr( 22 ) );
					}
					else
					{
						Message.From = ShortAddress;
						Message.Body = unescape( Buffer[i].substr( 21 ) );
					}
					Message.FromAddress.Parse( Message.From );
					Messages.push( Message );
				}

				/* Send message history to the session tracker as an event
				 */
				var Event = new ChatSessionEvent();
				Event.Type = 'history';
				Event.Payload = Messages;
				Tracker.DispatchEvent( Event );
			}
		}

		/* Stop queueing events
		 */
		this.Trackers.Add( ShortAddress, Tracker );
		if ( this.TrackersLoading.Exists( ShortAddress ) )
			this.TrackersLoading.Remove( ShortAddress );

		/* Fire all queued events
		 */
		if ( this.Events.Exists( ShortAddress ) )
		{
			/* Temporarily disable sound notification
			 */
			var PlaySounds = external.globals( 'cfg' )( 'soundmessage' );
			external.globals( 'cfg' )( 'soundmessage' ) = false;

			/* Dispatch the messages
			 */
			for ( var i = 0; i < this.Events( ShortAddress ).length; ++i )
				Tracker.DispatchEvent( this.Events( ShortAddress )[i] );
			this.Events.Remove( ShortAddress );

			/* Update the contact list counters
			 */
			external.globals( 'ClientRoster' ).UnreadMessages();
			if ( external.globals( 'ClientRoster' ).Items.Exists( ShortAddress ) )
				external.globals( 'ClientRoster' ).Items( ShortAddress ).RefreshAll();

			/* Reset the sound notification
			 */
			external.globals( 'cfg' )( 'soundmessage' ) = PlaySounds;
		}
	}

	function GetTracker ( Address )
	{
		var ShortAddress = Address.ShortAddress();
		return this.Trackers.Exists( ShortAddress ) ? this.Trackers( ShortAddress ) : null;
	}

	function DeleteTracker ( Tracker )
	{
		var ShortAddress = Tracker.Address.ShortAddress();
		this.Trackers.Remove( ShortAddress )
	}

	function AddContainer ( Container )
	{
		this.Containers.Add( Container.Name, Container );
		this.ContainersLoading.Remove( Container.Name );
	}

	function DeleteContainer ( Container )
	{
		this.Containers.Remove( Container.Name );
	}

	function CreateContainer ( Address )
	{
		var ShortAddress = Address.ShortAddress();
		var ContainerName = external.StringToSHA1( Math.random().toString() );

		this.TrackersLoading.Add( ShortAddress, Address );
		this.ContainersLoading.Add( ContainerName, null );

		with ( external.createWindow( ContainerName + '/container', external.globals( 'cwd' ) + 'chat-container.html', [ window, ContainerName ] ) )
		{
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\letter.ico' );
			MinWidth = 250;
			MinHeight = 210;
			setAOT( external.globals( 'cfg' )( 'aotchat' ).toString() == 'true' );
		}
	}
}
