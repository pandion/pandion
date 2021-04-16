function Headline ()
{
	this.TTL						= 8;
	this.Icon					= '';
	this.Title					= '';
	this.XMLDOM					= null;
	this.Address				= new XMPPAddress( external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '/' + external.globals( 'cfg' ).Item( 'resource' ) );
	this.Archive				= true;
	this.Message				= '';
	this.ShowOptions			= false;
	this.ShowPreferences		= false;
	this.PreferenceSection 	= '';
	this.OpenArchive			= false;
	this.OpenHyperlink		= '';
	this.OpenConversation	= null;
	this.Options				= '';
	this.CloseOnOptions		= true;
	this.Show					= Show;
	this.AutoClose				= true;
	this.HtmlFile				= 'toaster.html';
		
	function Show ()
	{
		var toasterheigth = 120;
		var toasterwidth	= 200;
		var count			=  Math.floor ( screen.availHeight / toasterheigth );
				
		if ( this.Archive )
		{
			if ( external.globals( 'headlines' ).length == 50 )
				external.globals( 'headlines' ).shift();
			external.globals( 'headlines' ).push( this );
			if ( external.windows.Exists( 'headline_log' ) )
				external.windows( 'headline_log' ).Do( 'alerts_refresh', 0 );
		}

		var x = 0;
		var y = 0;
		var z = 0;
		
		for ( var i = 1; i <= (count * 4) && external.windows.Exists( 'headline' + i ); ++i );
		if ( i <= (count * 4) ) 
		{
			if ( external.globals( 'CountToaster' ) < i )
				external.globals( 'CountToaster' ) = i;
			
			z = Math.floor ( (i * toasterheigth) / (count * toasterheigth + 1) );
			x = screen.availWidth - ((18 + toasterwidth) + (toasterwidth * z));
			y = screen.availHeight - ( (i - (z * count)) * toasterheigth );
			
			with ( external.createWindow( 'headline' + i, external.globals( 'cwd' ) + this.HtmlFile, [ window, this ] ) )
			{
				showTitlebar( false );
				MinHeight = MinWidth = 0;
				setSize( toasterwidth, toasterheigth );
				setPos( x, y );
				setAOT( true );
			}
		}
	}
}
