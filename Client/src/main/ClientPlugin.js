/* This is a single plugin's foundation.
 */
function ClientPluginCore ( container )
{
	this.Id					= '';
	this.Name				= '';
	this.Path				= '';
	this.Type				= '';
	this.Version			= '';
	this.Visible			= true;
	this.Container			= container;
	this.Uninstall			= true;
	this.Description		= '';
	this.ClientPluginTab	= false;
	this.Menus				= new ActiveXObject( 'Scripting.Dictionary' ); // menuname -> [ ClientPluginMenuInfo, ClientPluginMenuInfo, ... ]
	this.Events				= new ActiveXObject( 'Scripting.Dictionary' ); // eventname -> ClientPluginEvent
}

/* This holds information about the menu entry.
 */
function ClientPluginMenuInfo ()
{
	this.Text		= '';
	this.Value		= '';
	this.OnlineOnly	= false;
}

/* This is an event datastructure.
 */
function ClientPluginEvent ()
{
	this.URL		= false;
	this.File		= false;
	this.XPath		= false;
	this.JScript	= false;
}

/* This is a tab and its button in the interface.
 */
function ClientPluginTab ( plugin )
{
	this.Icon		= '';
	this.Tooltip	= '';
	this.HTMLArea	= false;
	this.IsActive	= false;
	this.HTMLButton	= false;
	this.PluginCore	= plugin;

	this.Activate	= Activate;
	this.DrawButton	= DrawButton;

	/* Show or refresh the button.
	 */
	function DrawButton ()
	{
		if ( this.HTMLButton )
		{
			/* Update button
			 */
			this.HTMLButton.className = this.IsActive ? 'tab-bar-button-active' : 'tab-bar-button';
			this.HTMLButton.firstChild.src = this.Icon;
			this.HTMLButton.title = this.Tooltip;
		}
		else
		{
			/* Create button
			 */
			this.HTMLButton = document.createElement( 'SPAN' );
			this.HTMLButton.className = this.IsActive ? 'tab-bar-button-active' : 'tab-bar-button';
			this.HTMLButton.title = this.Tooltip;
			this.HTMLButton.ClientPluginTab = this;

			var image = document.createElement( 'IMG' );
			image.src = this.Icon;
			image.width = image.height = 16;
			this.HTMLButton.insertAdjacentElement( 'beforeEnd', image );

			this.HTMLButton.attachEvent(
				'onmousedown',
				function ()
				{
					if ( event.button != 1 )
						return;
					else if ( event.srcElement.tagName == 'IMG' )
						event.srcElement.parentNode.ClientPluginTab.Activate();
					else
						event.srcElement.ClientPluginTab.Activate();
				}
			);
			this.PluginCore.Container.HTMLTabBar.insertAdjacentElement( 'beforeEnd', this.HTMLButton );
		}
	}

	/* Hide all other tabs, make the button active and show the tab contents.
	 */
	function Activate ()
	{
		if ( this.IsActive )
			return;

		/* Deactivate old tab
		 */
		var names = ( new VBArray( this.PluginCore.Container.Plugins.Keys() ) ).toArray();
		for ( var i = 0; i < names.length; ++i )
		{
			var tab = this.PluginCore.Container.Plugins( names[i] ).ClientPluginTab;
			if ( tab && tab.IsActive )
			{
				tab.IsActive = false;
				tab.DrawButton();
				tab.HTMLArea.style.display = 'none';
				continue;
			}
		}

		/* Activate new tab
		 */
		this.IsActive = true;
		this.DrawButton();
		this.HTMLArea.style.display = 'block';
	}
}

/* This loads and manages the tabs.
 */
