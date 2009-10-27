window.attachEvent( 'onload', Begin );
window.attachEvent( 'onunload', End );

var gFileName, gSession, gInterval, gParameters, gLocation = '';
var gShutdownOnClose = false;

var HTTPOnTimeout = HTTPOnSockErr = OnHTTPFileErr = HTTPOnTransferAborted = function ( sessionID, error ) {
	if ( sessionID == gSession )
	{
		clearInterval( gInterval );
		gSession = 0;
		showPageDownload();
	}
}

function HTTPGetUpdateProgress ( sessionID )
{
	if ( sessionID != gSession )
		return;
	var total = external.HTTPEngine.GetLength( sessionID );
	var completed = external.HTTPEngine.GetProgress( sessionID );
	if ( completed > total )
		completed = total;
	var percentage = total > 0 ? Math.round( 100 * completed / total ) : 0;
	setProgressBar( percentage );
}

function HTTPOnConnected ( sessionID, remoteHost, remotePort, localPort )
{
	if ( sessionID != gSession )
		return;
	gInterval = setInterval( 'HTTPGetUpdateProgress(' + sessionID + ')', 100 );
}

function HTTPOnTransferComplete ( sessionID )
{
	if ( sessionID != gSession )
		return;
	clearInterval( gInterval );
	gSession = 0;
	showPageUpdate();
	saveUpdateParameters();
}

function Begin ()
{
	external.globals( 'Translator' ).TranslateWindow( 'autoupdate', document );
	document.onselectstart = document.ondragstart = function(){return false};
	document.oncontextmenu = function(){return false};
	if ( external.wnd.params.selectVersion )
	{
		document.getElementById( 'tracks-version' ).innerText = external.globals( 'softwareversion' ) + ' ' + external.globals( 'softwaretrack' );
		setDisplay( 'panel-tracks', 'block' );
		document.getElementById( 'btn-close' ).focus();
		external.wnd.hide( false );
		downloadXML( showAvailableUpdates );
	}
	else
	{
		setDisplay( 'panel-progress', 'block' );
		downloadXML( autoDownloadLatestUpdate );
	}
}

function downloadXML( callback )
{
	var xml = new ActiveXObject( 'MSXML2.DOMDocument' );
	xml.async = true;
	xml.resolveExternals = false;
	xml.onreadystatechange = function () {
		if ( xml.readyState == 4 )
			callback( ( xml.parseError.errorCode == 0 && xml.documentElement ) ? xml.documentElement : null );
	};
	xml.load( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'autoupdate' ) ) );
}

function autoDownloadLatestUpdate ( doc )
{
	if ( doc )
	{
		var currentAppcast = getAppcastByVersion( doc, external.globals( 'softwareversion' ) );
		if ( currentAppcast && currentAppcast.track.length > 0 )
			external.globals( 'softwaretrack' ) = currentAppcast.track;
		external.globals( 'last_autoupdate' ) = ( new Date() ).getTime();
		external.wnd.params.window.SaveCommonProfile();
		var topAppcastEntry = getLatestAppcastByTrack( doc, external.globals( 'softwaretrack' ) );
		if ( topAppcastEntry && versionIsHigherThanCurrent( topAppcastEntry.version ) )
			downloadUpdate( topAppcastEntry );
		else
			external.wnd.close();
	}
	else
		external.wnd.close();
}

