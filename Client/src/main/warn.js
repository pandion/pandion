function warn ( string )
{
	if ( ! external.globals( 'debug' ) )
		return true;

	if ( typeof string !== 'string' )
		string = JSON.stringify( string );

	if ( string.charAt( string.length - 1 ) == '\n' )
		external.globals( 'stderr' ).push( string );
	else
		external.globals( 'stderr' ).push( string  + '\n' );

	if ( external.globals( 'stderr' ).length > external.globals( 'stderrlength' ) )
		external.globals( 'stderr' ).shift();

	if ( external.windows.Exists( 'stderr' ) )
		external.windows( 'stderr' ).Do( 'redraw', 0 );
}
