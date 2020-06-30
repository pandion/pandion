function Begin ()
{
	external.globals( 'Translator' ).TranslateWindow( 'groupmessage', document );
	document.onselectstart = document.ondragstart = function(){return event.srcElement.tagName=='TEXTAREA'||event.srcElement.tagName=='INPUT'};
	document.oncontextmenu = function(){return !(event.srcElement.tagName=='INPUT'&&event.srcElement.type!='text')};
	var list = document.getElementById( 'userlist' );
	var groups = ( new VBArray( external.globals( 'ClientRoster' ).Groups.Keys() ) ).toArray().sort( function(a,b){return a.toLowerCase()==b.toLowerCase()?0:a.toLowerCase()<b.toLowerCase()?-1:1} );
	for ( var i = 0; i < groups.length; ++i )
	{
		var enabled = groups[i] == external.wnd.params[1];
		var group = document.createElement( 'DIV' );
		group.className = 'list-group';
		group.insertAdjacentHTML( 'beforeEnd', '<INPUT type=checkbox onclick="toggle_group( this.checked, this.parentNode )"' + ( enabled ? ' checked' : '' ) + '> ' );
		group.insertAdjacentText( 'beforeEnd', groups[i] + '\n' );

		var jids = ( new VBArray( external.globals( 'ClientRoster' ).Groups.Item( groups[i] ).Items.Keys() ) ).toArray().sort(function (a,b) {
			var c = external.globals('ClientRoster').Items.Item(a).Name.toLowerCase();
			var d = external.globals('ClientRoster').Items.Item(b).Name.toLowerCase();
			return c == d ? 0 : c < d ? -1 : 1
		});
		for ( var j = 0; j < jids.length; ++j )
		{
			var contact = document.createElement( 'NOBR' );
			contact.className = external.globals( 'ClientRoster' ).Items.Item( jids[j] ).Resources.Count ? 'list-contact-online' : 'list-contact-offline';
			contact.insertAdjacentHTML( 'beforeEnd', '<INPUT type=checkbox onclick="toggle_contact( this.checked, this.parentNode )"' + '<INPUT type=checkbox' + ( enabled ? ' checked' : '' ) + '> ' );
			contact.insertAdjacentText( 'beforeEnd', external.globals( 'ClientRoster' ).Items.Item( jids[j] ).Name + '\n' );
			contact.title = jids[j];

			group.insertAdjacentElement( 'beforeEnd', contact );
		}

		if ( group.children.length > 2 )
			list.insertAdjacentElement( 'beforeEnd', group );
	}
	var cfg = external.globals( 'cfg' );
	with ( document.getElementById( 'txt' ).style )
	{
		color						= cfg( 'user_dialog_color' );
		fontFamily					= cfg( 'user_dialog_face' );
		fontSize					= cfg( 'user_dialog_size' );
		fontWeight					= cfg( 'user_dialog_bold' );
		fontStyle					= cfg( 'user_dialog_italic' );
		textDecorationLineThrough	= cfg( 'user_dialog_strike' ).toString() == 'true';
		textDecorationUnderline		= cfg( 'user_dialog_underline' ).toString() == 'true';
	}
	document.getElementById( 'txt' ).focus();
	external.wnd.hide( false );
}

