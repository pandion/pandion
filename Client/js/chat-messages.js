/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
/* Create a global variable to communicate with the rest of the conversation window
 */
if ( ! window.SessionTracker )
	window.SessionTracker = window.frameElement.SessionTracker;

/* Global variables used by this background
 */
var gLastDate = ( new Date() ).toLocaleDateString();
var gLastTime = '';
var gLastAddress = '';

/* Activate the tracker and hook up the onresize event
 */
function Begin ()
{
	SessionTracker.Container.TrackerCreated( SessionTracker );
	window.onresize = Resize;
}

var gResizeTimeout = 0;
function Resize ()
{
//	clearTimeout( gResizeTimeout );
//	gResizeTimeout = setTimeout( 'document.getElementById( \'messages\' ).scrollIntoView( false )', 10 );
	document.getElementById( 'messages' ).scrollIntoView( false );
}

/* Receive error messages from the tracker
 */
function onError ( Message )
{
	DrawError( 'error', Message.ErrorMessage );
	gLastAddress = '';
	Resize();
}

/* Receive message cache from the tracker
 */
function onHistory ( Messages )
{
	for ( var i = 0; i < Messages.length; ++i )
	{
		var Message = Messages[i];
		var Direction = Message.From == external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) ? 'send' : 'recv';
		var SenderName = Direction == 'send' ? external.globals( 'cfg' )( 'nick' ) : SessionTracker.Name;
		var MessageTime = Message.Time.getHours() + ':' + ( Message.Time.getMinutes() < 10 ? '0' : '' ) + Message.Time.getMinutes();

		/* Insert date
		 */
		if ( gLastDate != Message.Time.toLocaleDateString() )
		{
			DrawDate( 'history-date', Message.Time.toLocaleDateString() );
			gLastDate = Message.Time.toLocaleDateString();
			gLastTime = '';
			gLastAddress = '';
		}

		/* Show /me syntax
		 */
		if ( Message.Body.length > 4 && Message.Body.substr( 0, 4 ) == '/me ' )
		{
			if ( gLastTime == MessageTime )
				DrawMe( 'history-me', Message, SenderName, Message.Body.substr( 3 ) );
			else
				DrawMe( 'history-me', Message, SenderName, Message.Body.substr( 3 ), 'history-me-time', MessageTime );
			gLastTime = MessageTime;
			gLastAddress = '';
		}

		/* Normal chat message
		 */
		else
		{
			/* Insert sender name
			 */
			if ( gLastAddress != Message.FromAddress.ShortAddress() )
				DrawName( 'history-name-' + Direction, SenderName );
			gLastAddress = Message.FromAddress.ShortAddress();

			/* Insert message body
			 */
			if ( gLastTime == MessageTime )
				DrawMessage( 'history-body-' + Direction, Message );
			else
				DrawMessage( 'history-body-' + Direction, Message, 'history-time', MessageTime );
			gLastTime = MessageTime;
		}
	}
//	gLastDate = ( new Date() ).toLocaleDateString();
	gLastTime = '';
	gLastAddress = '';
	Resize();
}

/* Receive messages from the tracker
 */
function onMessage ( Message )
{
	var Direction = Message.FromAddress.ShortAddress() == external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) ? 'send' : 'recv';
	var SenderName = SessionTracker.Occupants ? Message.FromAddress.Resource : ( Direction == 'send' ? external.globals( 'cfg' )( 'nick' ) : SessionTracker.Name );
	var MessageTime = Message.Time.getHours() + ':' + ( Message.Time.getMinutes() < 10 ? '0' : '' ) + Message.Time.getMinutes();

	/* Insert date
	 */
	if ( gLastDate != Message.Time.toLocaleDateString() )
	{
		DrawDate( 'date', Message.Time.toLocaleDateString() );
		gLastDate = Message.Time.toLocaleDateString();
		gLastTime = '';
		gLastAddress = '';
	}

	/* Server message in a conference room
	 */
	if ( SessionTracker.Occupants && ! SenderName.length  )
	{
		DrawMUCServer( 'system', Message, Message.Body );
		gLastAddress = '';
	}

	/* Personal message in a conference room
	 */
	else if ( SessionTracker.Occupants && Message.Type != 'groupchat' )
	{
		DrawMUCPersonal( 'private-' + Direction, Message, Direction == 'recv' ? SenderName + ' says to you: ' : 'You say to ' + Message.ToAddress.Resource + ': ' );
		gLastAddress = '';
	}

	/* Show /me syntax
	 */
	else if ( Message.Body.length > 4 && Message.Body.substr( 0, 4 ) == '/me ' )
	{
		if ( gLastTime == MessageTime )
			DrawMe( 'me', Message, SenderName, Message.Body.substr( 3 ) );
		else
			DrawMe( 'me', Message, SenderName, Message.Body.substr( 3 ), 'me-time', MessageTime );
		gLastTime = MessageTime;
		gLastAddress = '';
	}

	/* Normal chat message
	 */
	else
	{
		/* Insert sender name
		 */
		if ( gLastAddress != ( SessionTracker.Occupants ? Message.FromAddress.Resource : Message.FromAddress.ShortAddress() ) )
			DrawName( 'name-' + Direction, SenderName );
		gLastAddress = SessionTracker.Occupants ? Message.FromAddress.Resource : Message.FromAddress.ShortAddress();

		/* Insert message body
		 */
		if ( gLastTime == MessageTime )
			DrawMessage( 'body-' + Direction, Message );
		else
			DrawMessage( 'body-' + Direction, Message, 'time', MessageTime );
		gLastTime = MessageTime;
	}
	Resize();
}

