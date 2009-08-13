/* Hides the window when it is closed
 * Only the notification icon should be visible
 */
function TrayOnClose ()
{
	external.wnd.minimize();
	external.wnd.hide( true );
}

/* Fires when the window is minimized
 */
function TrayOnMinimize ()
{
}

/* Executed when someone clicks on the icon or doubleclicks it or does some other action involving the tray icon
 * Event codes are listed at:
 * <insert everchanging MSDN page here>
 */
function TrayCommotion ( id )
{
	if ( external.globals( 'trayonly' ) == 'yes' )
		TrayCommotionTrayOnly( id );
	else
		TrayCommotionNormal( id );
}

function TrayCommotionNormal ( id )
{
	if ( id == 514 )
	// left up
	{
		if ( external.windows.Exists( 'signup' ) )
		{
			external.windows( 'signup' ).restore();
			external.windows( 'signup' ).focus();
		}
		else if ( ! external.wnd.IsHidden && ! external.wnd.isActive() )
		{
			external.wnd.focus();
		}
	}
	else if ( id == 515 )
	// left double click
	{
		if ( external.globals( 'ChatSessionPool' ).Events.Count )
		{
			setTimeout( 'external.globals( \'ClientRoster\' ).ShowUnread( external.cursorX, external.cursorY )', 100 );
		}
		else if ( ! external.windows.Exists( 'signup' ) )
		{
			external.wnd.hide( false );
			external.wnd.restore();
			external.wnd.focus();
		}
	}
	else if ( id == 516 )
	// right down
	{
		var cfg			= external.globals( 'cfg' );
		var connected	= external.globals( 'XMPPConnected' );
		var mode		= connected ? external.globals( 'cfg' )( 'lastmode' ) : -1;
		var roster		= ! ( document.getElementById( 'signin-dialog' ).style.display == 'block' || external.windows.Exists( 'signup' ) );
		var signup		= external.windows.Exists( 'signup' );
		var logout		= roster && ! external.globals( 'sspiserver' ).length;
		var ask			= roster && external.globals( 'cfg' )( 'askstatus' ).toString() == 'true';
		var msg			= roster && external.globals( 'cfg' )( 'lastmsg' );

		var menu = external.newPopupMenu;
		menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tray_about', [ external.globals( 'softwarename' ) ] ), 2 );
		menu.AddSeparator();

//		menu.AddItem( roster,	false, mode == 1,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_ffc' ), 11 );
		menu.AddItem( roster,	false, mode == 0 || mode == 1,	false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_available' ), 10 );
		menu.AddItem( roster,	false, mode == 4,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_busy' ), 14 );
		menu.AddItem( roster,	false, mode == 2 || mode == 3,	false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_away' ), 12 );
//		menu.AddItem( roster,	false, mode == 3,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_xaway' ), 13 );
		menu.AddItem( roster,	false, mode == 5,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_invisible' ), 15 );
//		menu.AddItem( roster,	false, mode == -1,				false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_offline' ), 19 );
		menu.AddSeparator();
		menu.AddItem( roster,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tray_settings' ), 3 );
		menu.AddItem( logout,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_out' ), 4 );
		menu.AddItem( ! signup,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_exit' ), 1 );
		menu.Show( external.cursorX, external.cursorY, external.globals( 'Translator' ).Direction, true );

		switch ( menu.Choice )
		{
			case 1:
				/* Exit
				 */
				external.wnd.close();
			break;
			case 2:
				/* About
				 */
				dial_about();
			break;
			case 3:
				/* Settings
				 */
				dial_preferences( '' );
			break;
			case 4:
				/* Sign out
				 */
				if ( ask )
					dial_status_message( -1 );
				else
				{
					mode_new( -1, '' );
					dial_logout();
				}
			break;
			case 10: // available
				if ( ask )
					dial_status_message( 0 );
				else
					mode_new( 0, msg );
			break;
			case 11: // ffc
				if ( ask )
					dial_status_message( 1 );
				else
					mode_new( 1, msg );
			break;
			case 12: // away
				if ( ask )
					dial_status_message( 2 );
				else
					mode_new( 2, msg );
			break;
			case 13: // xaway
				if ( ask )
					dial_status_message( 3 );
				else
					mode_new( 3, msg );
			break;
			case 14: // dnd
				if ( ask )
					dial_status_message( 4 );
				else
					mode_new( 4, msg );
			break;
			case 15: // invisible
				if ( ask )
					dial_status_message( 5 );
				else
					mode_new( 5, msg );
			break;
			case 19: // offline
				if ( ask )
					dial_status_message( -1 );
				else
					mode_new( -1, msg );
			break;
		}
	}
}

function TrayCommotionTrayOnly ( id )
{
	if ( id == 513 || id == 514 || id == 515 )
	// left down, left up, double left
	{
		if ( external.windows.Exists( 'login' ) )
		{
			external.windows( 'login' ).restore();
			external.windows( 'login' ).focus();
		}
		else if ( external.windows.Exists( 'signup' ) )
		{
			external.windows( 'signup' ).restore();
			external.windows( 'signup' ).focus();
		}
		else
		{
			dial_headlines();
		}
	}
	else if ( id == 516 )
	// right down
	{
		var menu = external.newPopupMenu;
		menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tray_about', [ external.globals( 'softwarename' ) ] ), 1 );
		menu.Show( external.cursorX, external.cursorY, external.globals( 'Translator' ).Direction, true );

		switch ( menu.Choice )
		{
			case 1:
				/* About
				 */
				dial_about();
			break;
		}
	}
}
