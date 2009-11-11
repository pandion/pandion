/* List of tag names and tag attributes that are defined in JEP 71 (XHTML-IM)
 */
var AllowedTags = new ActiveXObject( 'Scripting.Dictionary' );

/* Text Module
 */
AllowedTags.Add( 'blockquote', null );
AllowedTags.Add( 'br', null );
AllowedTags.Add( 'cite', null );
AllowedTags.Add( 'code', null );
AllowedTags.Add( 'div', null );
AllowedTags.Add( 'em', null );
AllowedTags.Add( 'h1', null );
AllowedTags.Add( 'h2', null );
AllowedTags.Add( 'h3', null );
AllowedTags.Add( 'p', null );
AllowedTags.Add( 'pre', null );
AllowedTags.Add( 'q', null );
AllowedTags.Add( 'span', null );
AllowedTags.Add( 'strong', null );

/* Hypertext Module
 */
AllowedTags.Add( 'a', new ActiveXObject( 'Scripting.Dictionary' ) );

/* List Module
 */
AllowedTags.Add( 'ol', null );
AllowedTags.Add( 'ul', null );
AllowedTags.Add( 'li', null );

/* Image Module
 */
AllowedTags.Add( 'img', new ActiveXObject( 'Scripting.Dictionary' ) );

/* Allowed attributes for A
 */
with ( AllowedTags( 'a' ) )
{
	Add( 'href', null );
}

/* Allowed attributes for IMG
 */
with ( AllowedTags( 'img' ) )
{
	Add( 'align', null );
	Add( 'alt', null );
	Add( 'height', null );
	Add( 'src', null );
	Add( 'width', null );
}

/* Compatibility
 */
AllowedTags.Add( 'b', null );
AllowedTags.Add( 'i', null );
AllowedTags.Add( 'u', null );
AllowedTags.Add( 'font', new ActiveXObject( 'Scripting.Dictionary' ) );

with ( AllowedTags( 'font' ) )
{
	Add( 'face', null );
	Add( 'color', null );
	Add( 'size', null );
}

/* CSS blacklist
 */
var BlackStyles = new Array(
	'behavior',
	'behaviour',
	'content',
	'expression'
);

/* Protocol blacklist
 */
var BlackProtocols = new Array(
	'about',
	'data',
	'help',
	'javascript',
	'ms-help',
	'ms-its',
	'mhtml',
	'res',
	'resource',
	'shell',
	'vbscript',
	'view-source',
	'vnd.ms.radio',
	'wysiwyg'
);

/* Protocol whitelist
 */
var WhiteProtocols = new Array(
	'http',
	'https',
	'ftp',
	'telnet',
	'news',
	'nntp',
	'gopher',
	'mailto',
	'xmpp',
	'ed2k',
	'irc'
);

/* Build regular expression from CSS blacklist
 */
var BlackStylesRegEx = new RegExp();
BlackStylesRegEx.compile( '(' + BlackStyles.join( '|' ) + ')', 'i' );

/* Build regular expression from protocols blacklist
 */
for ( var i = 0; i < BlackProtocols.length; ++i )
	BlackProtocols[i] = '[\\s\\x01-\\x1F]*' + BlackProtocols[i].replace( /(.)/g, '$1[\\s\\x01-\\x1F]*' );
var BlackProtocolsRegEx = new RegExp();
BlackProtocolsRegEx.compile( '(' + BlackProtocols.join( '|' ) + '):', 'i' );

/* Build regular expression from protocols whitelist
 */
for ( var i = 0; i < WhiteProtocols.length; ++i )
	WhiteProtocols[i] = '[\\s\\x01-\\x1F]*' + WhiteProtocols[i].replace( /(.)/g, '$1[\\s\\x01-\\x1F]*' );
var WhiteProtocolsRegEx = new RegExp();
WhiteProtocolsRegEx.compile( '^(' + WhiteProtocols.join( '|' ) + '):', 'i' );

/* Use this function to display a message as HTML
 */
