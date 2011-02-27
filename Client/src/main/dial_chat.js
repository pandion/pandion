function dial_chat ( Address )
{
	var SessionPool = external.globals( 'ChatSessionPool' );

	if ( typeof Address == 'string' )
		Address = new XMPPAddress( Address );

	var TabbedWindows = external.globals( 'cfg' ).Item( 'tabbedchat' ).toString() == 'true';

	if ( SessionPool.GetTracker( Address ) )
	{
		var Tracker = SessionPool.GetTracker( Address );
		if ( Tracker.Address.LongAddress() != Address.LongAddress() )
		{
			Tracker.Address = Address;
			Tracker.DrawContainerInfo();
		}
		Tracker.Activate( true );
	}
	else
	{
		if ( TabbedWindows && SessionPool.Containers.Count )
		{
			var ContainerNames = ( new VBArray( SessionPool.Containers.Keys() ) ).toArray();
			SessionPool.Containers.Item( ContainerNames[0] ).CreateTracker( Address );
		}
		else if ( TabbedWindows && SessionPool.ContainersLoading.Count )
		{
			if ( ! SessionPool.TrackersLoading.Exists( Address.ShortAddress() ) )
				SessionPool.TrackersLoading.Add( Address.ShortAddress(), Address );
		}
		else if ( ! SessionPool.TrackersLoading.Exists( Address.ShortAddress() ) )
		{
			SessionPool.CreateContainer( Address );
		}
	}
}
