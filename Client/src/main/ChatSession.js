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
	this.Containers = new ActiveXObject( 'Scripting.Dictionary' );
	this.ContainersLoading = new ActiveXObject( 'Scripting.Dictionary' );
	this.Events = new ActiveXObject( 'Scripting.Dictionary' );
	this.RecentTrackers = [];
	this.Trackers = new ActiveXObject( 'Scripting.Dictionary' );
	this.TrackersLoading = new ActiveXObject( 'Scripting.Dictionary' );

	this.AddContainer = AddContainer;
	this.AddEvent = AddEvent;
	this.AddTracker = AddTracker;
	this.Clear = Clear;
	this.CreateContainer = CreateContainer;
	this.DeleteContainer = DeleteContainer;
	this.DeleteTracker = DeleteTracker;
	this.GetTracker = GetTracker;

	function Clear ()
	{
		this.Containers.RemoveAll();
		this.ContainersLoading.RemoveAll();
		this.Events.RemoveAll();
		this.Trackers.RemoveAll();
		this.TrackersLoading.RemoveAll();
	}

	function AddEvent ( Event )
	{
		var ShortAddress = Event.Address.ShortAddress();

		/* Parse the event right now
		 */
		if ( this.Trackers.Exists( ShortAddress ) )
			this.Trackers.Item( ShortAddress ).DispatchEvent( Event );

		/* Queue the event
		 */
		else if ( Event.Type == 'message' && Event.Payload.Body.length )
		{
			if ( this.Events.Exists( ShortAddress ) )
				this.Events.Item( ShortAddress ).push( Event );
			else
				this.Events.Add( ShortAddress, new Array( Event ) );

			/* Create a new tracker
			 */
			dial_chat( Event.Address );

			/* Play a sound notification for the incoming message.
			 */
			if ( external.globals( 'cfg' ).Item( 'soundmessage' ).toString() == 'true' && Event.Attention == '')
				sound_play( external.globals( 'cfg' ).Item( 'soundmessagefile' ), false );
		}
	}

	function AddTracker ( Tracker )
	{
		var ShortAddress = Tracker.Address.ShortAddress();

		/* Replay last messages
		 */
		if ( external.globals( 'cfg' ).Item( 'history_store' ).toString() == 'true' )
		{
			var Buffer = new Array();
			var Filename = ( new MD5() ).digest( ShortAddress );
			var Limit = 10;
			var Messages = new Array();
			var Path = external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '\\';

			if ( this.Events.Exists( ShortAddress ) )
				Limit += this.Events.Item( ShortAddress ).length;

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
				Limit = Buffer.length - this.Events.Item( ShortAddress ).length;
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
						Message.From = external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' );
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
			var PlaySounds = external.globals( 'cfg' ).Item( 'soundmessage' );
			external.globals( 'cfg' ).Item( 'soundmessage' ) = false;

			/* Dispatch the messages
			 */
			for ( var i = 0; i < this.Events.Item( ShortAddress ).length; ++i )
				Tracker.DispatchEvent( this.Events.Item( ShortAddress )[i] );
			this.Events.Remove( ShortAddress );

			/* Reset the sound notification
			 */
			external.globals( 'cfg' ).Item( 'soundmessage' ) = PlaySounds;
		}

		/* Remove the tracker address from the undo-close tab list
		 */
		for ( var i = 0; i < this.RecentTrackers.length; i++ )
			if ( this.RecentTrackers[i] == ShortAddress )
			{
				this.RecentTrackers.splice( i, 1 );
				break;
			}
	}

	function GetTracker ( Address )
	{
		var ShortAddress = Address.ShortAddress();
		return this.Trackers.Exists( ShortAddress ) ? this.Trackers.Item( ShortAddress ) : null;
	}

	function DeleteTracker ( Tracker )
	{
		var ShortAddress = Tracker.Address.ShortAddress();
		this.Trackers.Remove( ShortAddress );

		/* Store the tracker address in the undo-close tab list
		 */
		this.RecentTrackers.push( ShortAddress );
		if ( this.RecentTrackers.length > 10 )
			this.RecentTrackers.shift();
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
			setIcon( external.globals( 'cwd' ) + '..\\images\\chat-container\\bubble.ico' );
			MinWidth = 180;
			MinHeight = 210;
			setAOT( external.globals( 'cfg' ).Item( 'aotchat' ).toString() == 'true' );
		}
	}
}
