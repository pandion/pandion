/* This is a wrapper for the <iq/> stanza
 */
function XMPPIQ ()
{
	this.Id = '';
	this.To = '';
	this.From = '';
	this.Type = 'get';
	this.XMLDOM = null;
	this.Namespace = '';
	this.ToAddress = new XMPPAddress();
	this.FromAddress = new XMPPAddress();

	this.FromDOM = FromDOM;

	/* Parses the DOM we get from the XMPPOnIQ event
	 */
	function FromDOM ( dom )
	{
		var node;

		this.XMLDOM = dom;

		if ( node = dom.documentElement.getAttribute( 'to' ) )
		{
			this.To = node;
			this.ToAddress.Parse( node );
		}

		if ( node = dom.documentElement.getAttribute( 'from' ) )
		{
			this.From = node;
			this.FromAddress.Parse( node );
		}

		if ( node = dom.documentElement.getAttribute( 'id' ) )
			this.Id = node;

		if ( node = dom.documentElement.getAttribute( 'type' ) )
			this.Type = node;

		if ( node = dom.selectSingleNode( '/iq/*[@xmlns]' ) )
			this.Namespace = node.getAttribute( 'xmlns' );
	}
}
