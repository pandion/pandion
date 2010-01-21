function dial_mode ()
{
	var dir = external.globals( 'cwd' ) + '..\\images\\status\\';
	var Menu = external.newPopupMenu;
	var mode = external.globals( 'XMPPConnected' ) ? external.globals( 'cfg' )( 'lastmode' ) : -1;
	var sspi = external.globals( 'sspiserver' ).length;
//	Menu.AddItem( true, false, mode == 1,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_ffc' ), 2 );
	Menu.AddItem( true, false, mode == 0 || mode == 1,	false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_available' ), 3 );
	Menu.AddItem( true, false, mode == 4,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_busy' ), 4 );
	Menu.AddItem( true, false, mode == 2 || mode == 3,	false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_away' ), 5 );
//	Menu.AddItem( true, false, mode == 3,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_xaway' ), 6 );
	Menu.AddItem( true, false, mode == 5,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_invisible' ), 7 );
	Menu.AddSeparator();
	Menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_change' ), 1 );
	Menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_avatar' ), 9 );
	Menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_profile' ), 10 );
	var posx = external.globals( 'Translator' ).Direction ? (window.screenLeft + external.wnd.width - 50) : (window.screenLeft + 50);
	var posy = window.screenTop + 48;
	Menu.Show( posx, posy, external.globals( 'Translator' ).Direction );

	var msg = external.globals( 'cfg' )( 'lastmsg' );
	switch ( Menu.Choice )
	{
		case 1: // leave a note
			dial_status_message();
		break;
		case 2: // ffc
			mode_new( 1, msg );
		break;
		case 3: // available
			mode_new( 0, msg );
		break;
		case 4: // dnd
			mode_new( 4, msg );
		break;
		case 5: // away
			mode_new( 2, msg );
		break;
		case 6: // xaway
			mode_new( 3, msg );
		break;
		case 7: // invisible
			mode_new( 5, msg );
		break;
		case 9: // avatar
			dial_avatar_pick();
		break;
		case 10: // profile
			dial_vcard_edit();
		break;
	}
}