function showAvailableUpdates ( doc )
{
	if ( doc )
	{
		var tracks = {
			stable: getLatestAppcastByTrack( doc, 'stable' ),
			beta: getLatestAppcastByTrack( doc, 'beta' ),
			development: getLatestAppcastByTrack( doc, 'development' )
		};
		var descriptions = {
			stable: external.globals( 'Translator' ).Translate( 'autoupdate', 'description-stable' ),
			beta: external.globals( 'Translator' ).Translate( 'autoupdate', 'description-beta' ),
			development: external.globals( 'Translator' ).Translate( 'autoupdate', 'description-development' )
		};
		var insertSimpleElement = function ( containerElement, tagName, content )
		{
			var childElement = document.createElement( tagName );
			childElement.innerText = content;
			containerElement.appendChild( childElement );
			return childElement;
		};
		for ( var name in tracks )
			if ( tracks[ name ] )
			{
				var button = document.createElement( 'BUTTON' );
				button.className = 'update-button';
				insertSimpleElement( button, 'H4', tracks[ name ].name + ' ' + tracks[ name ].version + ' ' + name );
				if ( tracks[ name ].version === external.globals( 'softwareversion' ) )
				{
					button.disabled = true;
					insertSimpleElement( button, 'DIV', external.globals( 'Translator' ).Translate( 'autoupdate', 'up-to-date' ) );
				}
				else if ( name === 'stable' )
					insertSimpleElement( button, 'DIV', external.globals( 'Translator' ).Translate( 'autoupdate', 'recommended' ) );
				insertSimpleElement( button, 'P', descriptions[ name ] );
				document.getElementById( 'tracks-' + ( name === external.globals( 'softwaretrack' ) ? 'current' : 'alternative' ) ).appendChild( button );
				button.onclick = (function ( name ) { return function () {
					setDisplay( 'panel-tracks', 'none' );
					setDisplay( 'panel-progress', 'block' );
					external.globals( 'softwaretrack' ) = name;
					downloadUpdate( tracks[ name ] );
				} } )( name );
			}
		setDisplay( 'preloader', 'none' );
		setDisplay( 'txt-latest', document.getElementById( 'tracks-current' ).childNodes.length ? 'block' : 'none' );
		setDisplay( 'txt-alternatives', document.getElementById( 'tracks-alternative' ).childNodes.length ? 'block' : 'none' );
		if ( document.getElementById( 'tracks-current' ).childNodes.length + document.getElementById( 'tracks-alternative' ).childNodes.length === 0 )
			setDisplay( 'txt-unavailable', 'block' );
	}
	else
	{
		setDisplay( 'preloader', 'none' );
		setDisplay( 'txt-no-feed', 'block' );
	}
}

function getLatestAppcastByTrack ( doc, track )
{
	var appcastNamespaceUri = 'http://pandion.im/protocol/appcast/1.0';
	var appcasts = getAppcastsFromFeed( doc );
	var topAppcastEntry = null;
	var topTimestamp = Number.NEGATIVE_INFINITY;
	for ( var i = 0; i < appcasts.length; i++ )
		if ( appcasts[i].attributes.getQualifiedItem( 'track', appcastNamespaceUri ).value === track )
		{
			var timestamp = appcasts[i].selectSingleNode( 'updated' ).text;
			var timeFromEpoch = timestampToNumber( timestamp );
			if ( timeFromEpoch > topTimestamp )
			{
				topTimestamp = timeFromEpoch;
				topAppcastEntry = parseAppcastFromEntry( appcasts[i] );
			}
		}
	return topAppcastEntry;
}

function getAppcastByVersion ( doc, version )
{
	var appcastNamespaceUri = 'http://pandion.im/protocol/appcast/1.0';
	var appcasts = getAppcastsFromFeed( doc );
	for ( var i = 0; i < appcasts.length; i++ )
		if ( appcasts[i].attributes.getQualifiedItem( 'version', appcastNamespaceUri ).value === external.globals( 'softwareversion' ) )
			return parseAppcastFromEntry( appcasts[i] );
	return null;
}

function getAppcastsFromFeed ( doc )
{
	return doc.selectNodes( '/feed/entry[updated][link[@rel="enclosure"][@href]]' );
}

function parseAppcastFromEntry ( entry )
{
	var appcastNamespaceUri = 'http://pandion.im/protocol/appcast/1.0';
	var appcast = {
		arguments: entry.selectSingleNode( 'link[@rel="enclosure"][@href]' ).attributes.getQualifiedItem( 'arguments', appcastNamespaceUri ),
		name: entry.attributes.getQualifiedItem( 'name', appcastNamespaceUri ).value,
		enclosure: entry.selectSingleNode( 'link[@rel="enclosure"]/@href' ).value,
		track: entry.attributes.getQualifiedItem( 'track', appcastNamespaceUri ).value,
		version: entry.attributes.getQualifiedItem( 'version', appcastNamespaceUri ).value
	};
	appcast[ 'arguments' ] = appcast[ 'arguments' ] ? appcast[ 'arguments' ].value : '';
	appcast[ 'alternate' ] = entry.selectSingleNode( 'link[@rel="alternate"][@type="text/html" or @type="text/plain"]/@href' );
	appcast[ 'alternate' ] = appcast[ 'alternate' ] ? appcast[ 'alternate' ].value : '';
	return appcast;
}

