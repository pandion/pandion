function Translator ()
{
	this.CodeCache = new ActiveXObject( 'Scripting.Dictionary' );
	this.Direction = false; // false: LTR, true: RTL
	this.HTMLCache = new ActiveXObject( 'Scripting.Dictionary' );

	this.Reload = Reload;
	this.LoadFile = LoadFile;
	this.Translate = Translate;
	this.TranslateWindow = TranslateWindow;

	function Reload ()
	{
		this.Direction = false;
		this.CodeCache.RemoveAll();
		this.HTMLCache.RemoveAll();

		var path = external.globals( 'cwd' ) + '..\\languages\\';
		var language = 'en';
		var userLanguage = navigator.userLanguage.toLowerCase();
		if ( userLanguage == 'zh-hk' || userLanguage == 'zh-sg' )
			userLanguage = 'zh-tw';

		this.LoadFile( path + language + '.xml' );

		if ( external.FileExists( path + external.globals( 'language' ) + '.xml' ) )
			language = external.globals( 'language' );
		else if ( external.FileExists( path + userLanguage + '.xml' ) )
			language = userLanguage;
		else if ( userLanguage.indexOf( '-' ) > 0 && external.FileExists( path + userLanguage.substr( 0, userLanguage.indexOf( '-' ) ) + '.xml' ) )
			language = userLanguage.substr( 0, userLanguage.indexOf( '-' ) );

		if ( language != 'en' )
			this.LoadFile( path + language + '.xml' );

		external.globals( 'language' ) = language;
	}

	function LoadFile ( path )
	{
		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.async = false;
		dom.resolveExternals = false;
		dom.load( path );
		if ( dom.documentElement == null )
			return;

		if ( dom.documentElement.getAttribute( 'dir' ).length !== null )
			this.Direction = dom.documentElement.getAttribute( 'dir' ).toString().toLowerCase() == 'rtl';

		var windowNodes = dom.documentElement.selectNodes( '/translation/window[@name]' );
		for ( var i = windowNodes.length - 1; i >= 0; --i )
		{
			var windowName = windowNodes.item(i).getAttribute( 'name' );
			if ( ! this.CodeCache.Exists( windowName ) )
				this.CodeCache.Add( windowName, new ActiveXObject( 'Scripting.Dictionary' ) );
			if ( ! this.HTMLCache.Exists( windowName ) )
				this.HTMLCache.Add( windowName, new ActiveXObject( 'Scripting.Dictionary' ) );

			var iterator = windowNodes.item(i).selectNodes( './item[@id]' );
			for ( var j = iterator.length - 1; j >= 0; --j )
			{
				var itemTag = iterator.item(j);
				var data = {};
				var childTags = itemTag.childNodes;
				for ( var k = childTags.length - 1; k >= 0; --k )
					switch ( childTags.item(k).tagName )
					{
						case 'title': data.title = childTags.item(k).text; break;
						case 'value': data.value = childTags.item(k).text; break;
						case 'accessKey': data.accessKey = childTags.item(k).text; break;
						case 'innerHTML': data.innerHTML = childTags.item(k).text; break;
						case 'innerText': data.innerText = childTags.item(k).text; break;
					}
				if ( ! this.HTMLCache( windowName ).Exists( itemTag.getAttribute( 'id' ) ) )
					this.HTMLCache( windowName ).Add( itemTag.getAttribute( 'id' ), data );
				else
					this.HTMLCache( windowName )( itemTag.getAttribute( 'id' ) ) = data;
			}

			iterator = windowNodes.item(i).selectNodes( './code[@id]' );
			for ( var j = iterator.length - 1; j >= 0; --j )
				if ( ! this.CodeCache( windowName ).Exists( iterator.item(j).getAttribute( 'id' ) ) )
					this.CodeCache( windowName ).Add( iterator.item(j).getAttribute( 'id' ), iterator.item(j).text );
				else
					this.CodeCache( windowName )( iterator.item(j).getAttribute( 'id' ) ) = iterator.item(j).text;
		}
	}

	function Translate ( windowName, stringName, variables )
	{
		if ( ! this.CodeCache.Exists( windowName ) )
			return '';
		if ( ! this.CodeCache( windowName ).Exists( stringName ) )
			return '';
		return this.CodeCache( windowName )( stringName ).replace(
			/\${(\d+)}/mg,
			function ( $0, $1 )
			{
				if ( variables && $1 < variables.length )
					return variables[$1];
				return $0;
			}
		);
	}

	function TranslateWindow ( windowName, doc )
	{
		if ( ! this.HTMLCache.Exists( windowName ) )
			return;
		var windowCache = this.HTMLCache( windowName );
		var names = ( new VBArray( windowCache.Keys() ) ).toArray();
		for ( var i = names.length - 1; i >= 0; --i )
		{
			var htmlElem = doc.getElementById( names[i] );
			if ( htmlElem )
				for ( var textItem in windowCache( names[i] ) )
					htmlElem[ textItem ] = windowCache( names[i] )[ textItem ];
		}
	}
}