function ClientPluginContainer ()
{
	this.Plugins		= new ActiveXObject( 'Scripting.Dictionary' ); // filename->PluginCore
	this.HTMLTabBar		= document.getElementById( 'tab-bar' );
	this.HTMLTabArea	= document.getElementById( 'tab-area' );

	this.Reload			= Reload;
	this.ParseURL		= ParseURL;
	this.LoadPlugin		= LoadPlugin;
	this.UnloadPlugin	= UnloadPlugin;
	this.DispatchEvent	= DispatchEvent;

	function LoadPlugin ( name, path )
	{
		var plugin = new ClientPluginCore( this );

		/* Load XML file into DOM
		 */
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.async = false;
		dom.resolveExternals = false;
		dom.validateOnParse = false;
		if ( ! dom.load( path ) )
			return warn( 'Can not load plug-in file\n' + path );

		/* Id
		 */
		plugin.Id = name;

		/* Type
		 */
		plugin.Type = dom.documentElement.getAttribute( 'type' );

		/* Type
		 */
		plugin.Type = dom.documentElement.getAttribute( 'type' );

		/* Name
		 */
		plugin.Name = dom.documentElement.selectSingleNode( '/plugin/name' );
		if ( ! plugin.Name )
			return warn( 'Plug-in has no name\n' + path );
		plugin.Name = plugin.Name.text.substr( 0, 215 );
		if ( ! plugin.Name.length )
			return warn( 'Plug-in has no name\n' + path );

		/* Version
		 */
		plugin.Version = dom.documentElement.selectSingleNode( '/plugin/version' );
		if ( ! plugin.Version )
			return warn( 'Plug-in has no version\n' + path );
		plugin.Version = plugin.Version.text.substr( 0, 16 );
		if ( ! plugin.Version.length )
			return warn( 'Plug-in has no version\n' + path );

		/* Description
		 */
		plugin.Description = dom.documentElement.selectSingleNode( '/plugin/description' );
		if ( ! plugin.Description )
			return warn( 'Plug-in has no description\n' + path );
		plugin.Description = plugin.Description.text;
		if ( ! plugin.Description.length )
			return warn( 'Plug-in has no description\n' + path );

		/* Visible
		 */
		if ( dom.documentElement.selectSingleNode( '/plugin/visible[.="no"]' ) )
			plugin.Visible = false;

		/* Uninstall
		 */
		if ( dom.documentElement.selectSingleNode( '/plugin/uninstall[.="no"]' ) )
			plugin.Uninstall = false;

		/* The directory to store plugin files.
		 */
		plugin.Path = external.globals( 'usersdir' ) + 'Plugins\\' + external.StringToSHA1( name ) + '\\';
		try
		{
			if ( ! external.Directory.Exists( plugin.Path ) )
				external.Directory.Create( plugin.Path );
		}
		catch ( e )
		{
			return warn( 'Can not create directory to extract plug-in\n' + plugin.Path );
		}

		/* Extract files
		 */
		var tags = dom.documentElement.selectNodes( '/plugin/file[name and data]' );
		for ( var i = 0; i < tags.length; ++i )
			with ( tags.item( i ) )
			{
				/* Get the filename
				 */
				var filename = selectSingleNode( './name' );
				if ( ! filename )
					continue;
				filename = filename.text;
				if ( external.FileExists( plugin.Path + filename ) )
					external.File( plugin.Path + filename ).Delete();
				if ( filename.length == 0 || filename.length > 215 || filename.search( /\\\/\:\*\?\"\<\>\|/ ) != -1 )
					continue;

				/* Decode the data
				 */
				var filedata = selectSingleNode( './data' );
				if ( ! filedata )
					continue;
				else if ( filedata.getAttribute( 'content' ).toLowerCase() == 'base64' )
					external.File( plugin.Path + filename ).WriteBase64( filedata.text );
				else if ( filedata.getAttribute( 'content' ).toLowerCase() == 'plain' )
					external.File( plugin.Path + filename ).WriteLine( filedata.text );
				else
					continue;
			}

		/* Parse events
		 */
		tags = dom.documentElement.selectNodes( '/plugin/event[ @for and ( code or url or file ) ]' );
		for ( var i = 0; i < tags.length; ++i )
			with ( tags.item( i ) )
			{
				if ( ! plugin.Events.Exists( getAttribute( 'for' ) ) )
					plugin.Events.Add( getAttribute( 'for' ), new Array() );

				var url = selectSingleNode( './url' );
				var code = selectSingleNode( './code[@parameter and @language]' );
				var file = selectSingleNode( './file' );

				var pluginevent = new ClientPluginEvent();

				if ( getAttribute( 'xpath' ) )
					pluginevent.XPath = getAttribute( 'xpath' );

				if ( code && code.getAttribute( 'parameter' ) && code.getAttribute( 'language' ).toLowerCase() == 'jscript' )
					pluginevent.JScript = new Function( code.getAttribute( 'parameter' ), code.text );

				if ( url && url.text.length )
					pluginevent.URL = url.text;
				else if ( file && file.text.length > 0 && file.text.length < 216 && file.text.search( /\\\/\:\*\?\"\<\>\|/ ) == -1 )
					pluginevent.File = file.text;

				plugin.Events( getAttribute( 'for' ) ).push( pluginevent );
			}
		if ( ! plugin.Events.Count )
			return warn( 'Plug-in contains no events\n' + path );

		/* Add items to the menus.
		 */
		tags = dom.documentElement.selectNodes( '/plugin/menu[@name]' );
		for ( var i = 0; i < tags.length; ++i )
		{
			var menuname = tags.item( i ).getAttribute( 'name' );
			if ( ! plugin.Menus.Exists( menuname ) )
				plugin.Menus.Add( menuname, new Array() );
			var tags_options = tags.item( i ).selectNodes( './option[text and value]' );
			for ( var j = 0; j < tags_options.length; ++j )
			{
				var option = tags_options.item( j );
				var data = new ClientPluginMenuInfo();
				data.Text = option.selectSingleNode( 'text' ).text;
				data.Value = option.selectSingleNode( 'value' ).text;
				if ( option.getAttribute( 'onlineonly' ) == 'yes' )
					data.OnlineOnly = true;
				plugin.Menus( menuname ).push( data );
			}
		}

		/* Display the tab button and create an IFRAME.
		 */
		if ( plugin.Type == 'tab-iframe' )
		{
			var tab = dom.documentElement.selectSingleNode( '/plugin/tab[icon and tooltip]' );
			if ( ! tab )
				return warn( 'Plug-in of type tab-iframe contains no tab icon and tooltip\n' + path );
			plugin.ClientPluginTab = new ClientPluginTab( plugin );
			plugin.ClientPluginTab.Icon = plugin.Path + tab.selectSingleNode( './icon' ).text;
			plugin.ClientPluginTab.Tooltip = tab.selectSingleNode( './tooltip' ).text;
			plugin.ClientPluginTab.HTMLArea = document.createElement( 'IFRAME' );
			with ( plugin.ClientPluginTab.HTMLArea )
			{
				style.display = 'none';
				height = width = '100%';
				src = 'about:blank';
			}
			plugin.ClientPluginTab.HTMLArea.Plugin = plugin;
			plugin.Container.HTMLTabArea.insertAdjacentElement( 'beforeEnd', plugin.ClientPluginTab.HTMLArea );
			plugin.ClientPluginTab.DrawButton();
		}
		/* Display the tab button and load the custom HTML.
		 */
		else if ( plugin.Type == 'tab-html' )
		{
			var tab = dom.documentElement.selectSingleNode( '/plugin/tab[icon and tooltip and html]' );
			if ( ! tab )
				return warn( 'Plug-in of type tab-html contains no tab HTML, icon and tooltip\n' + path );
			plugin.ClientPluginTab = new ClientPluginTab( plugin );
			plugin.ClientPluginTab.Icon = plugin.Path + tab.selectSingleNode( './icon' ).text;
			plugin.ClientPluginTab.Tooltip = tab.selectSingleNode( './tooltip' ).text;
			plugin.ClientPluginTab.HTMLArea = document.createElement( 'DIV' );
			with ( plugin.ClientPluginTab.HTMLArea )
			{
				style.display = 'none';
				style.height = '100%';
				insertAdjacentHTML( 'beforeEnd', tab.selectSingleNode( './html' ).text );
			}
			plugin.ClientPluginTab.HTMLArea.Plugin = plugin;
			plugin.Container.HTMLTabArea.insertAdjacentElement( 'beforeEnd', plugin.ClientPluginTab.HTMLArea );
			plugin.ClientPluginTab.DrawButton();
		}
		/* Do not display a tab for code-only plugins.
		 */
		else if ( plugin.Type == 'code' )
		{
		}
		else
			return warn( 'Plug-in type is not supported\n' + path );

		this.Plugins.Add( name, plugin );

		/* Fire the correct event sequence to load the plug-in's actions
		 */
		var context = new ActiveXObject( 'Scripting.Dictionary' );
		context.Add( 'name', 'onLoad' );
		context.Add( 'plugin', name );
		this.DispatchEvent( context );

		if ( external.globals( 'XMPPConnected' ) )
		{
			context = new ActiveXObject( 'Scripting.Dictionary' );
			context.Add( 'name', 'onConnect' );
			context.Add( 'plugin', name );
			this.DispatchEvent( context );

			if ( document.getElementById( 'connecting' ).style.display == 'none' )
			{
				context = new ActiveXObject( 'Scripting.Dictionary' );
				context.Add( 'name', 'onLoginSuccess' );
				context.Add( 'plugin', name );
				this.DispatchEvent( context );
			}
		}
	}

	/* Unload the plug-in from memory, then delete it from disk
	 */
	function UnloadPlugin ( name )
	{
		if ( this.Plugins.Exists( name ) )
		{
			var context = new ActiveXObject( 'Scripting.Dictionary' );
			context.Add( 'name', 'onUnload' );
			context.Add( 'plugin', name );
			this.DispatchEvent( context );

			if ( this.Plugins( name ).ClientPluginTab )
			{
				if ( this.Plugins( name ).ClientPluginTab.IsActive )
					this.Plugins( '/roster' ).ClientPluginTab.Activate();
				this.Plugins( name ).ClientPluginTab.HTMLArea.removeNode( true );
				this.Plugins( name ).ClientPluginTab.HTMLButton.removeNode( true );
				document.getElementById( 'tab-bar-row' ).style.display = this.HTMLTabBar.children.length > 1 ? 'block' : 'none';
			}

			try
			{
				if ( external.Directory.Exists( this.Plugins( name ).Path ) )
					external.Directory.Delete( this.Plugins( name ).Path );

				if ( external.FileExists( external.globals( 'usersdir' ) + 'Plugins\\' + name ) )
					external.File( external.globals( 'usersdir' ) + 'Plugins\\' + name ).Delete();

				if ( external.FileExists( external.globals( 'cwd' ) + '..\\plugins\\' + name ) )
					external.File( external.globals( 'cwd' ) + '..\\plugins\\' + name ).Delete();
			}
			catch ( e )
			{
			}

			this.Plugins.Remove( name );
		}
	}

	/* Search for new plugins on the harddrive, install them and load them in memory.
	 */
	function Reload ()
	{
		var Path = external.globals( 'usersdir' ) + 'Plugins\\';

		if ( external.Directory.Exists( Path ) )
		{
			var files = new VBArray( external.Directory.ListFiles( Path ) ).toArray();
			for ( var i = 0; i < files.length; ++i )
				if ( ! this.Plugins.Exists( files[i].Name ) && files[i].Name.substr( files[i].Name.length - 4 ) == '.pdn' )
					this.LoadPlugin( files[i].Name, Path + files[i].Name );
		}

		document.getElementById( 'tab-bar-row' ).style.display = this.HTMLTabBar.children.length > 1 ? 'block' : 'none';

		if ( external.windows.Exists( 'plugin_list' ) )
			external.windows( 'plugin_list' ).Do( 'plugin_refresh', null );
	}

	/* Sends the event to the correct plugin(s).
	 * context is a Dictionary object with various information about the event.
	 */
	function DispatchEvent ( context )
	{
		if ( ! context.Exists( 'name' ) || context( 'name' ).length == 0 )
			return false;

		var names = ( new VBArray( this.Plugins.Keys() ) ).toArray();
		for ( var i = 0; i < names.length; ++i )
			if ( this.Plugins( names[i] ).Events.Exists( context( 'name' ) ) )
			{
				if ( context.Exists( 'plugin' ) && context( 'plugin' ) != names[i] )
					continue;

				var handlers = this.Plugins( names[i] ).Events( context( 'name' ) );
				for ( var j = 0; j < handlers.length; ++j )
				{
					if ( handlers[j].XPath && ! ( context.Exists( 'xmldom' ) && context( 'xmldom' ).selectSingleNode( handlers[j].XPath ) ) )
						continue;

					with ( handlers[j] )
					{
						if ( this.Plugins( names[i] ).Type == 'tab-iframe' )
						{
							if ( typeof URL == 'string' )
								this.Plugins( names[i] ).ClientPluginTab.HTMLArea.src = this.ParseURL( URL );
							else if ( typeof File == 'string' )
								this.Plugins( names[i] ).ClientPluginTab.HTMLArea.src = this.Plugins( names[i] ).Path + File;
						}
						if ( typeof JScript == 'function' )
						{
							if ( JScript.call( this.Plugins( names[i] ), context ) )
								return true;
						}
					}
				}
			}
		return false;
	}

	/* Replace any tokens in the URL with the real values.
	 */
	function ParseURL ( url, pairs )
	{
		if ( ! pairs )
			pairs = {};
		if ( url.indexOf( '$' ) == -1 )
			return url;
		var random = Math.random();
		var substitutes = {
			'softwarename': function () { return encodeURIComponent( external.globals( 'softwarename' ) ) },
			'softwareversion': function () { return encodeURIComponent( external.globals( 'softwareversion' ) ) },
			'username': function () { return encodeURIComponent( external.globals( 'cfg' )( 'username' ) ) },
			'server': function () { return encodeURIComponent( external.globals( 'cfg' )( 'server' ) ) },
			'resource': function () { return encodeURIComponent( external.globals( 'cfg' )( 'resource' ) ) },
			'password': function () { return encodeURIComponent( external.globals( 'cfg' )( 'password' ) ) },
			'random': function () { return encodeURIComponent( random ) },
			'username-sha1': function () { return external.StringToSHA1( random + external.globals( 'cfg' )( 'username' ) ) },
			'server-sha1': function () { return external.StringToSHA1( random + external.globals( 'cfg' )( 'server' ) ) },
			'resource-sha1': function () { return external.StringToSHA1( random + external.globals( 'cfg' )( 'resource' ) ) },
			'password-sha1': function () { return external.StringToSHA1( random + external.globals( 'cfg' )( 'password' ) ) }
		};
		return url.replace(
			/\${([^}]+)}/g,
			function ( $0, $1 )
			{
				if ( typeof pairs[ $1 ] == 'function' )
					return pairs[ $1 ]();
				else if ( typeof pairs[ $1 ] == 'string' )
					return pairs[ $1 ];
				else if ( typeof substitutes[ $1 ] == 'function' )
					return substitutes[ $1 ]();
				else
					return $0;
			}
		);
	}
}
