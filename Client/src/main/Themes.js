/**
	Load theme
*/
function InitThemes ( )
{
	var theme = external.globals( 'theme' );

	switch ( theme )
	{
	case 'default':
		external.globals( 'ColorRosterGroupHeader' ) = '#0570AD';
		external.globals( 'ColorModeName') = '#312505';
		addCss ( 'theme', '' );
		break;
	case 'blue': 
		external.globals( 'ColorRosterGroupHeader' ) = '#0570AD';
		external.globals( 'ColorModeName') = '#312505';
		addCss ( 'theme', '../css/' + theme + '/main.css' );
		break;
	case 'green':
		external.globals( 'ColorRosterGroupHeader' ) = '#225500';
		external.globals( 'ColorModeName') = '#225500';
		addCss ( 'theme', '../css/' + theme + '/main.css' );  
		break;
	case 'grey':
		external.globals( 'ColorRosterGroupHeader' ) = '#252525';
		external.globals( 'ColorModeName') = '#252525';
		addCss ( 'theme', '../css/' + theme + '/main.css' );  
		break;
	case 'pink':
		external.globals( 'ColorRosterGroupHeader' ) = '#800000';
		external.globals( 'ColorModeName') = '#B90000';
		addCss ( 'theme', '../css/' + theme + '/main.css' );  
		break;
	}
}

/**
	Alter theme
*/
function AlterTheme( theme )
{
	var current = external.globals( 'theme' );
	
	if( current != theme )
	{
		try
		{
			external.globals( 'theme' ) = theme;
			
			var signin = document.getElementById( 'signin-dialog' ).style.display == 'block';
			
			if ( signin )
			{
				InitThemes( );
			} 
			else 
			{
				document.getElementById( 'rosterfield' ).style.display = 'none';
				document.getElementById( 'connecting' ).style.display = 'block';
			
				InitThemes( );
							
				setTimeout(function()
				{ 
					document.getElementById( 'rosterfield' ).style.display = 'block';
					document.getElementById( 'connecting' ).style.display = 'none';
				}, 50);
				
				setTimeout(function()
				{ 
					//Update opens chat
					var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
					for ( var i = 0; i < ContainerNames.length; ++i )
						external.globals( 'ChatSessionPool' ).Containers.Item( ContainerNames[i] ).ThemeUpdate();	
					
				}, 100);
			}
		} catch (ex) {}
	}
}	
/**
	Alter chat theme
*/
function ChatThemes ( )
{
	var theme = external.globals( 'theme' );

	switch ( theme )
	{
		case 'default':
			addCss ( 'theme', '' );
			break;
		default: 
			addCss ('theme', '../css/' + theme + '/chat-container.css' );
			break;
	}
}

/* Add or alter css href
*/
function addCss( id, href ) 
{
	var theme = document.getElementById(id);
	if ( !theme ) 
	{
		var link = document.createElement( "link" );
		link.href = href;
		link.id	= id;
		link.type = "text/css";
		link.rel = "stylesheet";
		link.media = "screen,print";
		document.getElementsByTagName( "head" )[0].appendChild( link );
	}
	else 
	{
		theme.href = href;
	}
}

