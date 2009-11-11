/* Context menu for items in our contact list
 */
function mousemenu ( jid, res, group )
{
	event.cancelBubble = true;
	var connected = external.globals( 'XMPPConnected' );
	var Online = external.globals( 'ClientRoster' ).Items.Exists( jid ) && external.globals( 'ClientRoster' ).Items( jid ).Resources.Count;
	var menu = external.newPopupMenu;
	menu.AddItem( true,		false, false, true,  0, external.globals( 'Translator' ).Translate( 'main', 'menu_message' ),		1 );
//	menu.AddItem( Online,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_filetransfer' ),	10 );

	var submenu_rooms = external.newPopupMenu;
	submenu_rooms.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_room_create' ), 100 );
	var rooms = ( new VBArray( external.globals( 'ConferenceSessionPool' ).Trackers.Keys() ) ).toArray().sort();
	if ( rooms.length )
		submenu_rooms.AddSeparator();
	for ( var i = 0; i < rooms.length; ++i )
		submenu_rooms.AddItem( true, false, false, false, 0, rooms[i].substr( 0, 30 ), 101 + i );
	menu.AddItem( Online && res.length > 0, false, false, false, submenu_rooms.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_room_invite' ), 0 );

	var plugindata = new Array();
	var pluginname = new Array();
	var keys = ( new VBArray( external.globals( 'ClientPluginContainer' ).Plugins.Keys() ) ).toArray();
	for ( var i = 0; i < keys.length; ++i )
		if ( external.globals( 'ClientPluginContainer' ).Plugins( keys[i] ).Menus.Exists( 'roster' ) )
		{
			var pluginoptions = external.globals( 'ClientPluginContainer' ).Plugins( keys[i] ).Menus( 'roster' );
			for ( var j = 0; j < pluginoptions.length; ++j )
			{
				menu.AddItem( pluginoptions[j].OnlineOnly ? Online : true, false, false, false, 0, pluginoptions[j].Text, 400 + plugindata.length );
				plugindata.push( pluginoptions[j] );
				pluginname.push( external.globals( 'ClientPluginContainer' ).Plugins( keys[i] ) );
			}
		}

	menu.AddSeparator();
	menu.AddItem( connected,	false,										false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_rename' ),	6 );
	menu.AddItem( true,			external.globals( 'block' ).Exists( jid ),	false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_block' ),	7 );
	menu.AddItem( connected,	false,										false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_delete' ),	8 );

	var groups = new Array();
	keys = ( new VBArray( external.globals( 'ClientRoster' ).Groups.Keys() ) ).toArray();
	for ( var i = 0; i < keys.length; i++ )
		if ( ! external.globals( 'ClientRoster' ).Groups( keys[i] ).Items.Exists( jid ) )
			groups.push( keys[i] );
	groups = groups.sort();

	var submenu_moveto = external.newPopupMenu;
	submenu_moveto.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_group_create' ), 200 );
	if ( groups.length )
		submenu_moveto.AddSeparator();
	for ( var i = 0; i < groups.length; ++i )
		submenu_moveto.AddItem( true, false, false, false, 0, groups[i].substr( 0, 30 ), 201 + i );
	menu.AddItem( connected, false, false, false, submenu_moveto.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_group_move' ), 0 );

	var submenu_copyto = external.newPopupMenu;
	submenu_copyto.AddItem( connected, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_group_create' ), 300 );
	if ( groups.length )
		submenu_copyto.AddSeparator();
	for ( var i = 0; i < groups.length; ++i )
		submenu_copyto.AddItem( true, false, false, false, 0, groups[i].substr( 0, 30 ), 301 + i );
	menu.AddItem( connected, false, false, false, submenu_copyto.Handle, external.globals( 'Translator' ).Translate( 'main', 'menu_group_copy' ), 0 );

	menu.AddItem( connected,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_request' ), 9 );
	menu.AddSeparator();
	menu.AddItem( true,			false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_history' ), 4 );
	menu.AddItem( connected,	false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'menu_profile' ), 5 );

	menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );

	if ( ! external.globals( 'ClientRoster' ).Items.Exists( jid ) )
		return;

	switch ( menu.Choice )
	{
		case   1: dial_chat( jid + '/' + res );												break;
		case   4: dial_history( jid );														break;
		case   5: dial_userinfo( jid + '/' + res );											break;
		case   6: external.globals( 'ClientRoster' ).Items( jid ).ChangeName();				break;
		case   7: dial_block( jid );														break;
		case   8: external.globals( 'ClientRoster' ).Items( jid ).Purge( false, group );	break;
		case   9: external.globals( 'ClientRoster' ).Items( jid ).ReRequest();				break;
//		case  10: dial_file( jid + '/' + res );												break;
		case 100: dial_conference_create( jid );											break;
		case 200: dial_group_create( jid, group );											break;
		case 300: dial_group_create( jid, '' );												break;

		default:
			if ( menu.Choice > 100 && menu.Choice - 101 < rooms.length )
			{
				if ( external.globals( 'ConferenceSessionPool' ).Trackers.Exists( rooms[ menu.Choice - 101 ] ) )
					dial_conference_invite( external.globals( 'ConferenceSessionPool' ).Trackers( rooms[ menu.Choice - 101 ] ), jid, '' );
			}
			else if ( menu.Choice > 200 && menu.Choice - 201 < groups.length )
				external.globals( 'ClientRoster' ).Items( jid ).CC( group, groups[ menu.Choice - 201 ] );
			else if ( menu.Choice > 300 && menu.Choice - 301 < groups.length )
				external.globals( 'ClientRoster' ).Items( jid ).CC( '', groups[ menu.Choice - 301 ] );
			else if ( menu.Choice >= 400 && menu.Choice - 401 < plugindata.length )
			{
				var context = new ActiveXObject( 'Scripting.Dictionary' );
				context.Add( 'address',		jid );
				context.Add( 'resource',	res );
				context.Add( 'group',		group );
				context.Add( 'name',		'onMenu' );
				context.Add( 'menuname',	'roster' );
				context.Add( 'plugin',		pluginname[ menu.Choice - 400 ].Id );
				context.Add( 'menuvalue',	plugindata[ menu.Choice - 400 ].Value );
				external.globals( 'ClientPluginContainer' ).DispatchEvent( context );
			}
	}
}