function SendMessage ()
{
	var cfg = external.globals( 'cfg' );
	var txt = document.getElementById( 'txt' ).value;
	var url = document.getElementById( 'url' ).value;
	var contacts = document.getElementById( 'userlist' ).getElementsByTagName( 'NOBR' );

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	if ( document.getElementById( 'check_url' ).checked )
	{
		dom.loadXML( '<message><body/><x xmlns="jabber:x:oob"><url/><desc/></x></message>' );
		dom.selectSingleNode( '/message/x/url' ).text = url;
		dom.selectSingleNode( '/message/x/desc' ).text = txt;
		if ( document.getElementById( 'check_normal' ).checked )
		{
			dom.selectSingleNode( '/message/body' ).text = encodeURI( url ) + '\n' + txt;
			dom.documentElement.setAttribute( 'type', 'normal' );
		}
		else 
		{
			dom.selectSingleNode( '/message/body' ).text = encodeURI( url ) + '\n' + txt;
			dom.documentElement.setAttribute( 'type', 'headline' );
			if ( document.getElementById( 'check_attention' ).checked ) 
			{
				var attention = dom.createElement( 'attention' );
				attention.setAttribute( 'xmlns' , 'urn:xmpp:attention:0');
				dom.documentElement.appendChild( attention );
			}
		}
	}
	else
	{
		dom.loadXML( '<message><body/><html><body><span/></body></html><x xmlns="jisp:x:jep-0038"><name/></x></message>' );
		dom.documentElement.setAttribute( 'type', document.getElementById( 'check_normal' ).checked ? 'normal' : 'headline' );
		if ( document.getElementById( 'check_attention' ).checked ) 
		{
			var attention = dom.createElement( 'attention' );
			attention.setAttribute( 'xmlns' , 'urn:xmpp:attention:0');
			dom.documentElement.appendChild( attention );
		}
		dom.selectSingleNode( '/message/x[@xmlns="jisp:x:jep-0038"]/name' ).text = cfg( 'emoticonset' );
		dom.documentElement.firstChild.text = txt;
		dom.documentElement.setAttribute( 'xml:lang', external.globals( 'language' ) );

		var HTMLSpan	= dom.documentElement.selectSingleNode( 'html/body/span' );
		var TextLines	= txt.split( '\n' );
		var CSSText		= 'color: '			+ cfg( 'user_dialog_color' ) + '; '
						+ 'font-family: '	+ cfg( 'user_dialog_face' ) + '; '
						+ 'font-size: '		+ cfg( 'user_dialog_size' ) + '; '
						+ 'font-weight: '	+ cfg( 'user_dialog_bold' ) + '; '
						+ 'font-style: '	+ cfg( 'user_dialog_italic' ) + '; '
						+ 'text-decoration: '
						+ ( cfg( 'user_dialog_strike' ).toString() == 'true' ? ' line-through' : '' )
						+ ( cfg( 'user_dialog_underline' ).toString() == 'true' ? ' underline' : '' ) + ';';
		HTMLSpan.setAttribute( 'style', CSSText.toLowerCase() );
		HTMLSpan.appendChild( dom.createTextNode( TextLines[0] ) );
		for ( var i = 1; i < TextLines.length; ++i )
		{
			HTMLSpan.appendChild( dom.createElement( 'br' ) );
			HTMLSpan.appendChild( dom.createTextNode( TextLines[i] ) );
		}
		dom.documentElement.selectSingleNode( 'html' ).setAttribute( 'xmlns', 'http://jabber.org/protocol/xhtml-im' );
		dom.documentElement.selectSingleNode( 'html' ).firstChild.setAttribute( 'xmlns', 'http://www.w3.org/1999/xhtml' );
	}

	for ( var i = 0; i < contacts.length; i++ )
		if ( contacts[i].children(0).checked )
		{
			dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
			dom.documentElement.setAttribute( 'to', contacts[i].title );
			if ( external.globals( 'debug' ) )
				external.wnd.params[0].warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
			var Message = new XMPPMessage();
			Message.FromDOM( dom );
			external.wnd.params[0].history_add( contacts[i].title, Message.Stamp, Message.Body , false );
		}
	external.wnd.close();
}

function toggle_group ( enabled, group )
{
	var contacts = group.getElementsByTagName( 'NOBR' );
	for ( var i = 0; i < contacts.length; ++i )
		contacts[i].children(0).checked = enabled;
}

function toggle_contact ( enabled, contact )
{
	if ( ! enabled )
		contact.parentNode.children(0).checked = false;
}
