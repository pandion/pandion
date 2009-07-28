window.attachEvent( 'onload', Begin );
window.attachEvent( 'onunload', End );

var gFileName, gSession, gInterval, gParameters, gLocation = '';
var gShutdownOnClose = false;
var gXML = new ActiveXObject( 'MSXML2.DOMDocument' );

var HTTPOnTimeout = HTTPOnSockErr = OnHTTPFileErr = HTTPOnTransferAborted =
	function ( sessionID, error )
	{
		if ( sessionID == gSession )
		{
//			alert("HTTPError Called");
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
//	alert("HTTPOnConnected Called");
	if ( sessionID != gSession )
		return;
	gInterval = setInterval( 'HTTPGetUpdateProgress(' + sessionID + ')', 100 );
}

function HTTPOnTransferComplete ( sessionID )
{
//	alert("HTTPOnTransferComplete Called");
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
	gXML.async = true;
	gXML.resolveExternals = false;
	gXML.onreadystatechange = checkXMLReadystate;
	gXML.load( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'autoupdate' ) ) );
}

function checkXMLReadystate ()
{
	if ( gXML.readyState == 4 )
	{
		if ( gXML.parseError.errorCode == 0 && gXML.documentElement )
			checkUpdateVersion();
		else
			external.wnd.close();
	}
}

function checkUpdateVersion ()
{
//	alert("checkUpdateVersion Called");
	if ( gXML.documentElement )
	{
		external.globals( 'last_autoupdate' ) = ( new Date() ).getTime();
		external.wnd.params.SaveCommonProfile();

		var Latest = gXML.documentElement.selectSingleNode( '/versions/latest' );
		if ( Latest )
		{
			var Name		= getAttribute( Latest, 'name' );
			var Version		= getAttribute( Latest, 'ordinal' );
			var Installer	= getAttribute( Latest, 'installer' );
			var Changelog	= getAttribute( Latest, 'changelog' );
			gLocation		= getAttribute( Latest, 'download' );
			gParameters		= getAttribute( Latest, 'parameters' );

			document.getElementById( 'txt-title' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'title', [ Name, Version ] );
			document.getElementById( 'txt-continue' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'continue', [ Name ] );
			document.getElementById( 'txt-restart' ).innerText = external.globals( 'Translator' ).Translate( 'autoupdate', 'restart', [ Name ] );
			document.getElementById( 'txt-whats-new' ).href = Changelog ? Changelog : external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'softwareurl' ) );

			if ( Name.length && Version.length && ( gLocation.length || ( Installer.length && Installer.indexOf( '//' ) != -1 ) ) )
			{
				if ( Installer.length )
				{
					Installer = Installer.substr( Installer.indexOf( '//' ) + 2 );

					var Path = Installer;
					if ( Installer.indexOf( '/' ) != -1 )
						Path = Installer.substr( Installer.indexOf( '/' ) );
					else
						Path = '/';

					if ( Installer.indexOf( '?' ) != -1 )
						Installer = Installer.substr( 0, Installer.indexOf( '?' ) );

					var Host = Installer;
					if ( Installer.indexOf( '/' ) != -1 )
						Host = Installer.substr( 0, Installer.indexOf( '/' ) );

					var Port = 80;
					if ( Host.indexOf( ':' ) != -1 )
					{
						Port = parseInt( Host.substr( Host.lastIndexOf( ':' ) + 1 ) );
						Host = Host.substr( 0, Host.indexOf( ':' ) );
					}
					if ( isNaN( Port ) )
						Port = 80;

					gFileName = 'Update.exe';
/*					gFileName = Installer;
					if ( Installer.lastIndexOf( '/' ) != -1 )
						gFileName = decodeURIComponent( Installer.substr( Installer.lastIndexOf( '/' ) + 1 ).replace( /\\\:\*\?\"\<\>\|/, '' ) );
					else
						gFileName = 'noname';
					if ( ! gFileName.length )
						gFileName = 'noname';
*/
					var DownloadLocation = external.globals( 'usersdir' ) + 'Downloads\\';

					if ( ! external.Directory.Exists( DownloadLocation ) )
						external.Directory.Create( DownloadLocation );

					if ( external.FileExists( DownloadLocation + gFileName ) )
						external.file( DownloadLocation + gFileName ).Delete();

					external.HTTPEngine.subscribe( external.wnd );
					gSession = external.HTTPEngine.Get( DownloadLocation + gFileName, Path, 0, 0xFFFFFFFF, Host, Port );

					if ( gSession )
					{
						showPageProgress();
						external.wnd.hide( false );
						document.getElementById( 'btn-later' ).select();
						return;
					}
				}

				showPageDownload();
				external.wnd.hide( false );
				document.getElementById( 'btn-later' ).select();
			}
		}
		/* Check again in 24 hours
		 */
		external.globals( 'AutoUpdateTimeout' ) = external.wnd.params.setTimeout( 'dial_autoupdate()', /* 7 * */ 24 * 3600 * 1000 );
	}
	external.wnd.close();
}

function getAttribute ( node, attribute )
{
	var value = node.getAttribute( attribute );
	return value ? value : '';
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
}

function update ()
{
	var DownloadLocation = external.globals( 'usersdir' ) + 'Downloads\\';

	if ( external.FileExists( DownloadLocation + 'UpdateParameters.txt' ) )
		external.File( DownloadLocation + 'UpdateParameters.txt' ).Delete();

	if ( ! external.FileExists( DownloadLocation + 'UpdateParameters.txt' ) )
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
	var path = external.globals( 'usersdir' ) + 'Downloads\\UpdateParameters.txt';
	if ( external.FileExists( path ) )
		external.File( path ).Delete();
	var file = external.File( path );
	file.WriteLine( gParameters );
	file.Close();
}
