/* Chops up and filters XMPP addresses
 */
function XMPPAddress ( address )
{
	this.Host			= '';
	this.User			= '';
	this.Resource		= '';

	this.Parse			= Parse;
	this.LongAddress	= LongAddress;
	this.CleanAddress	= CleanAddress;
	this.ShortAddress	= ShortAddress;

	if ( address )
		this.Parse( address );

	function Parse ( address )
	{
		this.Host			= '';
		this.User			= '';
		this.Resource		= '';

		/* Filter the address for easier digestion
		 */
		if ( address.indexOf( '/' ) != -1 )
		{
			this.Resource = address.substr( address.indexOf( '/' ) + 1 );
			address = address.substr( 0, address.indexOf( '/' ) );
		}
		if ( address.indexOf( '@' ) != -1 )
		{
			this.User = address.substr( 0, address.indexOf( '@' ) ).toLowerCase();
			this.Host = address.substr( address.indexOf( '@' ) + 1 ).toLowerCase();
		}
		else
			this.Host = address.toLowerCase();
	}

	/* Returns the address without the resource
	 */
	function ShortAddress ()
	{
		if ( this.User.length )
			return this.User + '@' + this.Host;
		else
			return this.Host;
	}

	/* Returns the address including the resource
	 */
	function LongAddress ()
	{
		if ( this.Resource.length )
			return this.ShortAddress() + '/' + this.Resource;
		else
			return this.ShortAddress();
	}

	/* Returns the short address without the transport
	 */
	function CleanAddress ()
	{
		if ( external.globals( 'ClientServices' ).Services.Exists( this.Host ) )
			return this.User.length ? this.User.replace( /%/, '@' ) : this.Host;
		else
			return this.ShortAddress();
	}
}
