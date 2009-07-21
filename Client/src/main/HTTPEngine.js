function HTTPOnListening ( localPort )
{
	warn( 'EVNT: HTTPEngine listening at port ' + localPort );
	external.globals( 'HTTPEnginePort' ) = localPort;
}

function HTTPSetProxy ( protocol, remotehost )
{
	try
	{
		var proxyenable = external.RegRead( 'HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings', 'ProxyEnable' ) == 1;

		var remotehostislocal = ! remotehost.match( /[^0-9\.]/ ) && ( remotehost.substr( 0, 9 ) == '127.0.0.1' || remotehost.substr( 0, 8 ) == '192.168.' || remotehost.substr( 0, 3 ) == '10.' || remotehost.substr( 0, 7 ) == '172.16.' );

		var bypassproxy = false;
		var proxyoverride = external.RegRead( 'HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings', 'ProxyOverride' ).replace( /\s/gm, '' ).split( ';' );
		for ( var i = 0; i < proxyoverride.length; ++i )
			if ( ( proxyoverride[i] == '<local>' && remotehostislocal ) || remotehost.substr( 0, proxyoverride[i].length ) == proxyoverride[i] )
			{
				bypassproxy = true;
				break;
			}

		var proxyport = 0;
		var proxyhost = '';
		if ( proxyenable && ! bypassproxy )
		{
			proxyport = 80;
			var str = external.RegRead( 'HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings', 'ProxyServer' ).toString();
			if ( str.indexOf( '=' ) == -1 )
			{
				proxyhost = str;
				if ( str.indexOf( ':' ) != -1 )
				{
					proxyport = parseInt( str.substr( str.lastIndexOf( ':' ) + 1 ) );
					proxyhost = str.substr( 0, str.indexOf( ':' ) );
				}
				if ( isNaN( proxyport ) )
					throw '';
			}
			else
			{
				var pairs = str.split( ';' );
				for ( var i = 0; i < pairs.length; ++i )
				{
					var pair = pairs[i].split( '=' );
					if ( pair[0] == protocol )
					{
						proxyhost = pair[1];
						if ( pair[1].indexOf( ':' ) != -1 )
						{
							proxyport = parseInt( pair[1].substr( pair[1].lastIndexOf( ':' ) + 1 ) );
							proxyhost = pair[1].substr( 0, pair[1].indexOf( ':' ) );
						}
						if ( isNaN( proxyport ) )
							proxyport = 80;
						break;
					}
				}
			}
		}
		external.HTTPEngine.SetProxyInfo( proxyhost, proxyport, '', '' );
	}
	catch ( e )
	{
	}
}
