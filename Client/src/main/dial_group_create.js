function dial_group_create ( address, group )
{
	var WindowName = address ? address + '/newgroup' : 'newgroup';
	if ( external.windows.Exists( WindowName ) )
		return external.windows( WindowName ).focus();
	else
		with ( external.createWindow( WindowName, external.globals( 'cwd' ) + 'group_create.html', new Array( window, address, group ) ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_group_create' ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			MinHeight = MinWidth = 0;
			setSize( 320, 80 );
			setPos( ( screen.availWidth - 320 ) / 2, ( screen.availHeight - 80 ) / 2 );
		}
}
