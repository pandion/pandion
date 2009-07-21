function BannerLoad ( BannerWidth, BannerHeight, BannerURL )
{
	var BannerArea = document.getElementById( 'banner-area' );
	while ( BannerArea.firstChild.childNodes.length )
		BannerArea.firstChild.childNodes(0).removeNode( true );

	var Placeholder = document.createElement( 'DIV' );
	Placeholder.attachEvent(
		'onclick',
		function ()
		{
			external.windows( 'MainWindow' ).Do( 'dial_webbrowser', external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'softwareurl' ) ) );
		}
	);
	BannerArea.firstChild.appendChild( Placeholder );

	var BannerFrame = document.createElement( 'IFRAME' );
	with ( BannerFrame )
	{
		src					= 'about:blank';
		style.display		= 'none';
		scrolling			= 'no';
		allowTransparency	= true;
		width				= BannerWidth;
		height				= BannerHeight;
		URL					= BannerURL;
		Delay				= 0;
	}
	BannerArea.firstChild.appendChild( BannerFrame );

	BannerArea.style.display = 'block';
}

function BannerConnect ()
{
	var BannerArea = document.getElementById( 'banner-area' );
	with ( BannerArea.firstChild.childNodes(1) )
	{
		attachEvent( 'onreadystatechange', BannerReadystatechange );
		src = URL;
	}
}

function BannerDisconnect ()
{
	var BannerArea = document.getElementById( 'banner-area' );
	BannerArea.firstChild.childNodes(0).style.display = 'block';
	with ( BannerArea.firstChild.childNodes(1) )
	{
		if ( Delay )
		{
			clearTimeout( Delay );
			Delay = 0;
		}
		detachEvent( 'onreadystatechange', BannerReadystatechange );
		style.display	= 'none';
		src				= 'about:blank';
	}
}

function BannerReadystatechange ()
{
	if ( event.srcElement.readyState == 'complete' )
	{
		try
		{
			if ( event.srcElement.contentWindow.location.protocol )
			{
				BannerDisconnect();
				BannerDelayConnect( 5 * 60 * 1000 );
			}
		}
		catch ( e )
		{
			event.srcElement.parentNode.childNodes(0).style.display = 'none';
			event.srcElement.parentNode.childNodes(1).style.display = 'block';
			setTimeout( 'BannerResetFocus()', 0 );
		}
	}
}

function BannerDelayConnect ( Delay )
{
	var BannerArea = document.getElementById( 'banner-area' );
	if ( BannerArea.firstChild.childNodes(1).Delay )
		clearTimeout( BannerArea.firstChild.childNodes(1).Delay );
	BannerArea.firstChild.childNodes(1).Delay = setTimeout( 'BannerConnect()', Delay );
}

function BannerResetFocus ()
{
	if ( document.getElementById( 'send-text' ) && ! document.getElementById( 'send-text' ).disabled )
		document.getElementById( 'send-text' ).focus();
}
