function handle_keyboard ()
{
	var k = event.keyCode;

	if ( k == 9 && event.ctrlKey )
	// tab + ctrl: activate next or previous tab
	{
		event.returnValue = false;
		var buttons = external.globals( 'ClientPluginContainer' ).HTMLTabBar.children;
		for ( var i = 0; i < buttons.length; ++i )
			if ( buttons(i).ClientPluginTab.IsActive )
				break;
		if ( event.shiftKey )
			buttons( i == 0 ? buttons.length - 1 : i-1 ).ClientPluginTab.Activate();
		else
			buttons( i == buttons.length - 1 ? 0 : i+1 ).ClientPluginTab.Activate();
	}

	else if ( k == 112 )
	// F1: manual
	{
		if ( external.globals( 'helpmanual' ).length )
			dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpmanual' ) ) );
	}

	else if ( k == 123 )
	// F12: show console
	{
		dial_console();
	}

	else if ( k == 107 )
	// plus: detailed view
	{
		if ( external.globals( 'cfg' )( 'contactlistdisplay' ) == 'compact' )
		{
			external.globals( 'cfg' )( 'contactlistdisplay' ) = 'detailed';
			var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
			for ( var i = 0; i < TrackerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Trackers( TrackerNames[i] ).RefreshOccupants();
			external.globals( 'ClientRoster' ).RefreshAll();
			MenuBarUpdate( 'tools' );
		}
	}

	else if ( k == 109 )
	// minus: compact view
	{
		if ( external.globals( 'cfg' )( 'contactlistdisplay' ) == 'detailed' )
		{
			external.globals( 'cfg' )( 'contactlistdisplay' ) = 'compact';
			var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
			for ( var i = 0; i < TrackerNames.length; ++i )
				external.globals( 'ConferenceSessionPool' ).Trackers( TrackerNames[i] ).RefreshOccupants();
			external.globals( 'ClientRoster' ).RefreshAll();
			MenuBarUpdate( 'tools' );
		}
	}
}
