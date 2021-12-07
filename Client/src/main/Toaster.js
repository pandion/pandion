var gTimeout = 0;

function Begin ()
{
	external.globals( 'Translator' ).TranslateWindow( 'headline-toaster', document );
	document.onselectstart = document.ondragstart = document.onkeydown = document.onkeyup = document.onkeypress = function(){return false};
	var Headline = external.wnd.params[1];

	if ( Headline.Icon.length == 40 )
	{
		if ( external.FileExists( external.globals( 'usersdir' ) + 'Avatars\\' + Headline.Icon ) )
			document.getElementById( 'icon' ).src = external.globals( 'usersdir' ) + 'Avatars\\' + Headline.Icon;
		else
			document.getElementById( 'icon' ).src = external.globals( 'cwd' ) + '..\\images\\brand\\default.ico';
	}
	else if ( Headline.Icon.length )
	{
		if ( external.FileExists( external.globals( 'cwd' ) + '..\\images\\clients\\small\\' + Headline.Icon ) )
			document.getElementById( 'icon' ).src = external.globals( 'cwd' ) + '..\\images\\clients\\small\\' + Headline.Icon;
		else
			document.getElementById( 'icon' ).src = external.globals( 'cwd' ) + '..\\images\\clients\\' + Headline.Icon;
	}
	else
		document.getElementById( 'icon' ).src = external.globals( 'cwd' ) + '..\\images\\toaster\\star.png';

	document.getElementById( 'title' ).innerText	= Headline.Title.length ? Headline.Title : external.globals( 'softwarename' );
	document.getElementById( 'message' ).innerText	= Headline.Message;

	document.getElementById( 'options' ).parentNode.parentNode.style.display = ( Headline.ShowOptions || Headline.ShowPreferences )  ? '' : 'none';

	document.attachEvent( 'onclick',		Click );
	document.attachEvent( 'onmouseover',	Pause );
	document.attachEvent( 'onmouseout',		Play );

	external.wnd.animate( 0x00000008 | 0x00040000, 125 );
	
	if ( Headline.AutoClose )
		gTimeout = setTimeout( 'Close()', 10000 );
}

function Click ()
{
	var Headline = external.wnd.params[1];
	if ( Headline.OpenArchive )
		external.wnd.params[0].dial_headlines();
	if ( Headline.OpenConversation )
		external.wnd.params[0].dial_chat( Headline.OpenConversation );
	if ( Headline.OpenHyperlink.length )
		external.wnd.params[0].dial_webbrowser( Headline.OpenHyperlink );
	external.wnd.close();
}

function Close ()
{
	external.wnd.animate( 0x00000004 | 0x00040000 | 0x00010000, 250 );
	external.wnd.close();
}

function CloseIfAddress ( Address )
{
   if ( Address )
   {
      var Headline = external.wnd.params[1];
      if (Address == Headline.Address.ShortAddress())
         external.wnd.close();
   }
}

function Pause ()
{
	var Headline = external.wnd.params[1];
	if ( Headline.AutoClose )
		clearTimeout( gTimeout );
}

function Play ()
{
	var Headline = external.wnd.params[1];
	if ( Headline.AutoClose )
		gTimeout = setTimeout( 'Close()', 500 );
}

function Options ()
{
	var Headline = external.wnd.params[1];
	
	if ( Headline.ShowOptions )
		external.wnd.params[0].dial_headlines();
	if ( Headline.ShowPreferences )
		external.wnd.params[0].dial_preferences( Headline.PreferenceSection );
	if ( Headline.CloseOnOptions )  
	{
		external.wnd.close();
		document.detachEvent( 'onclick', Click );
	}
}
