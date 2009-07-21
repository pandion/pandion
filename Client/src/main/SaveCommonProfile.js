function SaveCommonProfile ()
{
	var Data = new ActiveXObject( 'Scripting.Dictionary' );
	Data.Add( 'autologin',				external.globals( 'autologin' ) );
	Data.Add( 'language',				external.globals( 'language' ) );
	Data.Add( 'last_address',			external.globals( 'last_address' ) );
	Data.Add( 'last_password',			external.globals( 'last_password_remember' ).toString() != 'true' ? '' : external.globals( 'last_password' ) );
	Data.Add( 'last_password_remember',	external.globals( 'last_password_remember' ) );
	Data.Add( 'last_autoupdate',		external.globals( 'last_autoupdate' ) );
	Data.Add( 'serveraddress',			external.globals( 'serveraddress' ) );
	Data.Add( 'serverport',				external.globals( 'serverport' ) );
	Data.Add( 'encryption',				external.globals( 'encryption' ) );
	Data.Add( 'authentication',			external.globals( 'authentication' ) );
	Data.Add( 'proxymethod',			external.globals( 'proxymethod' ) );
	Data.Add( 'proxyaddress',			external.globals( 'proxyaddress' ) );
	Data.Add( 'proxyport',				external.globals( 'proxyport' ) );
	hash2file( Data, external.globals( 'usersdir' ) + 'global.xml' );
}