/* Example code to get the avatar of the message sender
var SenderAvatar = '';
if ( SessionTracker.Occupants )
{
	if ( SessionTracker.Occupants.Exists( Message.FromAddress.Resource ) )
		SenderAvatar = SessionTracker.Occupants( Message.FromAddress.Resource ).Avatar;
	else
		SenderAvatar = 'unknown-soldier.gif';
}
else
{
	if ( Direction == 'send' )
		SenderAvatar = external.globals( 'cfg' )( 'avatar' );
	else
		SenderAvatar = SessionTracker.Avatar;
}
if ( SenderAvatar.length == 40 )
	SenderAvatar = external.globals( 'usersdir' ) + 'Avatars\\' + SenderAvatar;
else
	SenderAvatar = external.globals( 'cwd' ) + '..\\images\\clients\\' + SenderAvatar;
*/

function DrawError ( StyleClass, PlainText )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	var Col = Row.insertCell(-1);
	Col.colSpan = 2;

	var Body = document.createElement( 'LI' );
	Body.className = StyleClass;
	Body.innerText = PlainText;
	Col.insertAdjacentElement( 'beforeEnd', Body );
}

function DrawDate ( StyleClass, PlainText )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	var Col = Row.insertCell(-1);
	Col.colSpan = 2;

	var Body = document.createElement( 'NOBR' );
	Body.className = StyleClass
	Body.innerText = PlainText;
	Col.insertAdjacentElement( 'beforeEnd', Body );
}

function DrawMUCServer ( StyleClass, Message, PlainText )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	var Col = Row.insertCell(-1);
	Col.colSpan = 2;

	var Body = document.createElement( 'LI' );
	Body.className = StyleClass;
	SessionTracker.Container.DOM.FilterHyperlinks( Message, Body, PlainText );
	Col.insertAdjacentElement( 'beforeEnd', Body );
}

function DrawMUCPersonal ( StyleClass, Message, PlainText )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	var Col = Row.insertCell(-1);
	Col.colSpan = 2;

	var Body = document.createElement( 'LI' );
	Body.className = StyleClass;
	Body.innerText = PlainText;
	SessionTracker.Container.DOM.FilterHyperlinks( Message, Body, Message.Body );
	Col.insertAdjacentElement( 'beforeEnd', Body );
}

function DrawMe ( StyleClass, Message, SenderName, PlainText, TimeStyleClass, Time )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	Row.vAlign = 'top';
	var Col = Row.insertCell(-1);

	var Body = document.createElement( 'LI' );
	Body.className = StyleClass;
	Body.innerText = SenderName;
	SessionTracker.Container.DOM.FilterHyperlinks( Message, Body, PlainText );
	Col.insertAdjacentElement( 'beforeEnd', Body );

	if ( Time )
	{
		Col = Row.insertCell(-1);
		Col.className = TimeStyleClass;
		Col.innerText = Time;
	}
	else
		Col.colSpan = 2;
}

function DrawName ( NameStyleClass, SenderName )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	Row.vAlign = 'bottom';
	var Col = Row.insertCell(-1);
	Col.colSpan = 2;

	var Name = document.createElement( 'NOBR' );
	Name.className = NameStyleClass;
	Name.innerText = SenderName + ' ';
	Col.insertAdjacentElement( 'beforeEnd', Name );
}

function DrawMessage ( StyleClass, Message, TimeStyleClass, Time )
{
	var Row = document.getElementById( 'messages' ).insertRow(-1);
	Row.vAlign = 'top';
	var Col = Row.insertCell(-1);
	Col.className = StyleClass;

	SessionTracker.Container.DOM.MessageToHTMLElement( Message, Col );

	if ( Time )
	{
		Col = Row.insertCell(-1);
		Col.className = TimeStyleClass;
		Col.innerText = Time;
	}
	else
		Col.colSpan = 2;
}

/* Allow user to copy, select, and clear the messages
 */
function ShowMenu ()
{
	var Menu = external.newPopupMenu;
	var HasMessages = document.getElementById( 'messages' ).rows.length;
	var HasSelection = document.selection.type == 'Text' && document.selection.createRangeCollection().item(0).text.length;
	Menu.AddItem( HasSelection,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-messages', 'copy' ),		1 );
	Menu.AddItem( HasMessages,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-messages', 'copyall' ),	2 );
	Menu.AddSeparator();
	Menu.AddItem( HasMessages,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-messages', 'clear' ),		3 );
	Menu.AddSeparator();
	Menu.AddItem( HasMessages,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'chat-messages', 'selectall' ),	4 );
	Menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );

	switch ( Menu.Choice )
	{
		case 1: Copy();			break;
		case 2: CopyAll();		break;
		case 3: Clear();		break;
		case 4: SelectAll();	break;
	}
}

/* Copy
 */
function Copy ()
{
	if ( document.selection.type == 'Text' )
	{
		var text = document.selection.createRangeCollection().item(0).text;
		if ( text.length )
			window.clipboardData.setData( 'Text', text );
	}
}

/* Copy All
 */
function CopyAll ()
{
	with ( document.selection.createRangeCollection().item(0) )
	{
		moveToElementText( document.body );
		if ( text.length )
			window.clipboardData.setData( 'Text', text );
	}
}

/* Select All
 */
function SelectAll ()
{
	with ( document.selection.createRangeCollection().item(0) )
	{
		moveToElementText( document.body );
		select();
	}
}

/* Clear
 */
function Clear ()
{
	gLastDate = ( new Date() ).toLocaleDateString();
	gLastTime = '';
	gLastAddress = '';

	if ( ! SessionTracker.Occupants )
		SessionTracker.Container.PurgeHistory( SessionTracker.Address );

	var Messages = document.getElementById( 'messages' );
	while ( Messages.rows.length )
		Messages.deleteRow(0);
}
