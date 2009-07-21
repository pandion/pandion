/* Load and cache emoticon definitions.
 */
function EmoticonCache ()
{
	this.Styles					= new ActiveXObject( 'Scripting.Dictionary' );
	this.List					= new ActiveXObject( 'MSXML2.DOMDocument' );
	this.ListHooks				= new ActiveXObject( 'Scripting.Dictionary' );

	this.GetSubset				= GetSubset;
	this.LoadStyle				= LoadStyle;
	this.LoadSubset				= LoadSubset;
	this.InstallStyle			= InstallStyle;
	this.ListReadyStateChange	= ListReadyStateChange;

	this.List.onreadystatechange = ListReadyStateChange;

	/* Download all the available queued emoticon styles when the list is downloaded
	 */
	function ListReadyStateChange ()
	{
		var List		= external.globals( 'EmoticonCache' ).List;
		var ListHooks	= external.globals( 'EmoticonCache' ).ListHooks;
		if ( List.readyState == 4 )
		{
			if ( List.parseError.errorCode == 0 && List.documentElement && ListHooks.Count )
			{
				var Codenames = ( new VBArray( ListHooks.Keys() ) ).toArray();
				for ( var i = 0; i < Codenames.length; ++i )
				{
					var ListItem = List.documentElement.selectSingleNode( '/jisp/style[ name and download and codename = "' + Codenames[i] + '" ]' );
					if ( ListItem )
					{
						var ItemName = ListItem.selectSingleNode( 'name' ).text;
						var Location = ListItem.selectSingleNode( 'download' ).text;
						dial_extension_download( Location, ItemName, false, 'emoticon' );
					}
				}
			}
			ListHooks.RemoveAll();
			if ( external.windows.Exists( 'emoticon_list' ) )
				external.windows( 'emoticon_list' ).Do( 'emoticon_refresh', null );
			if ( external.windows.Exists( 'emoticon_download' ) )
				external.windows( 'emoticon_download' ).Do( 'Draw', null );
		}
	}

	/* Search, download and install emoticon styles
	 */
	function InstallStyle ( Codename )
	{
		if ( this.List.readyState == 4 )
		{
			if ( this.List.parseError.errorCode == 0 )
			{
				/* Check the cached emoticon styles list
				 */
				if ( this.List.documentElement )
				{
					var ListItem = this.List.documentElement.selectSingleNode( '/jisp/style[ name and download and codename = "' + Codename + '" ]' );
					if ( ListItem )
					{
						var ItemName = ListItem.selectSingleNode( 'name' ).text;
						var Location = ListItem.selectSingleNode( 'download' ).text;
						dial_extension_download( Location, ItemName, false, 'emoticon' );
					}
				}
				/* Download the emoticon styles list
				 */
				else
				{
					this.List.async				= true;
					this.List.resolveExternals	= false;
					this.List.load( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'emoticonlist' ) ) );
					if ( ! this.ListHooks.Exists( Codename ) )
						this.ListHooks.Add( Codename, null );
				}
			}
			/* Can not download XML, do not attempt again
			 */
			else
			{
			}
		}
		/* Add a hook and wait until the list is downloaded
		 */
		else if ( ! this.ListHooks.Exists( Codename ) )
			this.ListHooks.Add( Codename, null );
	}

	/* Load an emoticon style and language subset.
	 */
	function GetSubset ( Name, Languages )
	{
		if ( ! this.Styles.Exists( Name ) )
			this.Styles.Add( Name, this.LoadStyle( Name ) );

		if ( this.Styles( Name ) )
		{
			var ConcatLanguages = Languages.join();
			if ( ! this.Styles( Name ).Subsets.Exists( ConcatLanguages ) )
				this.Styles( Name ).Subsets.Add( ConcatLanguages, this.LoadSubset( this.Styles( Name ), Languages ) );

			return this.Styles( Name ).Subsets( ConcatLanguages );
		}
		else
		{
			this.Styles.Remove( Name );
			return null;
		}
	}

	/* Parse emoticon definition.
	 */
	function LoadStyle ( Name )
	{
		var Style = new EmoticonStyle();

		var dom					= new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.async				= false;
		dom.resolveExternals	= false;
		dom.load( external.globals( 'usersdir' ) + 'Emoticons\\' + Name + '\\icondef.xml' );

		Style.Name			= dom.selectSingleNode( '/icondef/meta/name' );
		Style.Version		= dom.selectSingleNode( '/icondef/meta/version' );
		Style.Description	= dom.selectSingleNode( '/icondef/meta/description' );

		Style.Name			= Style.Name		? Style.Name.text			: external.globals( 'Translator' ).Translate( 'main', 'emoticon_unknown' );
		Style.Version		= Style.Version		? Style.Version.text		: '';
		Style.Description	= Style.Description	? Style.Description.text	: '';

		var Icons = dom.selectNodes( '/icondef/icon[text]' );

		for ( var i = 0; i < Icons.length; ++i )
		{
			var Icon	= Icons.item( i );
			var Node	= null;
			var Action	= new EmoticonAction();
			var Strings	= Icon.getElementsByTagName( 'text' );

			if ( Node = Icon.selectSingleNode( 'object[@mime = "audio/mpeg" or @mime = "audio/x-wav" or @mime = "application/ogg" or @mime = "audio/x-ms-wma"]' ) )
				Action.Sound = Node.text;
			if ( Node = Icon.selectSingleNode( 'object[@mime = "text/css"]' ) )
				Action.Format = Node.text;
			if ( Node = Icon.selectSingleNode( 'object[@mime = "image/gif" or @mime = "image/png" or @mime = "image/jpeg" or @mime = "image/bmp"]' ) )
				Action.Graphic = Node.text;

			Style.Actions.push( Action );

			for ( var j = Strings.length - 1; j >= 0; --j )
			{
				var Language = Strings.item( j ).getAttribute( 'xml:lang' );
				if ( ! Language )
					Language = 'default';
				else if ( ! ( Language.length < 3 || Language.indexOf( 'i-' ) == 0 || Language.indexOf( 'x-' ) == 0 ) )
					Language = Language.substr( 0, Language.indexOf( '-' ) ).toLowerCase();
				else
					Language = Language.toLowerCase();

				var IconString = Strings.item( j ).text;

				if ( IconString.length )
				{
					if ( ! Style.Strings.Exists( Language ) )
						Style.Strings.Add( Language, new ActiveXObject( 'Scripting.Dictionary' ) );

					if ( ! Style.Strings( Language ).Exists( IconString ) )
						Style.Strings( Language ).Add( IconString, Style.Actions.length - 1 );
				}
			}
		}
		return Style.Strings.Count ? Style : false;
	}

	/* Merge language subsets.
	 */
	function LoadSubset ( Style, Languages )
	{
		var Subset			= new EmoticonSubset( Style );
//		var StringFilter	= /(\^|\$|\?|\*|\:|\;|\)|\(|\\|\/|\>|\<|\[|\]|\||\#|\+)/g;
		var StringFilter    = /(\^|\$|\?|\*|\:|\;|\)|\(|\\|\/|\>|\<|\[|\]|\||\#|\+|\.)/g;
		var UnsortedStrings	= new Array();

		for ( i = 0; i < Languages.length; ++i )
			if ( Style.Strings.Exists( Languages[i] ) )
			{
				var LanguageStrings = ( new VBArray( Style.Strings( Languages[i] ).Keys() ) ).toArray();
				for ( var j = 0; j < LanguageStrings.length; ++j )
					if ( ! Subset.StringToAction.Exists( LanguageStrings[j] ) )
					{
						UnsortedStrings.push( LanguageStrings[j].replace( StringFilter, '\\$1' ) );
						Subset.StringToAction.Add( LanguageStrings[j], Style.Strings( Languages[i] )( LanguageStrings[j] ) );
					}
			}

		Subset.Expression.compile( '(' + UnsortedStrings.sort( function(a,b){return a.length==b.length?0:a.length<b.length?1:-1} ).join( '|' ) + ')', 'm' );
		return Subset;
	}
}

function EmoticonStyle ()
{
	this.Name			= '';
	this.Version		= '';
	this.Description	= '';
	this.Actions		= new Array();
	this.Subsets		= new ActiveXObject( 'Scripting.Dictionary' );
	this.Strings		= new ActiveXObject( 'Scripting.Dictionary' );
}

function EmoticonSubset ( Style )
{
	this.Style			= Style;
	this.Expression		= new RegExp();
	this.StringToAction	= new ActiveXObject( 'Scripting.Dictionary' );
}

function EmoticonAction ()
{
	this.Sound		= null;
	this.Format		= null;
	this.Graphic	= null;
}