function MessageToHTMLElement ( Message, HTMLElement )
{
	if ( external.globals( 'ClientServices' ).Services.Exists( Message.ToAddress.Host ) && ( external.globals( 'ClientServices' ).Services( Message.ToAddress.Host ).Options & 0x0001 ) )
		switch ( external.globals( 'ClientServices' ).Services( Message.ToAddress.Host ).Options & 0x881E )
		{
			case 0x0002: Message.EmoticonSet = 'msn_messenger'; break;
			case 0x0004: Message.EmoticonSet = 'icq'; break;
			case 0x0008: Message.EmoticonSet = 'aim'; break;
			case 0x0010: Message.EmoticonSet = 'yahoo_messenger'; break;
			case 0x0800: Message.EmoticonSet = 'gadu-gadu'; break;
			case 0x8000: Message.EmoticonSet = 'tlen.pl'; break;
		}
	else if ( Message.ToAddress.Host == 'jabber.wp.pl' )
		Message.EmoticonSet = 'wpkontakt';

	if ( external.globals( 'ClientServices' ).Services.Exists( Message.FromAddress.Host ) && ( external.globals( 'ClientServices' ).Services( Message.FromAddress.Host ).Options & 0x0001 ) )
		switch ( external.globals( 'ClientServices' ).Services( Message.FromAddress.Host ).Options & 0x881E )
		{
			case 0x0002: Message.EmoticonSet = 'msn_messenger'; break;
			case 0x0004: Message.EmoticonSet = 'icq'; break;
			case 0x0008: Message.EmoticonSet = 'aim'; break;
			case 0x0010: Message.EmoticonSet = 'yahoo_messenger'; break;
			case 0x0800: Message.EmoticonSet = 'gadu-gadu'; break;
			case 0x8000: Message.EmoticonSet = 'tlen.pl'; break;
		}
	else if ( Message.FromAddress.Host == 'jabber.wp.pl' )
		Message.EmoticonSet = 'wpkontakt';

	if ( ! external.FileExists( external.globals( 'usersdir' ) + 'Emoticons\\' + Message.EmoticonSet + '\\icondef.xml' ) )
	{
		if ( Message.EmoticonSet.length )
			external.globals( 'EmoticonCache' ).InstallStyle( Message.EmoticonSet );
		Message.EmoticonSet = external.globals( 'cfg' )( 'emoticonset' );
	}

	if ( Message.XHTMLBody )
		FilterNode( Message, HTMLElement, Message.XHTMLBody );
	else
	{
		var TextLines = Message.Body.split( '\n' );
		FilterHyperlinks( Message, HTMLElement, TextLines[0] );
		for ( var i = 1; i < TextLines.length; ++i )
		{
			HTMLElement.insertAdjacentText( 'beforeEnd', '\n' );
			FilterHyperlinks( Message, HTMLElement, TextLines[i] );
		}
	}
}

/* Allow only XHTML-IM approved tags and attributes
 */
function FilterNode ( Message, HTMLElement, XMLTag )
{
	var NodeHTMLElement = HTMLElement;
	var TagName = XMLTag.tagName.toLowerCase();
	if ( TagName == 'body' )
		TagName = 'span';
	if ( AllowedTags.Exists( TagName ) )
	{
		NodeHTMLElement = document.createElement( TagName );

		/* Check attributes
		 */
		var Attributes = XMLTag.attributes;
		for ( var i = 0; i < Attributes.length; ++i )
		{
			var AttributeName = Attributes( i ).name.toLowerCase();
			/* Security check on CSS
			 */
			if ( AttributeName == 'style' )
			{
				var Style = Attributes( i ).value;
				Style.replace( '\\', '' );
				Style.replace( '&amp;', '&' );
				Style.replace( '&', '&amp;' );
				if ( ! BlackProtocolsRegEx.test( Style ) && ! BlackStylesRegEx.test( Style ) )
					NodeHTMLElement.style.cssText = Style;
			}
			/* Security check on protocol
			 */
			else if ( ( AttributeName == 'src' || AttributeName == 'href' ) && ! WhiteProtocolsRegEx.test( Attributes( i ).value ) )
			{
				continue;
			}
			/* Copy attribute if whitelisted
			 */
			else if ( AllowedTags( TagName ) != null && AllowedTags( TagName ).Exists( AttributeName ) )
				NodeHTMLElement.setAttribute( AttributeName, Attributes( i ).value, 0 );
		}

		/* Override hyperlink handling
		 */
		if ( TagName == 'a' )
		{
			NodeHTMLElement.attachEvent( 'oncontextmenu', HyperlinkMenu );
			NodeHTMLElement.attachEvent( 'onclick', HyperlinkClick );
			if ( ! NodeHTMLElement.title.length )
				NodeHTMLElement.title = NodeHTMLElement.href;
		}
	}

	/* Recursive parsing on the elements and text
	 */
	var ChildNodes = XMLTag.childNodes;
	for ( var i = 0; i < ChildNodes.length; ++i )
		switch ( ChildNodes.item( i ).nodeType )
		{
			/* Element
			 */
			case 1:
				FilterNode( Message, NodeHTMLElement, ChildNodes.item( i ) );
			break;

			/* Text
			 */
			case 3:
				if ( TagName == 'a' )
					FilterNothing( NodeHTMLElement, ChildNodes.item( i ).data );
				else
					FilterHyperlinks( Message, NodeHTMLElement, ChildNodes.item( i ).data );
			break;
		}

	HTMLElement.insertAdjacentElement( 'beforeEnd', NodeHTMLElement );
}

