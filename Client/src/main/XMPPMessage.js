/* This is a wrapper for the <message/> stanza
 */
function XMPPMessage ()
{
	this.FromDOM				= FromDOM;
	this.Body					= '';
	this.XHTMLBody				= null;
	this.ChatState				= '';
	this.ConferenceInvitation	= '';
	this.EmoticonSet			= '';
	this.ErrorCode				= 0;
	this.ErrorMessage			= '';
	this.From					= '';
	this.FromAddress			= new XMPPAddress();
	this.Id						= '';
	this.Language				= '';
	this.MUCInvitationFrom		= '';
	this.MUCInvitationPassword	= '';
	this.Stamp					= '';
	this.Subject				= '';
	this.Thread					= '';
	this.Time					= new Date();
	this.To						= '';
	this.ToAddress				= new XMPPAddress();
	this.Type					= 'normal';
	this.Attention				= '';
	this.OOB					= [];
	this.EventId				= '';
	this.WantsComposing			= false;
	this.WantsDelivered			= false;
	this.WantsDisplayed			= false;
	this.IsComposing			= false;
	this.IsDelivered			= false;
	this.IsDisplayed			= false;
	this.DropComposing			= false;

	/* Parses the DOM we get from the XMPPOnMessage event
	 */
	function FromDOM ( dom )
	{
		var node;

		if ( node = dom.documentElement.getAttribute( 'xml:lang' ) )
		{
			this.Language = node;
			if ( ! this.Language )
				this.Language = 'default';
			else if ( ! ( this.Language.length < 3 || this.Language.indexOf( 'i-' ) == 0 || this.Language.indexOf( 'x-' ) == 0 ) )
				this.Language = this.Language.substr( 0, this.Language.indexOf( '-' ) );
			this.Language = this.Language.toLowerCase();
		}

		if ( node = dom.documentElement.getAttribute( 'id' ) )
			this.Id = node;

		if ( node = dom.documentElement.getAttribute( 'from' ) )
		{
			this.From = node;
			this.FromAddress.Parse( node );
		}

		if ( node = dom.documentElement.getAttribute( 'to' ) )
		{
			this.To = node;
			this.ToAddress.Parse( node );
		}

		if ( node = dom.documentElement.getAttribute( 'type' ) )
			this.Type = node;
		/** JEP 224 -  Attention
		*/
		if ( node = dom.selectSingleNode( '/message/attention' ) )
			this.Attention = node.getAttribute( 'xmlns' );

		if ( node = dom.selectSingleNode( '/message/thread' ) )
			this.Thread = node.text;

		if ( node = dom.selectSingleNode( '/message/subject[@xml:lang = "' + external.globals( 'language' ) + '"]' ) )
			this.Subject = node.text;
		else if ( node = dom.selectSingleNode( '/message/subject[not( @xml:lang )]' ) )
			this.Subject = node.text;
		else if ( node = dom.selectSingleNode( '/message/subject' ) )
			this.Subject = node.text;

		if ( node = dom.selectSingleNode( '/message/body[@xml:lang = "' + external.globals( 'language' ) + '"]' ) )
			this.Body = node.text;
		else if ( node = dom.selectSingleNode( '/message/body[not( @xml:lang )]' ) )
			this.Body = node.text;
		else if ( node = dom.selectSingleNode( '/message/body' ) )
			this.Body = node.text;

		if ( node = dom.selectSingleNode( '/message/html/body[@xml:lang = "' + external.globals( 'language' ) + '"]' ) )
			this.XHTMLBody = node;
		else if ( node = dom.selectSingleNode( '/message/html/body[not( @xml:lang )]' ) )
			this.XHTMLBody = node;
		else if ( node = dom.selectSingleNode( '/message/html/body' ) )
			this.XHTMLBody = node;

		/* Get the error information
		 */
		if ( node = dom.selectSingleNode( '/message[@type = "error"]/error[@code]' ) )
		{
			this.ErrorCode = node.getAttribute( 'code' );
			this.ErrorMessage = node.text;
		}

		/* JEP 91 - Delayed Delivery
		 */
		with ( this.Time )
		{
			var Y = getFullYear().toString();
			var M = (getMonth() + 1).toString();
			var D = getDate().toString();
			var h = getHours().toString();
			var m = getMinutes().toString();
			var s = getSeconds().toString();
			this.Stamp = Y + ( M.length > 1 ? '' : '0' ) + M + ( D.length > 1 ? '' : '0' ) + D + 'T' + ( h.length > 1 ? '' : '0' ) + h + ( m.length > 1 ? ':' : ':0' ) + m + ( s.length > 1 ? ':' : ':0' ) + s;
		}
		if ( node = dom.selectSingleNode( '/message/x[@xmlns = "jabber:x:delay" and @stamp]' ) )
		{
			var t = node.getAttribute( 'stamp' );
			if ( ( /^\d{8}T\d{2}:\d{2}:\d{2}/ ).test( t ) )
				with ( this.Time )
				{
					setUTCFullYear( t.substr( 0, 4 ) );
					setUTCMonth( t.substr( 4, 2 ) - 1 );
					setUTCDate( t.substr( 6, 2 ) );
					setUTCHours( t.substr( 9, 2 ) );
					setUTCMinutes( t.substr( 12, 2 ) );
					setUTCSeconds( t.substr( 15, 2 ) );
					var Y = getFullYear().toString();
					var M = (getMonth() + 1).toString();
					var D = getDate().toString();
					var h = getHours().toString();
					var m = getMinutes().toString();
					var s = getSeconds().toString();
					this.Stamp = Y + ( M.length > 1 ? '' : '0' ) + M + ( D.length > 1 ? '' : '0' ) + D + 'T' + ( h.length > 1 ? '' : '0' ) + h + ( m.length > 1 ? ':' : ':0' ) + m + ( s.length > 1 ? ':' : ':0' ) + s;
				}
		}

		/* JEP 38 - Icon Styles (Extension)
		 */
		if ( node = dom.selectSingleNode( '/message/x[@xmlns = "jisp:x:jep-0038"]/name' ) )
			this.EmoticonSet = node.text.replace( /\\\//, '' );

		/* No events or URLs in error messages
		 */
		if ( this.Type == 'error' )
			return;

		/* Conference invitation (undocumented)
		 */
		if ( node = dom.selectSingleNode( '/message/x[@xmlns = "jabber:x:conference" and @jid]' ) )
			this.ConferenceInvitation = node.getAttribute( 'jid' );

		/* JEP 45 - Multi-User Chat (Invitation)
		 */
		if ( node = dom.selectSingleNode( '/message/x[@xmlns = "http://jabber.org/protocol/muc#user"]/invite[@from]' ) )
		{
			this.MUCInvitationFrom = node.getAttribute( 'from' );
			if ( node = dom.selectSingleNode( '/message/x[@xmlns = "http://jabber.org/protocol/muc#user"]/password' ) )
				this.MUCInvitationPassword = node.text;
		}

		/* JEP 85 - Chat State Notifications
		 */
		if ( node = dom.selectSingleNode( '/message/*[@xmlns = "http://jabber.org/protocol/chatstates"]' ) )
			this.ChatState = node.tagName;

		/* JEP 22 - Message Events
		 */
		if ( node = dom.selectSingleNode( '/message/x[@xmlns = "jabber:x:event"]' ) )
		{
			if ( node.selectSingleNode( 'id' ) && ! this.Body.length )
			// Event is raised
			{
				this.EventId = node.selectSingleNode( 'id' ).text;
				if ( node.childNodes.length == 1 )
					this.DropComposing = true;
				if ( node.selectSingleNode( 'composing' ) )
					this.IsComposing = true;
				if ( node.selectSingleNode( 'displayed' ) )
					this.IsDisplayed = true;
				if ( node.selectSingleNode( 'delivered' ) )
					this.IsDelivered = true;
			}
			else
			// Event is requested
			{
				if ( node.selectSingleNode( 'composing' ) )
					this.WantsComposing = true;
				if ( node.selectSingleNode( 'displayed' ) )
					this.WantsDisplayed = true;
				if ( node.selectSingleNode( 'delivered' ) )
					this.WantsDelivered = true;
			}
		}

		/* JEP 66 - Out of Band Data
		 */
		with ( dom.selectNodes( '/message/x[@xmlns = "jabber:x:oob" and url]' ) )
			for ( var i = 0; i < length; ++i )
				with ( item( i ) )
				{
					this.OOB.push( selectSingleNode( 'url' ).text );
					this.OOB.push( selectSingleNode( 'desc' ) ? selectSingleNode( 'desc' ).text : '' );
				}
	}
}
