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
	downloadXML();
}

function downloadXML()
{
	var xml = new ActiveXObject( 'MSXML2.DOMDocument' );
	xml.async = true;
	xml.resolveExternals = false;
	xml.onreadystatechange = function ()
	{
		if ( xml.readyState == 4 )
		{
			if ( xml.parseError.errorCode == 0 &&  xml.documentElement )
				handleAtomFeed( xml.documentElement );
			else
				external.wnd.close();
		}
	}
	xml.load( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'autoupdate' ) ) );
}

function handleAtomFeed ( doc )
{
	external.globals( 'last_autoupdate' ) = ( new Date() ).getTime();
	external.wnd.params.SaveCommonProfile();

	/* Get the latest release from the feed
	 */
	var appcastNamespaceUri = 'http://pandion.im/protocol/appcast/1.0';
	var appcasts = doc.selectNodes( '/feed/entry[updated][link[@rel="enclosure"][@href]]' );
	var topAppcastEntry = null;
	var topTimestamp = Number.NEGATIVE_INFINITY;
	for ( var i = 0; i < appcasts.length; i++ )
	{
		if ( appcasts[i].attributes.getQualifiedItem( 'track', appcastNamespaceUri ).value == external.globals( 'softwaretrack' ) )
		{
			var timestamp = appcasts[i].selectSingleNode( 'updated' ).text;
			var timeFromEpoch = timestampToNumber( timestamp );
			if ( timeFromEpoch > topTimestamp )
			{
				topTimestamp = timeFromEpoch;
				topAppcastEntry = {
					arguments: appcasts[i].selectSingleNode( 'link[@rel="enclosure"][@href]' ).attributes.getQualifiedItem( 'arguments', appcastNamespaceUri ),
					name: appcasts[i].attributes.getQualifiedItem( 'name', appcastNamespaceUri ).value,
					enclosure: appcasts[i].selectSingleNode( 'link[@rel="enclosure"]/@href' ).value,
					track: appcasts[i].attributes.getQualifiedItem( 'track', appcastNamespaceUri ).value,
					version: appcasts[i].attributes.getQualifiedItem( 'version', appcastNamespaceUri ).value
				};
				topAppcastEntry[ 'arguments' ] = topAppcastEntry[ 'arguments' ] ? topAppcastEntry[ 'arguments' ].value : '';
				topAppcastEntry[ 'alternate' ] = appcasts[i].selectSingleNode( 'link[@rel="alternate"][@type="text/html" or @type="text/plain"]/@href' );
				topAppcastEntry[ 'alternate' ] = topAppcastEntry[ 'alternate' ] ? topAppcastEntry[ 'alternate' ].value : '';
			}
		}
	}

	/* Check if it's a new version
	 */
	if ( topAppcastEntry )
	{
		var updateVersion = topAppcastEntry.version.split( /[-\. ]/ );
		var currentVersion = external.globals( 'softwareversion' ).split( /[-\. ]/ );
		if ( parseInt( updateVersion[0], 10 ) > parseInt( currentVersion[0], 10 ) // major
			|| ( parseInt( updateVersion[1], 10 ) > parseInt( currentVersion[1], 10 ) // ... or minor
				&& parseInt( updateVersion[0], 10 ) == parseInt( currentVersion[0], 10 ) )
			|| ( parseInt( updateVersion[2], 10 ) > parseInt( currentVersion[2], 10 ) // ... or build
				&& parseInt( updateVersion[1], 10 ) == parseInt( currentVersion[1], 10 )
				&& parseInt( updateVersion[0], 10 ) == parseInt( currentVersion[0], 10 ) )
		)
			showUpdate( topAppcastEntry );
		else
			external.wnd.close();
	}
	else
		external.wnd.close();
}

function timestampToNumber ( timestamp )
{
	                            // 1 BCAD 2 Y          3 M         4 D            5 h         6 m         7 s         8 ms                              9 o   10 oh       11 om
	var match = timestamp.match( /^([-+]?)(\d{4,})(?:-?(\d{2})(?:-?(\d{2})(?:[Tt ](\d{2})(?::?(\d{2})(?::?(\d{2})(?:\.(\d{1,3})(?:\d+)?)?)?)?(?:[Zz]|(?:([-+])(\d{2})(?::?(\d{2}))?)?)?)?)?)?$/ );
	if ( match )
	{
		for ( var ints = [ 2, 3, 4, 5, 6, 7, 8, 10, 11 ], i = ints.length - 1; i >= 0; --i )
			match[ ints[i] ] = ints[i] in match ? parseInt( match[ ints[i] ], 10 ) : 0;
		if ( 1 in match && match[1] === '-' ) // BC/AD
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
		if ( 9 in match ) // offset
			ms += ( match[9] == '+' ? -1 : 1 ) * ( match[10] * 3600 * 1000 + match[11] * 1000 ); // oh om
		if ( match[2] >= 0 && match[2] <= 99 ) // 1-99 AD
			ms -= 59958144000000;
		return ms;
	}
	else
		return Number.NEGATIVE_INFINITY;
}

function showUpdate ( appcastEntry )
{
	gLocation = appcastEntry.alternate;
	gParameters = appcastEntry.arguments;

	document.getElementById( 'txt-title' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'title', [ appcastEntry.name, appcastEntry.version ] );
	document.getElementById( 'txt-continue' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'continue', [ appcastEntry.name ] );
	document.getElementById( 'txt-restart' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'restart', [ appcastEntry.name ] );
	document.getElementById( 'txt-whats-new' ).href = appcastEntry.alternate ? appcastEntry.alternate : external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'softwareurl' ) );

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
		external.wnd.hide( false );
		document.getElementById( 'btn-later' ).select();
	}
	else
	{
		showPageDownload();
		external.wnd.hide( false );
		document.getElementById( 'btn-later' ).select();
	}
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
		external.wnd.params.setTimeout( 'external.wnd.close()', 0 );
	/* Check again in 24 hours
	 */
	external.globals( 'AutoUpdateTimeout' ) = external.wnd.params.setTimeout( 'dial_autoupdate()', /* 7 * */ 24 * 3600 * 1000 );
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

function UpdateProgress ()
{
	if ( gSession )
	{
		var completed = external.HTTPEngine.GetProgress( gSession );
		var total = external.HTTPEngine.GetLength( gSession );
		if ( completed > total )
			completed = total;
		var percentage = total > 0 ? Math.round( 100 * completed / total ) : 0;
		setProgressBar( percentage );
	}
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
