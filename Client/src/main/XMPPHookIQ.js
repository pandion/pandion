/* Used to set parameters for IQ callbacks
 */
function XMPPHookIQ ( id )
{
	this.TTL		= 1;		// how many replies are we waiting for: when TTL hits 0 the hook is destroyed, when TTL is less than 0 the hook is never stopped
	this.From		= false;	// from address on the response must match this string
	this.Window		= null;		// Window object that contains the Callback function
	this.Callback	= '';		// name of the function to call (must take the XMPPIQ object as single parameter)
	this.Type		= 0;		/* 1 - get
								 * 2 - set
								 * 4 - result
								 * 8 - error
								 */

	if ( id )
		this.Id = id;
	else
		this.Id = 'sd' + ( ++external.globals( 'uniqueid' ) );
	external.globals( 'pending' ).Add( this.Id, this );

	this.Destroy = Destroy;
	function Destroy ()
	{
		if ( external.globals( 'pending' ).Exists( this.Id ) )
			external.globals( 'pending' ).Remove( this.Id );

		this.Window			= null;
		this.Callback		= '';
		this.TTL			= 0;
		this.From			= false;
		this.Namespace		= '';
		this.CheckValues	= 0;
	}
}
