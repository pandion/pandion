function pass_code ( enc_or_dec, str )
{
	/* Encode
	 */
	if ( enc_or_dec )
	{
		var e = '';
		for ( var i = 0; i < str.length; i++ )
			e += escape( String.fromCharCode( str.charCodeAt(i) + ( str.length * str.length ) + 127 ) );
		return e;
	}
	/* Decode
	 */
	else
	{
		var d = '';
		str = unescape( str );
		for ( var i = 0; i < str.length; i++ )
			d += String.fromCharCode( str.charCodeAt(i) - ( str.length * str.length ) - 127 );
		return d;
	}
}