function timestampToNumber ( timestamp )
{
	                            // 1 BCAD 2 Y          3 M         4 D            5 h         6 m         7 s         8 ms                              9 o   10 oh       11 om
	var match = timestamp.match( /^([-+]?)(\d{4,})(?:-?(\d{2})(?:-?(\d{2})(?:[Tt ](\d{2})(?::?(\d{2})(?::?(\d{2})(?:\.(\d{1,3})(?:\d+)?)?)?)?(?:[Zz]|(?:([-+])(\d{2})(?::?(\d{2}))?)?)?)?)?)?$/ );
	if ( match )
	{
		for ( var ints = [ 2, 3, 4, 5, 6, 7, 8, 10, 11 ], i = ints.length - 1; i >= 0; --i )
			match[ ints[i] ] = match[ints[i]].length > 0 ? parseInt( match[ ints[i] ], 10 ) : 0;
		if ( match[1] == '-' ) // BC/AD
			match[2] *= -1;
		var ms = Date.UTC(
			match[2], // Y
			match[3] - 1, // M
			match[4], // D
			match[5], // h
			match[6], // m
			match[7], // s
			match[8] // ms
		);
		if ( match[9].length > 0 ) // offset
			ms += ( match[9] == '+' ? -1 : 1 ) * ( match[10] * 3600 * 1000 + match[11] * 1000 ); // oh om
		if ( match[2] >= 0 && match[2] <= 99 ) // 1-99 AD
			ms -= 59958144000000;
		return ms;
	}
	else
		return Number.NEGATIVE_INFINITY;
}

function versionIsHigherThanCurrent ( version )
{
	var versionNumbers = version.split( /[-\. ]/ );
	var currentVersion = external.globals( 'softwareversion' ).split( /[-\. ]/ );
	return ( parseInt( versionNumbers[0], 10 ) > parseInt( currentVersion[0], 10 ) // major
		|| ( parseInt( versionNumbers[1], 10 ) > parseInt( currentVersion[1], 10 ) // ... or minor
			&& parseInt( versionNumbers[0], 10 ) == parseInt( currentVersion[0], 10 ) )
		|| ( parseInt( versionNumbers[2], 10 ) > parseInt( currentVersion[2], 10 ) // ... or build
			&& parseInt( versionNumbers[1], 10 ) == parseInt( currentVersion[1], 10 )
			&& parseInt( versionNumbers[0], 10 ) == parseInt( currentVersion[0], 10 ) )
	);
}

