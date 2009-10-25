function dial_logout ()
{
	/* Plugin event
	 */
	var context = new ActiveXObject( 'Scripting.Dictionary' );
	context.Add( 'name', 'onLogout' );
	external.globals( 'ClientPluginContainer' ).DispatchEvent( context );

	/* Reset tray icon
	 */
	external.globals( 'ChatSessionPool' ).Events.RemoveAll();
	if ( external.FileExists( external.globals( 'cwd' ) + '..\\images\\brand\\tray.ico' ) )
		external.notifyIcon.setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\tray.ico', 0 );
	else
		external.notifyIcon.setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico', 0 );
	external.notifyIcon.setText( external.globals( 'softwarename' ) );
	external.notifyIcon.update();

	/* Close all windows except debug and main
	 */
	var keys = ( new VBArray( external.windows.Keys() ) ).toArray();
	for ( var i = 0; i < keys.length; i++ )
		if ( keys[i] != 'stderr' && keys[i] != 'MainWindow' )
			external.windows( keys[i] ).close();

	external.globals( 'XMPPSSPIReconnect' ) = false;

	SettingsSave();
	external.globals( 'cfg' ).RemoveAll();
	external.globals( 'cfg' ) = false;

	dial_login();
}
