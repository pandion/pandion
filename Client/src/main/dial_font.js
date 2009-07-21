function dial_font ()
{
	if ( external.windows.Exists( 'font' ) )
		external.windows( 'font' ).focus();
	else
		external.createWindow( 'font', external.globals( 'cwd' ) + 'font.html', null ).setAOT( external.globals( 'cfg' )( 'aotchat' ).toString() == 'true' );
}
