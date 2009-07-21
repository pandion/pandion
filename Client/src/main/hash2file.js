function hash2file ( hash, filepath )
{
	var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
	dom.async = false;
	dom.resolveExternals = false;
	var root = dom.createElement( 'settings' );
	var keys = ( new VBArray( hash.Keys() ) ).toArray().sort();
	for ( var i = 0; i < keys.length; i++ )
	{
		root.appendChild( dom.createTextNode( '\n\t' ) );
		var tag = dom.createElement( keys[i] );
		tag.text = hash( keys[i] ).toString();
		root.appendChild( tag );
	}
	root.appendChild( dom.createTextNode( '\n' ) );
	dom.appendChild( dom.createProcessingInstruction( 'xml', 'version="1.0" encoding="UTF-8"' ) );
	dom.appendChild( root );
	try
	{
		dom.save( filepath );
	}
	catch ( e )
	{
		warn( 'ERRO: ' + e.description );
	}
	return true;
}
