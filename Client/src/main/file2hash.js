function file2hash ( filepath )
{
	var hash = new ActiveXObject( 'Scripting.Dictionary' );
	var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
	dom.async = false;
	dom.resolveExternals = false;
	dom.load( filepath );
	if ( dom.documentElement == null )
		return false;
	var children = dom.documentElement.childNodes;
	for ( var j = 0; j < children.length; ++j )
	{
		var tag = children.item( j );
		if ( hash.Exists( tag.tagName ) )
			hash( tag.tagName ) = tag.text;
		else
			hash.Add( tag.tagName, tag.text );
	}
	return hash;
}
