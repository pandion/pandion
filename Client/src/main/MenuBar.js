/* Remove all items and submenus from the menubar.
 */
function MenuBarErase ()
{
	while ( external.wnd.menuBar.Items )
		external.wnd.menuBar.RemItem( external.wnd.menuBar.Items );
	external.wnd.menuBar.Update();
}

/* Redraw part or all of the menubar.
 */
function MenuBarUpdate ( section )
{
	var cfg = external.globals( 'cfg' );
	var connected = external.globals( 'XMPPConnected' );
	var mode = cfg && connected ? cfg( 'lastmode' ) : -1;
	var sspi = external.globals( 'sspiserver' ).length;
	var signin = document.getElementById( 'signin-dialog' ).style.display == 'block';
	var roster = ! signin;

	if ( ! section || section == 'file' )
	{
		external.wnd.menuBar.RemItem( 1 );

		var status = external.newPopupMenu;
		//status.AddItem( roster, false, mode == 1, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_ffc' ), 100 );
		status.AddItem( roster, false, mode == 0 || mode == 1, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_available' ), 101 );
		status.AddItem( roster, false, mode == 4, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_busy' ), 102 );
		status.AddItem( roster, false, mode == 2 || mode == 3, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_away' ), 103 );
		//status.AddItem( roster, false, mode == 3, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_xaway' ), 104 );
		status.AddItem( roster, false, mode == 5, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_invisible' ), 105 );
		//status.AddItem( roster, false, mode == -1, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_offline' ), 106 );
		status.AddSeparator();
		status.AddItem( roster, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_change' ), 107 );
		status.AddSeparator();
//		status.AddItem( roster, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_received' ), 13 );
		status.AddItem( roster, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_export' ), 14 );
		status.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_import' ), 15 );
		status.AddSeparator();
		status.AddItem( roster && ! sspi, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_out' ), 11 );
		status.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_status_exit' ), 12 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_status' ), 0, 1, status );
	}

	if ( ! section || section == 'actions' )
	{
		external.wnd.menuBar.RemItem( 2 );

		var Bookmarks = external.newPopupMenu;
		Bookmarks.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_bookmarks_add' ), 220 );
		Bookmarks.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_bookmarks_manage' ), 221 );
		if ( cfg )
		{
			var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
			dom.async = false;
			dom.resolveExternals = false;
			dom.load( external.globals( 'usersdir' ) + 'Profiles\\' + cfg( 'username' ) + '@' + cfg( 'server' ) + '\\bookmarks.xml' );
			if ( dom.documentElement )
			{
				var BookmarkNodes = dom.documentElement.selectNodes( '/bookmarks/room[@address]' );
				if ( BookmarkNodes.length )
					Bookmarks.AddSeparator();
				for ( var i = 0; i < Math.min( BookmarkNodes.length, 999 ); ++i )
					Bookmarks.AddItem( connected, false, false, false, 0, BookmarkNodes.item(i).getAttribute( 'address' ), 22000 + i );
			}
		}

		var actions = external.newPopupMenu;
		actions.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_add' ), 20 );
		actions.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_addgroup' ), 21 );
		actions.AddSeparator();
		actions.AddItem( roster, false, false, false, Bookmarks.handle,external.globals( 'Translator' ).Translate( 'main', 'menu_axn_bookmarks' ), 22 );
		actions.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_join' ), 23 );
		actions.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_create' ), 24 );
		actions.AddSeparator();
		actions.AddItem( connected && ! sspi, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_password' ), 25 );
		actions.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_profile' ), 26 );
		actions.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_axn_avatar' ), 27 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_axn' ), 1, 2, actions );
	}

	if ( ! section || section == 'tools' )
	{
		external.wnd.menuBar.RemItem( 3 );

		var aot = external.globals( 'aot' ).toString() == 'true';
		var list = cfg && cfg( 'contactlistdisplay' ) == 'detailed';
		var language = external.globals( 'MenuLanguages' );
		var username = cfg && cfg( 'username' ).length;

		var display = external.newPopupMenu;
		display.AddItem( true, false, list, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_detailed' ), 330 );
		display.AddItem( true, false, ! list, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_tool_compact' ), 331 );

		var language = external.newPopupMenu;
		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.async = false;
		dom.resolveExternals = false;
		dom.load( external.globals( 'cwd' ) + '..\\languages\\languages.xml' );
		if ( dom.documentElement )
		{
			var LanguageNodes = dom.documentElement.selectNodes( '/languages/language[@name and @code]' );
			for ( var i = 0; i < Math.min( LanguageNodes.length, 99 ); ++i )
			{
				var Name = LanguageNodes.item(i).getAttribute( 'name' );
				var Code = LanguageNodes.item(i).getAttribute( 'code' );
				language.AddItem( true, false, Code == external.globals( 'language' ), false, 0, Name, 3800 + i );
			}
		}

		var tools = external.newPopupMenu;
		tools.AddItem( connected,false,false,false, 0,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_transport' ), 36 );
		tools.AddItem( roster,false,false,false, 0,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_head' ), 30 );
		tools.AddItem( roster,false,false,false, 0,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_plugin' ), 37 );
		tools.AddSeparator();
		tools.AddItem( roster,false,false,false, display.Handle,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_view' ), 33 );
		tools.AddItem( true,false,false,false, language.Handle,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_language' ), 38 );
		tools.AddItem( true,aot,false,false, 0,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_aot' ), 34 );
		tools.AddSeparator();
		tools.AddItem( roster,false,false,false, 0,external.globals( 'Translator' ).Translate( 'main', 'menu_tool_settings' ), 35 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_tool' ), 2, 3, tools );
	}

	if ( ! section || section == 'help' )
	{
		external.wnd.menuBar.RemItem( 4 );

		var help = external.newPopupMenu;
		help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_welcome' ), 45 );
		help.AddSeparator();
		if ( external.globals( 'helpmanual' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_manual' ), 40 );
		if ( external.globals( 'helpforum' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_forum' ), 41 );
		if ( external.globals( 'helpcontact' ).length )
			help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_contact' ), 42 );
		if ( external.globals( 'helpmanual' ).length || external.globals( 'helpforum' ).length || external.globals( 'helpcontact' ).length )
			help.AddSeparator();
		help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_update' ), 43 );
		help.AddSeparator();
		help.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_help_about' ), 44 );

		external.wnd.menuBar.AddItem( external.globals( 'Translator' ).Translate( 'main', 'menu_help' ), 3, 4, help );
	}

	external.wnd.menuBar.Update();
}

/* Launch the corresponding code or plugin event.
 */
function MenuBarSelect ( id )
{
	var cfg = external.globals( 'cfg' );
	var msg = cfg && cfg( 'lastmsg' );
	switch ( id )
	{
		case 11: //	sign out
			mode_new( -1, '' );
			dial_logout();
			break;
		case 12: // exit
			external.wnd.close(); break;
		case 13: // received files
			if ( external.Directory.Exists( external.globals( 'cfg' ).Item( 'downloaddir' ) + '\\' ) )
				external.shellExec( 'open', external.globals( 'cfg' ).Item( 'downloaddir' ) + '\\', '', '', 1 );
			else
				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'msg_received_files' ), external.globals( 'softwarename' ), 0 | 48 ); break;
		case 14: // export
			dial_contacts_export(); break;
		case 15: // import
			dial_contacts_import(); break;

		case 107: // status message
			dial_status_message(); break;
		case 100: // ffc
			mode_new( 1, msg ); break;
		case 101: // available
			mode_new( 0, msg ); break;
		case 102: // dnd
			mode_new( 4, msg ); break;
		case 103: // away
			mode_new( 2, msg ); break;
		case 104: // xaway
			mode_new( 3, msg ); break;
		case 105: // invisible
			mode_new( 5, msg ); break;
		case 106: // offline
			mode_new( -1, msg ); break;

		case 20: // add contact
			dial_adduser(); break;
		case 21: // add group
			dial_group_create(); break;
		case 220: // add bookmark
			dial_bookmarks_add(); break;
		case 221: // manage bookmarks
			dial_bookmarks_manage(); break;
		case 23: // find conference room
			dial_conference_browse(); break;
		case 24: // create conference room
			dial_conference_create(); break;
		case 25: // change my password
			dial_password_change(); break;
		case 26: // edit my profile
			dial_vcard_edit(); break;
		case 27: // change my avatar
			dial_avatar_pick(); break;

		case 30: // headlines
			dial_headlines(); break;
		case 32: // view network traffic
			dial_console(); break;
		case 330: // detailed
			if ( cfg( 'contactlistdisplay' ) == 'compact' )
			{
				cfg( 'contactlistdisplay' ) = 'detailed';
				var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
				for ( var i = 0; i < TrackerNames.length; ++i )
					external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
				external.globals( 'ClientRoster' ).RefreshAll();
			}
			MenuBarUpdate( 'tools' ); break;
		case 331: // compact
			if ( cfg( 'contactlistdisplay' ) == 'detailed' )
			{
				cfg( 'contactlistdisplay' ) = 'compact';
				var TrackerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray();
				for ( var i = 0; i < TrackerNames.length; ++i )
					external.globals( 'ConferenceSessionPool' ).Trackers.Item( TrackerNames[i] ).RefreshOccupants();
				external.globals( 'ClientRoster' ).RefreshAll();
			}
			MenuBarUpdate( 'tools' ); break;
		case 34: // always on top
			external.wnd.setAOT( external.globals( 'aot' ) = ! ( external.globals( 'aot' ).toString() == 'true' ) );
			MenuBarUpdate( 'tools' ); break;
		case 35: // settings
			dial_preferences( '' ); break;
		case 36: // transports
			dial_transport_list(); break;
		case 37: // plug-ins
			dial_plugin_list(); break;

		case 45: // welcome screen
			dial_welcome(); break;
		case 40: // online manual
			dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpmanual' ) ) ); break;
		case 41: // support forum
			dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpforum' ) ) ); break;
		case 42: // contact us
			dial_webbrowser( external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpcontact' ) ) ); break;
		case 43: // check for update
			dial_autoupdate( true ); break;
		case 44: // about
			dial_about(); break;

		default:
			/* Switch the user interface language
			 */
			if ( id >= 3800 && id < 3900 )
			{
				var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
				dom.async = false;
				dom.resolveExternals = false;
				dom.load( external.globals( 'cwd' ) + '..\\languages\\languages.xml' );
				if ( dom.documentElement )
				{
					var LanguageNodes = dom.documentElement.selectNodes( '/languages/language[@name and @code]' );
					if ( LanguageNodes.length > id - 3800 )
					{
						var Code = LanguageNodes.item( id - 3800 ).getAttribute( 'code' );
						if ( Code != external.globals( 'language' ) )
						{
							var OldDefaultStatus = external.globals( 'Translator' ).Translate( 'main', 'cl_status_empty' );
							external.globals( 'language' ) = Code;
							SaveCommonProfile();
							external.globals( 'Translator' ).Reload();
							external.globals( 'Translator' ).TranslateWindow( 'main', document );
							MenuBarUpdate();
							document.getElementById( 'txt_welcome' ).innerText = external.globals( 'Translator' ).Translate( 'main', 'txt_welcome', [ external.globals( 'softwarename' ) ] );
							external.notifyIcon.setText( external.globals( 'softwarename' ) );
							if ( document.getElementById( 'content-dialog' ).style.display == 'block' )
							{
								mode_new( cfg( 'lastmode' ), cfg( 'lastmsg' ) );
								external.wnd.setTitle( cfg( 'username' ) + '@' + cfg( 'server' ) + ' - ' + external.globals( 'softwarename' ) );
							}
							else
							{
								external.wnd.setTitle( external.globals( 'softwarename' ) );
							}
							var GroupNames = ( new VBArray( external.globals( 'ClientRoster' ).Groups.Keys() ) ).toArray();
							for ( var i = 0; i < GroupNames.length; ++i )
							{
								var Group = external.globals( 'ClientRoster' ).Groups.Item( GroupNames[i] );
								Group.HTMLOnline.firstChild.innerHTML = external.globals( 'Translator' ).Translate( 'main', 'cl_group_empty' );
								Group.HTMLShowAll.title = external.globals( 'Translator' ).Translate( 'main', 'cl_group_expand' );
								Group.HTMLHeader.title = external.globals( 'Translator' ).Translate( 'main', 'tt-group-' + ( Group.ShowAll ? 'hide' : 'show' ) );
								var RosterItems = ( new VBArray( Group.Items.Keys() ) ).toArray();
								for ( var j = 0; j < RosterItems.length; ++j )
								{
									var RosterItem = external.globals( 'ClientRoster' ).Items.Item( RosterItems[j] );
									var Resources = ( new VBArray( RosterItem.Resources.Keys() ) ).toArray();
									for ( var k = 0; k < Resources.length; ++k )
										if ( RosterItem.Resources.Item( Resources[k] ).Status == OldDefaultStatus )
											RosterItem.Resources.Item( Resources[k] ).Status = external.globals( 'Translator' ).Translate( 'main', 'cl_status_empty' );
								}
							}
							external.globals( 'ClientRoster' ).RefreshAll();
							external.globals( 'ClientRoster' ).Search.Reset();
							external.globals( 'ClientRoster' ).Search.Blur();
							var ContainerNames = ( new VBArray( external.globals( 'ChatSessionPool' ).Containers.Keys() ) ).toArray();
							for ( var i = 0; i < ContainerNames.length; ++i )
								external.globals( 'ChatSessionPool' ).Containers.Item( ContainerNames[i] ).LanguageUpdate();
							ContainerNames = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Containers.Keys() ) ).toArray();
							for ( var i = 0; i < ContainerNames.length; ++i )
								external.globals( 'ConferenceSessionPool' ).Containers.Item( ContainerNames[i] ).LanguageUpdate();
						}
					}
				}
			}
			/* Bookmarked conference room
			 */
			else if ( id >= 22000 && id < 23000 )
			{
				var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
				dom.async = false;
				dom.resolveExternals = false;
				dom.load( external.globals( 'usersdir' ) + 'Profiles\\' + external.globals( 'cfg' ).Item( 'username' ) + '@' + external.globals( 'cfg' ).Item( 'server' ) + '\\bookmarks.xml' );
				if ( dom.documentElement )
				{
					var BookmarkNodes = dom.documentElement.selectNodes( '/bookmarks/room[@address]' );
					if ( BookmarkNodes.length > id - 22000 )
					{
						var Address = BookmarkNodes.item( id - 22000 ).getAttribute( 'address' );
						var Password = BookmarkNodes.item( id - 22000 ).getAttribute( 'password' );
						dial_conference( Address, Password );
					}
				}
			}
			else
				warn( 'EVNT: selected menubar option ' + id );
	}
}
