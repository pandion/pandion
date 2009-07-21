function outnit ()
{
	try
	{
		external.notifyIcon.remove();

		if ( external.globals( 'XMPPConnected' ) )
			mode_new( -1, external.globals( 'cfg' )( 'lastmsg' ) );

		SettingsSave();

		if ( external.globals.Exists( 'autoupdatecommand' ) )
			external.shellExec( 'open', external.globals( 'autoupdatecommand' ), external.globals( 'autoupdateparameters' ), external.globals( 'autoupdatedirectory' ), 1 );
	}
	catch ( e )
	{
	}
}
