function Headline ()
{
	this.TTL				= 8;
	this.Icon				= '';
	this.Title				= '';
	this.XMLDOM				= null;
	this.Address			= new XMPPAddress( external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
	this.Archive			= true;
	this.Message			= '';
	this.ShowOptions		= false;
	this.OpenArchive		= false;
	this.OpenHyperlink		= '';
	this.OpenConversation	= null;

	this.Show				= Show;

	function Show ()
	{
		if ( this.Archive )
		{
			if ( external.globals( 'headlines' ).length == 50 )
				external.globals( 'headlines' ).shift();
			external.globals( 'headlines' ).push( this );
			if ( external.windows.Exists( 'headline_log' ) )
				external.windows( 'headline_log' ).Do( 'alerts_refresh', 0 );
		}

		for ( var i = 1; i <= 8 && external.windows.Exists( 'headline' + i ); ++i );
		if ( i <= 8 )
			with ( external.createWindow( 'headline' + i, external.globals( 'cwd' ) + 'toaster.html', [ window, this ] ) )
			{
				showTitlebar( false );
				MinHeight = MinWidth = 0;
				setAOT( true );
				setSize( 200, 120 );
				setPos( screen.availWidth - 218, screen.availHeight - i * 120 );
			}
	}
}