/* Highlight hyperlinks in the text
 */
function FilterHyperlinks ( Message, HTMLElement, MessageText )
{
	var PathExpression = /(?:([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})|(?:(?:(news|telnet|nttp|file|http|ftp|https|irc|callto):\/\/)|(www|ftp)[-A-Za-z0-9]*\.)[-A-Za-z0-9\.]+)(?::[0-9]*)?(?:\/[-A-Za-z0-9_\$\.\+\!\*\(\),;:@&=\?\/~\#\%]*[^]'\.}>\),\\"])?/m; // 1. IP address, 2. protocol, 3. www or ftp
	var AddressExpression = /(?:(xmpp|mailto):)?[a-z0-9!#$%&'*+\/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+\/=?^_`{|}~-]+)*(@)(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?(?:(\?)[-A-Za-z0-9_\$\.\+\!\*\(\),;:@&=\?\/~\#\%]*[^]'\.}>\),\\"])?/m; // 1. protocol, 2. '@' symbol, 3. '?' symbol
	var Result = null;
	while ( Result = ( PathExpression.exec( MessageText ) || AddressExpression.exec( MessageText ) ) )
	{
		FilterEmoticons( Message, HTMLElement, MessageText.substr( 0, Result.index ) );
		MessageText = MessageText.substr( Result.lastIndex );

		var Hyperlink = document.createElement( 'A' );
		Hyperlink.href = Result[0];
		if ( Result[2] == '@' )
		{
			if ( Result[1].length == 0 )
				Hyperlink.href = 'xmpp:' + Result[0];
		}
		else if ( Result[1].length > 0 || Result[3] == 'www' )
			Hyperlink.href = 'http://' + Result[0];
		else if ( Result[3] == 'ftp' )
			Hyperlink.href = 'ftp://' + Result[0];
		Hyperlink.innerText = Result[0];
		Hyperlink.attachEvent( 'oncontextmenu', HyperlinkMenu );
		Hyperlink.attachEvent( 'onclick', HyperlinkClick );
		HTMLElement.insertAdjacentElement( 'beforeEnd', Hyperlink );
	}
	FilterEmoticons( Message, HTMLElement, MessageText );
}

/* Replace emoticon text with emoticon images, soundclips, or text markup
 */
function FilterEmoticons ( Message, HTMLElement, MessageText )
{
	var Languages = new Array( 'default' );
	if ( Message.Language )
		Languages.push( Message.Language );

	var Subset = external.globals( 'EmoticonCache' ).GetSubset( Message.EmoticonSet, Languages );

	if ( Subset && external.globals( 'cfg' )( 'emoticon' ).toString() == 'true' )
	{
		var Result = null;
		while ( Result = Subset.Expression.exec( MessageText ) )
		{
			FilterMarkup( HTMLElement, MessageText.substr( 0, Result.index ) );
			MessageText = MessageText.substr( Result.lastIndex );

			var Action = Subset.Style.Actions[ Subset.StringToAction( Result[0] ) ];

			if ( Action.Graphic && Action.Sound )
			{
				var Graphic = document.createElement( 'IMG' );
				Graphic.align = 'absmiddle';
				Graphic.alt = Result[0];
				Graphic.border = 0;
				Graphic.src = external.globals( 'usersdir' ) + 'Emoticons\\' + Message.EmoticonSet + '\\' + Action.Graphic;
				Graphic.style.cursor = 'hand';
				Graphic.SoundFile = external.globals( 'usersdir' ) + 'Emoticons\\' + Message.EmoticonSet + '\\' + Action.Sound;
				Graphic.attachEvent(
					'onclick',
					function ( event )
					{
						external.wnd.params[0].sound_play( event.srcElement.SoundFile, true );
						event.cancelBubble = true;
					}
				);
				HTMLElement.insertAdjacentElement( 'beforeEnd', Graphic );
				Graphic.click();
			}
			else if ( Action.Graphic )
			{
				var Graphic = document.createElement( 'IMG' );
				Graphic.align = 'absmiddle';
				Graphic.alt = Result[0];
				Graphic.border = 0;
				Graphic.src = external.globals( 'usersdir' ) + 'Emoticons\\' + Message.EmoticonSet + '\\' + Action.Graphic;
				Graphic.attachEvent(
					'onclick',
					function ( event )
					{
						event.srcElement.style.display = 'none';
						event.srcElement.nextSibling.style.display = 'inline';
						event.cancelBubble = true;
					}
				);
				HTMLElement.insertAdjacentElement( 'beforeEnd', Graphic );

				var Alternative = document.createElement( 'SPAN' );
				Alternative.style.display = 'none';
				Alternative.innerText = Result[0];
				Alternative.attachEvent(
					'onclick',
					function ( event )
					{
						event.srcElement.style.display = 'none';
						event.srcElement.previousSibling.style.display = 'inline';
						event.cancelBubble = true;
					}
				);
				HTMLElement.insertAdjacentElement( 'beforeEnd', Alternative );
			}
			else if ( Action.Sound )
			{
				var Label = document.createElement( 'LABEL' );
				Label.className = 'sound-label';
				Label.innerText = Result[0];
				HTMLElement.insertAdjacentElement( 'beforeEnd', Label );

				var PlayButton = document.createElement( 'BUTTON' );
				PlayButton.className = 'sound-button';
				PlayButton.SoundFile = external.globals( 'usersdir' ) + 'Emoticons\\' + Message.EmoticonSet + '\\' + Action.Sound;
				PlayButton.attachEvent(
					'onclick',
					function ( event )
					{
						external.wnd.params[0].sound_play( event.srcElement.SoundFile, true );
						event.cancelBubble = true;
					}
				);

				var PlayIcon = document.createElement( 'IMG' );
				PlayIcon.align = 'absmiddle';
				PlayIcon.border = 0;
				PlayIcon.src = external.globals( 'cwd' ) + '..\\images\\misc\\play.gif';
				PlayButton.insertAdjacentElement( 'beforeEnd', PlayIcon );

				HTMLElement.insertAdjacentElement( 'beforeEnd', PlayButton );

				PlayButton.click();
			}
			else if ( Action.Format )
			{
				var Format = document.createElement( 'SPAN' );
				Format.style.cssText = Action.Format;
				Format.innerText = Result[0];
				HTMLElement.insertAdjacentElement( 'beforeEnd', Format );
			}
		}
		FilterMarkup( HTMLElement, MessageText );
	}
	else
		FilterMarkup( HTMLElement, MessageText );
}

/* Apply basic text markup: *bold* _underline_ /italic/ -strikethrough-
 */
function FilterMarkup ( HTMLElement, MessageText )
{
	var Expression = /[\/\*\_\-][^\s]{2,}/m;
	var Result = null;
	while ( Result = Expression.exec( MessageText ) )
		if ( Result[0].charAt( 0 ) == Result[0].charAt( Result[0].length - 1 ) )
		{
			FilterNothing( HTMLElement, MessageText.substr( 0, Result.index ) );
			MessageText = MessageText.substr( Result.lastIndex );

			var Markup = document.createElement( 'SPAN' );
			Markup.innerText = Result[0].substr( 1, Result[0].length - 2 );
			switch ( Result[0].charAt( 0 ) )
			{
				case '/': Markup.style.fontStyle = 'italic'; break;
				case '*': Markup.style.fontWeight = 'bold'; break;
				case '_': Markup.style.textDecorationUnderline = true; break;
				case '-': Markup.style.textDecorationLineThrough = true; break;
			}
			HTMLElement.insertAdjacentElement( 'beforeEnd', Markup );
		}
		else
		{
			FilterNothing( HTMLElement, MessageText.substr( 0, Result.lastIndex ) );
			MessageText = MessageText.substr( Result.lastIndex );
		}
	FilterNothing( HTMLElement, MessageText );
}

/* Output as text
 */
function FilterNothing ( HTMLElement, MessageText )
{
	if ( HTMLElement.tagName == 'BR' )
		HTMLElement.insertAdjacentText( 'afterEnd', MessageText.replace( '\n', '' ) );
	else
		HTMLElement.insertAdjacentText( 'beforeEnd', MessageText.replace( '\n', '' ) );
}

/* Open hyperlink with default application
 */
function HyperlinkClick ( event )
{
	event.returnValue = false;
	event.cancelBubble = true;
	if ( document.getElementById( 'send-text' ) && ! document.getElementById( 'send-text' ).disabled )
		document.getElementById( 'send-text' ).focus();
	external.windows( 'MainWindow' ).Do( 'dial_webbrowser', event.srcElement.href );
}

/* Context menu menu for hyperlinks
 */
function HyperlinkMenu ( event )
{
	event.returnValue = false;
	event.cancelBubble = true;

	var Menu = external.newPopupMenu;
	Menu.AddItem( true, false, false, true,  0, external.globals( 'Translator' ).Translate( 'main', 'cl_link_open' ), 1 );
	Menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_link_copy' ), 2 );
	Menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );

	if ( document.getElementById( 'send-text' ) && ! document.getElementById( 'send-text' ).disabled )
		document.getElementById( 'send-text' ).focus();

	switch ( Menu.Choice )
	{
		case 1:
			external.windows( 'MainWindow' ).Do( 'dial_webbrowser', event.srcElement.href );
		break;
		case 2:
			window.clipboardData.setData( 'Text', event.srcElement.href );
		break;
	}
}
