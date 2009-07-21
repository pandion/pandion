function dial_webbrowser ( Address )
{
	if ( Address.substr( 0, 7 ) == 'http://' || Address.substr( 0, 8 ) == 'https://' )
	{
		try
		{
			var HTTPHandler = external.RegRead( 'HKEY_CLASSES_ROOT', 'HTTP\\shell\\open\\command', '' );
			if ( HTTPHandler.indexOf( 'iexplore.exe' ) != -1 )
				window.open( Address, '_blank' );
			else
				external.shellExec( 'open', Address, '', '', 0 );
		}
		catch ( e )
		{
			external.shellExec( 'open', Address, '', '', 0 );
		}
	}
	else
		external.shellExec( 'open', Address, '', '', 0 );
}
