function history_add ( ShortAddress, Time, PlainBody, Direction )
{
	if ( external.globals( 'cfg' ).Item( 'history_store' ).toString() == 'true' && PlainBody.length )
	{
		if ( ShortAddress.indexOf( '/' ) != -1 )
			ShortAddress = ShortAddress.substr( 0, ShortAddress.indexOf( '/' ) );

		var Message		= Time + ';' + ( Direction ? 'in' : 'out' ) + ';' + PlainBody.replace( /%/mg, '%25' ).replace( /\r/mg, '%0D' ).replace( /\n/mg, '%0A' );
		var Path		= external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '\\';
		var Filename	= ( new MD5() ).digest( ShortAddress );

		var File = external.File( Path + Filename + '.log' );
		File.WriteLine( Message );
		File.Close();

		if ( ! external.Directory.Exists( Path + 'Message Cache' ) )
			external.Directory.Create( external.globals( 'usersdir' ) + 'Message Cache' );

		File = external.File( Path + 'Message Cache\\' + Filename + '.buffer' );
		File.WriteLine( Message );
		File.Close();

		if ( external.windows.Exists( ShortAddress + '/history' ) )
			external.windows( ShortAddress + '/history' ).Do( 'receive', new Array( Time, PlainBody, Direction ) );
	}
}
