function BackgroundBrowse ()
{
	var List		= external.globals( 'BackgroundList' );
	var ListHooks	= external.globals( 'BackgroundHooks' );
	if ( List.readyState == 4 )
	{
		if ( List.parseError.errorCode == 0 && List.documentElement && ListHooks.Count )
		{
			var Codenames = ( new VBArray( ListHooks.Keys() ) ).toArray();
			for ( var i = 0; i < Codenames.length; ++i )
			{
				var ListItem = List.documentElement.selectSingleNode( '/background/style[ name and download and codename = "' + Codenames[i] + '" ]' );
				if ( ListItem )
				{
					var ItemName = ListItem.selectSingleNode( 'name' ).text;
					var Location = ListItem.selectSingleNode( 'download' ).text;
					dial_extension_download( Location, ItemName, false, 'background' );
				}
			}
		}
		ListHooks.RemoveAll();
		if ( external.windows.Exists( 'background_download' ) )
			external.windows( 'background_download' ).Do( 'Draw', null );
	}
}
