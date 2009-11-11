function dial_conference ( Address, Password, InviteJid, InviteGroup )
{
	var SessionPool = external.globals( 'ConferenceSessionPool' );

	if ( typeof Address == 'string' )
		Address = new XMPPAddress( Address );
	Password = Password || '';
	InviteJid = InviteJid || '';
	InviteGroup = InviteGroup || '';

	var Tracker = SessionPool.GetTracker( Address );
	if ( Tracker )
	{
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
		if ( InviteJid || InviteGroup )
			dial_conference_invite( Tracker, InviteJid, InviteGroup );
		Tracker.Activate();
	}
	else
	{
		var room = {
			'address': Address,
			'password': Password,
			'inviteJid': InviteJid,
			'inviteGroup': InviteGroup
		};
		var TabbedWindows = external.globals( 'cfg' )( 'tabbedchat' ).toString() == 'true';
		if ( TabbedWindows && SessionPool.Containers.Count )
		{
			var ContainerNames = ( new VBArray( SessionPool.Containers.Keys() ) ).toArray();
			SessionPool.Containers( ContainerNames[0] ).CreateTracker( room );
		}
		else if ( TabbedWindows && SessionPool.ContainersLoading.Count )
		{
			if ( ! SessionPool.TrackersLoading.Exists( Address.ShortAddress() ) )
				SessionPool.TrackersLoading.Add( Address.ShortAddress(), room );
		}
		else
		{
			SessionPool.CreateContainer( room );
		}
	}
}
