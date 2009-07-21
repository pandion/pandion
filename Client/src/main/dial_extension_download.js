function dial_extension_download ( Location, Name, Visible, Type )
{
	var LocationHash = external.StringToSHA1( Location );
	if ( external.windows.Exists( 'extension-download/' + LocationHash ) )
		external.windows( 'extension-download/' + LocationHash ).focus();
	else
		with ( external.createWindow( 'extension-download/' + LocationHash, external.globals( 'cwd' ) + 'extension-download.html', [ window, Location, Name, Visible, Type ] ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_extension_download', [ Name ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			MinHeight = 100;
			MinWidth = 350;
			setSize( 350, 100 );
			setPos( ( screen.availWidth - 350 ) / 2, ( screen.availHeight - 100 ) / 2 );
		}
}
