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

/**
	Debug MSG
*/
function debug_msg( msg , title ) 
{
	try
	{
		if (typeof msg == 'string' )
			external.wnd.messageBox( true, msg, title, 0 );
		
		else 
		{
			var output = '';
			for (var property in msg) 
			{
				output += property + ': ' + msg[property]+'; ';
				//external.wnd.messageBox( true, msg[property], title, 0 );
			}
			external.wnd.messageBox( true, output, title, 0 );
		}
		
	} catch (ex) 
	{
		external.wnd.messageBox( true, ex.message, "ERROR", 0 );
	}
}