function dial_font ()
{
	var cfg = external.globals( 'cfg' );
	var dialog = external.ComCtrl.ComDlg.ChooseFont;

	dialog.Flags = 1 | 256 | 65536;

	var RGB = cfg( 'user_dialog_color' );
	var BGR = RGB.charAt( 5 ) + RGB.charAt( 6 ) + RGB.charAt( 3 ) + RGB.charAt( 4 ) + RGB.charAt( 1 ) + RGB.charAt( 2 );

	dialog.Color = parseInt( BGR, 16 );
	dialog.FontFace = cfg( 'user_dialog_face' );
	dialog.PointSize = Math.max( parseInt( cfg( 'user_dialog_size' ), 10 ), 1 );
	dialog.FontWeight = cfg( 'user_dialog_bold' ) == 'bold' ? 700 : 400;
	dialog.FontItalic = cfg( 'user_dialog_italic' ) == 'italic';
	dialog.FontStrikeOut = cfg( 'user_dialog_strike' ).toString() == 'true';
	dialog.FontUnderLine = cfg( 'user_dialog_underline' ).toString() == 'true';

	dialog.Display();

	BGR = dialog.Color.toString( 16 );
	while ( BGR.length < 6 )
		BGR = '0' + BGR;
	RGB = BGR.charAt( 4 ) + BGR.charAt( 5 ) + BGR.charAt( 2 ) + BGR.charAt( 3 ) + BGR.charAt( 0 ) + BGR.charAt( 1 );
	cfg( 'user_dialog_color' ) = '#' + RGB;
	cfg( 'user_dialog_face' ) = dialog.FontFace;
	cfg( 'user_dialog_size' ) = dialog.PointSize + 'pt';
	cfg( 'user_dialog_bold' ) = dialog.FontWeight > 400 ? 'bold' : 'normal';
	cfg( 'user_dialog_italic' ) = dialog.FontItalic ? 'italic' : 'normal';
	cfg( 'user_dialog_strike' ) = dialog.FontStrikeOut ? true : false;
	cfg( 'user_dialog_underline' ) = dialog.FontUnderLine ? true : false;

	var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
	for ( var i = 0; i < ContainerNames.length; ++i )
		external.globals( 'ChatSessionPool' ).Containers.Item( ContainerNames[i] ).InputUpdate();

	ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
	for ( var i = 0; i < ContainerNames.length; ++i )
		external.globals( 'ConferenceSessionPool' ).Containers.Item( ContainerNames[i] ).InputUpdate();
}
