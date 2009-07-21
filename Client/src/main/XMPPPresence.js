/* This is a wrapper for the <presence/> stanza
 */
function XMPPPresence ()
{
	this.From			= '';
	this.Show			= '';
	this.Type			= 'available';
	this.Status			= '';
	this.XMLDOM			= null;
	this.Priority		= 0;
	this.FromAddress	= new XMPPAddress();

	this.FromDOM		= FromDOM;

	/* Parses the DOM we get from the XMPPOnPresence event
	 */
	function FromDOM ( dom )
	{
		var node;

		this.XMLDOM = dom;

		if ( node = dom.documentElement.getAttribute( 'from' ) )
		{
			this.From = node;
			this.FromAddress.Parse( node );
		}

		if ( node = dom.documentElement.getAttribute( 'type' ) )
			this.Type = node;

		if ( node = dom.selectSingleNode( '/presence/show' ) )
			this.Show = node.text;

		if ( node = dom.selectSingleNode( '/presence/status' ) )
			this.Status = node.text;

		if ( node = dom.selectSingleNode( '/presence/priority' ) )
		{
			var node = parseInt( node.text, 10 );
			if ( isNaN( node ) )
			{
				if ( this.Type == 'invisible' )
					this.Priority = 8;
				else if ( this.Type != 'unavailable' )
					switch ( this.Show )
					{
						case 'chat':	this.Priority = 10; break;
						case 'away':	this.Priority =  4; break;
						case 'xa':		this.Priority =  2; break;
						case 'dnd':		this.Priority =  6; break;
						default:		this.Priority =  8;
					}
			}
			else
				this.Priority = node;
		}
	}
}
