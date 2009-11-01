/* An event can be a message, presence, or iq
 */
function ConferenceSessionEvent ()
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
function ConferenceSessionPool ()
{
	this.Events				= new ActiveXObject( 'Scripting.Dictionary' );
	this.Trackers			= new ActiveXObject( 'Scripting.Dictionary' );
	this.Containers			= new ActiveXObject( 'Scripting.Dictionary' );
	this.TrackersLoading	= new ActiveXObject( 'Scripting.Dictionary' );
	this.PasswordsLoading	= new ActiveXObject( 'Scripting.Dictionary' );
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
		this.PasswordsLoading.RemoveAll();
		this.ContainersLoading.RemoveAll();
	}

	function AddEvent ( Event )
	{
		var ShortAddress = Event.Address.ShortAddress();

		/* Parse the event right now
		 */
		if ( this.Trackers.Exists( ShortAddress ) )
			this.Trackers( ShortAddress ).DispatchEvent( Event );

		/* Queue the event
		 */
		else if ( this.TrackersLoading.Exists( ShortAddress ) )
		{
			if ( this.Events.Exists( ShortAddress ) )
				this.Events( ShortAddress ).push( Event );
			else
				this.Events.Add( ShortAddress, new Array( Event ) );

			if ( external.globals( 'cfg' )( 'soundmessage' ).toString() == 'true' && Event.Type == 'message' && Event.Payload.Body.length && ( external.globals( 'cfg' )( 'lastmode' ) < 2 || external.globals( 'cfg' )( 'lastmode' ) == 5 ) )
				sound_play( external.globals( 'cfg' )( 'soundmessagefile' ), false );
		}
	}

	function AddTracker ( Tracker )
	{
		var ShortAddress = Tracker.Address.ShortAddress();

		/* Stop queueing events
		 */
		this.Trackers.Add( ShortAddress, Tracker );
		if ( this.TrackersLoading.Exists( ShortAddress ) )
		{
			this.TrackersLoading.Remove( ShortAddress );
			this.PasswordsLoading.Remove( ShortAddress );
		}

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

	function CreateContainer ( Address, Password )
	{
		var cfg = external.globals( 'cfg' );
		var ShortAddress = Address.ShortAddress();
		var ContainerName = external.StringToSHA1( Math.random().toString() );

		this.TrackersLoading.Add( ShortAddress, Address );
		this.PasswordsLoading.Add( ShortAddress, Password );
		this.ContainersLoading.Add( ContainerName, null );

		with ( external.createWindow( ContainerName + '/container', external.globals( 'cwd' ) + 'conference-container.html', [ window, ContainerName ] ) )
		{
			setIcon( external.globals( 'cwd' ) + '..\\images\\conference\\contacts.ico' );
			MinWidth = 250;
			MinHeight = 210;
		}
	}
}
