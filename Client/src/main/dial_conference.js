function dial_conference ( Address, Password )
{
	var SessionPool = external.globals( 'ConferenceSessionPool' );

	if ( typeof Address == 'string' )
		Address = new XMPPAddress( Address );

	if ( ! Password )
		Password = '';

	var TabbedWindows = external.globals( 'cfg' )( 'tabbedchat' ).toString() == 'true';

	if ( SessionPool.GetTracker( Address ) )
	{
		var Tracker = SessionPool.GetTracker( Address );
		if ( Password.length && Tracker.Password != Password )
		{
			Tracker.Password = Password;
			Tracker.SendPresence( external.globals( 'cfg' )( 'lastmode' ), external.globals( 'cfg' )( 'lastmsg' ) );
		}
		if ( Tracker.Address.Resource.length && Tracker.Address.Resource != Address.Resource )
		{
			Tracker.Address = Address;
			Tracker.DrawContainerInfo();
		}
		Tracker.Activate();
	}
	else
	{
		if ( TabbedWindows && SessionPool.Containers.Count )
		{
			var ContainerNames = ( new VBArray( SessionPool.Containers.Keys() ) ).toArray();
			SessionPool.Containers( ContainerNames[0] ).CreateTracker( Address, Password );
		}
		else if ( TabbedWindows && SessionPool.ContainersLoading.Count )
		{
			if ( ! SessionPool.TrackersLoading.Exists( Address.ShortAddress() ) )
			{
				SessionPool.TrackersLoading.Add( Address.ShortAddress(), Address );
				SessionPool.PasswordsLoading.Add( Address.ShortAddress(), Password );
			}
		}
		else
		{
			SessionPool.CreateContainer( Address, Password );
		}
	}
}
