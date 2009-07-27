function login ( Address, Password )
{
	/* Check input
	 */
	if ( typeof Address != 'object' )
		Address = new XMPPAddress( new String( Address ) );
	if ( typeof Password != 'string' )
		Password = new String( Password );

	/* Reset the client
	 */
	external.globals( 'XMPPReconnect' ) = false;
	external.globals( 'headlines' )		= new Array();
	external.globals( 'block' ).RemoveAll();
	external.globals( 'ChatSessionPool' ).Clear();
	external.globals( 'ClientRoster' ).Clear();

	/* Copy default conference bookmarks to profile directory
	 */
	if ( ! external.FileExists( external.globals( 'usersdir' ) + 'Profiles\\' + ( Address.User.length ? Address.User + '@' + Address.Host : Address.Host ) + '\\bookmarks.xml' )
		&& external.FileExists( external.globals( 'cwd' ) + '..\\settings\\bookmarks.xml' )
	)
		external.File(	external.globals( 'cwd' ) + '..\\settings\\bookmarks.xml' ).Copy(
						external.globals( 'usersdir' ) + 'Profiles\\' + ( Address.User.length ? Address.User + '@' + Address.Host : Address.Host ) + '\\bookmarks.xml' );

	/* Load the profile settings from the disk
	 */
	LoadProfile( Address );
	var cfg = external.globals( 'cfg' );

	/* Authentication credentials
	 */
	cfg( 'password' )	= Password;
	cfg( 'username' )	= Address.User;
	cfg( 'server' )		= Address.Host;

	/* Default resource
	 */
	if ( Address.Resource.length )
		cfg( 'resource' ) = Address.Resource;
	else if ( ! cfg( 'resource' ).length )
		cfg( 'resource' ) = external.globals( 'softwarename' );

	/* Display the window
	 */
	external.notifyIcon.setText( external.globals( 'softwarename' ) );
	external.notifyIcon.update();
	external.wnd.setIcon( external.globals(	'cwd' )	+ '..\\images\\brand\\default.ico' );
	external.globals( 'ClientPluginContainer' ).Plugins( '/roster' ).ClientPluginTab.Activate();
	external.globals( 'ClientRoster' ).Search.Close();

	/* Save the login data
	 */
	if ( ! external.globals( 'sspiserver' ).length )
	{
		external.globals( 'last_password' )	= pass_code( true, Password );
		external.globals( 'last_address' )	= Address.ShortAddress();
		SaveCommonProfile();
	}

	/* Connect to the server
	 */
	mode_new( parseInt( cfg( 'lastmode' ), 10 ), cfg( 'lastmsg' ) );
}
