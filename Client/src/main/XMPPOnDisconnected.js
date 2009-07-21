function XMPPOnDisconnected ()
{
	warn( 'EVNT: Disconnected' );

	external.globals( 'HTTPEnginePort' ) = 0;
	external.HTTPEngine.StopListening( 0 );

	external.globals( 'ClientRoster' ).AllOffline();
	external.globals( 'ClientServices' ).Clear();

	external.globals( 'XMPPConnected' )		= false;
	external.globals( 'XMPPEncryption' )	= '';
	external.globals( 'XMPPPrivacy' )		= -1;

	clearTimeout( external.globals( 'AutoUpdateTimeout' ) );

	clearTimeout( external.globals( 'autoaway' ) );
	clearInterval( external.globals( 'XMPPKeepAliveInterval' ) );

	MenuBarUpdate();

	if ( external.globals( 'bannerhalf' ).length )
		BannerDisconnect();

	if ( external.windows.Exists( 'signup' ) )
		external.windows( 'signup' ).Do( 'step1_error', '' );
	else
	{
		var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
		for ( var i = 0; i < ContainerNames.length; ++i )
		{
			if ( external.globals( 'bannerfull' ).length )
				external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).DOM.BannerDisconnect();
			external.globals( 'ChatSessionPool' ).Containers( ContainerNames[i] ).InputUpdate();
		}

		if ( external.globals( 'bannerfull' ).length )
		{
			ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
			for ( var i = 0; i < ContainerNames.length; ++i )
					external.globals( 'ConferenceSessionPool' ).Containers( ContainerNames[i] ).DOM.BannerDisconnect();
		}

		var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
		for ( var i = 0; i < TrackerNames.length; ++i )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<presence type="unavailable"/>' );
			dom.documentElement.setAttribute( 'from', TrackerNames[i] + '/' + external.globals( 'ConferenceSessionPool' ).Trackers( TrackerNames[i] ).Address.Resource );

			var Presence = new XMPPPresence();
			Presence.FromDOM( dom );

			var Event = new ConferenceSessionEvent();
			Event.Type = 'presence';
			Event.Address = Presence.FromAddress;
			Event.Payload = Presence;
			external.globals( 'ConferenceSessionPool' ).AddEvent( Event );
		}

		if ( external.windows.Exists( 'transport_list' ) )
			external.windows( 'transport_list' ).Do( 'transport_refresh', null );

		if ( external.windows.Exists( 'conference_browse' ) )
			external.windows( 'conference_browse' ).Do( 'Refresh', null );

		document.getElementById( 'connecting'	).style.display = 'none';
		document.getElementById( 'welcomemsg'	).style.display = 'none';
		document.getElementById( 'mode-secure'	).style.display = 'none';
		document.getElementById( 'rosterfield'	).style.display = external.globals( 'ClientRoster' ).Items.Count ? 'block' : 'none';
		document.getElementById( 'commerror'	).style.display = external.globals( 'ClientRoster' ).Items.Count ? 'none'  : 'block';
		document.getElementById( 'mode-message'	).innerText = external.globals( 'Translator' ).Translate( 'main', 'cl_status_offline' );

		external.globals( 'XMPPReconnectTimeout' ) = setTimeout(
			function ()
			{
				if ( external.globals( 'XMPPReconnect' ) && ! external.globals( 'XMPPConnected' ) )
				{
					Reconnect();
				}
			},
			10000 + Math.round( Math.random() * 20000 )
		);

		if ( external.globals( 'sspiserver' ).length && external.globals( 'trayonly' ) == 'yes' )
		{
			if ( ! external.globals( 'XMPPSSPIReconnect' ) )
			{
				external.globals( 'XMPPSSPIReconnect' ) = true;
				var Toaster		= new Headline();
				Toaster.Archive	= false;
				Toaster.Title	= external.globals( 'softwarename' );
				Toaster.Message	= external.globals( 'Translator' ).Translate( 'main', 'sspi_autoreconnecting' );
				Toaster.Show();
			}

			setTimeout( "login( ( new XMPPAddress( external.globals( 'sspiserver' ) ) ), '' );", 10000 );
		}

		/* Plugin event
		 */
		var context = new ActiveXObject( 'Scripting.Dictionary' );
		context.Add( 'name', 'onDisconnect' );
		external.globals( 'ClientPluginContainer' ).DispatchEvent( context );
	}
}