function downloadUpdate ( appcastEntry )
{
	gLocation = appcastEntry.alternate;
	gParameters = appcastEntry.arguments;

	document.getElementById( 'txt-version' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'title', [ appcastEntry.name, appcastEntry.version ] );
	document.getElementById( 'txt-whats-new' ).href = appcastEntry.alternate ? appcastEntry.alternate : external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'softwareurl' ) );
	document.getElementById( 'progress-version' ).innerText = external.globals( 'softwareversion' ) + ' ' + external.globals( 'softwaretrack' );

	if ( appcastEntry.enclosure.length )
	{
		appcastEntry.enclosure = appcastEntry.enclosure.substr( appcastEntry.enclosure.indexOf( '//' ) + 2 );

		var Path = appcastEntry.enclosure;
		if ( appcastEntry.enclosure.indexOf( '/' ) != -1 )
			Path = appcastEntry.enclosure.substr( appcastEntry.enclosure.indexOf( '/' ) );
		else
			Path = '/';

		if ( appcastEntry.enclosure.indexOf( '?' ) != -1 )
			appcastEntry.enclosure = appcastEntry.enclosure.substr( 0, appcastEntry.enclosure.indexOf( '?' ) );

		var Host = appcastEntry.enclosure;
		if ( appcastEntry.enclosure.indexOf( '/' ) != -1 )
			Host = appcastEntry.enclosure.substr( 0, appcastEntry.enclosure.indexOf( '/' ) );

		var Port = 80;
		if ( Host.indexOf( ':' ) != -1 )
		{
			Port = parseInt( Host.substr( Host.lastIndexOf( ':' ) + 1 ) );
			Host = Host.substr( 0, Host.indexOf( ':' ) );
		}
		if ( isNaN( Port ) )
			Port = 80;

		if ( appcastEntry.enclosure.lastIndexOf( '/' ) != -1 )
			gFileName = decodeURIComponent( appcastEntry.enclosure.substr( appcastEntry.enclosure.lastIndexOf( '/' ) + 1 ).replace( /\\\:\*\?\"\<\>\|/, '' ) );
		else
			gFileName = 'noname';
		if ( ! gFileName.length )
			gFileName = 'noname';

		var DownloadLocation = external.globals( 'usersdir' ) + 'Downloads\\';

		if ( ! external.Directory.Exists( DownloadLocation ) )
			external.Directory.Create( DownloadLocation );

		if ( external.FileExists( DownloadLocation + gFileName ) )
			external.file( DownloadLocation + gFileName ).Delete();

		external.HTTPEngine.subscribe( external.wnd );
		gSession = external.HTTPEngine.Get( DownloadLocation + gFileName, Path, 0, 0xFFFFFFFF, Host, Port );

		showPageProgress();
	}
	else
	{
		showPageDownload();
	}

	external.wnd.hide( false );
	document.getElementById( 'btn-later' ).focus();
}

function setDisplay ( id, mode )
{
	document.getElementById( id ).style.display = mode;
}

function showPageProgress ()
{
	setDisplay( 'progress-area', 'block' );
	setDisplay( 'txt-continue', 'block' );
	setDisplay( 'txt-restart', 'none' );
	setDisplay( 'txt-browser', 'none' );
	setDisplay( 'btn-update', 'none' );
	setDisplay( 'btn-download', 'none' );
}

function showPageUpdate ()
{
	setDisplay( 'progress-area', 'none' );
	setDisplay( 'txt-continue', 'none' );
	setDisplay( 'txt-restart', 'block' );
	setDisplay( 'txt-browser', 'none' );
	setDisplay( 'btn-update', 'inline' );
	setDisplay( 'btn-download', 'none' );
}

function showPageDownload ()
{
	setDisplay( 'progress-area', 'none' );
	setDisplay( 'txt-continue', 'none' );
	setDisplay( 'txt-restart', 'none' );
	setDisplay( 'txt-browser', 'block' );
	setDisplay( 'btn-update', 'none' );
	setDisplay( 'btn-download', 'inline' );
}

function End ()
{
	external.HTTPEngine.unsubscribe( external.wnd );
	if ( gSession )
		external.HTTPEngine.Abort( gSession );
	if ( gShutdownOnClose )
		external.wnd.params.window.setTimeout( 'external.wnd.close()', 0 );
	if ( ! external.wnd.params.selectVersion )
		external.globals( 'AutoUpdateTimeout' ) = external.wnd.params.window.setTimeout( 'dial_autoupdate( false )', /* 7 * */ 24 * 3600 * 1000 );
}

function update ()
{
	var DownloadLocation = external.globals( 'usersdir' ) + 'Downloads\\';
	if ( external.FileExists( DownloadLocation + 'Update.txt' ) )
		external.File( DownloadLocation + 'Update.txt' ).Delete();
	if ( ! external.FileExists( DownloadLocation + 'Update.txt' ) )
	{
		external.globals.Add( 'autoupdatecommand', DownloadLocation + gFileName );
		external.globals.Add( 'autoupdateparameters', gParameters );
		external.globals.Add( 'autoupdatedirectory', DownloadLocation );
		gShutdownOnClose = true;
		external.wnd.close();
	}
}

function download ()
{
	external.wnd.hide( true );
	external.windows( 'MainWindow' ).Do( 'dial_webbrowser', gLocation )
	external.wnd.close();
}

function setProgressBar ( percentage )
{
	document.getElementById( 'progress-remaining' ).style.clip = 'rect( auto auto auto ' + percentage + '% )';
	document.getElementById( 'progress-completed' ).innerText = document.getElementById( 'progress-remaining' ).innerText = percentage + '%';
}

function saveUpdateParameters ()
{
	var path = external.globals( 'usersdir' ) + 'Downloads\\Update.txt';
	if ( external.FileExists( path ) )
		external.File( path ).Delete();
	var file = external.File( path );
	file.WriteLine( gFileName );
	file.WriteLine( gParameters );
	file.Close();
}
