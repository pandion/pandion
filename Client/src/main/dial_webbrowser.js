function dial_webbrowser ( Address )
{
	if ( Address.substr( 0, 7 ) == 'http://' || Address.substr( 0, 8 ) == 'https://' )
	{
		try
		{
			var HTTPHandler = external.RegRead( 'HKEY_CLASSES_ROOT', 'HTTP\\shell\\open\\command', '' );
			if ( HTTPHandler.indexOf( 'iexplore.exe' ) != -1 )
				return window.open( Address, '_blank' );
		}
		catch ( e )
		{
		}
	}
	external.shellExec( 'open', Address, '', '', 0 );
}
