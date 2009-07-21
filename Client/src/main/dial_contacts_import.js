function dial_contacts_import ()
{
	var Dialog, Location;
	try
	{
		Dialog			= external.ComCtrl.ComDlg.GetFileName;
		Dialog.Flags	= 2048;
		Dialog.Filter	= external.globals( 'Translator' ).Translate( 'main', 'import_files' ) + ' (*.xml, *.blt, *.ctt, *.txt)|*.xml;*.blt;*.ctt;*.txt|' + external.globals( 'softwarename' ) + ' (*.xml)|*.xml|AIM (*.blt)|*.blt|MSN (*.ctt)|*.ctt|Gadu-Gadu (*.txt)|*.txt';
		Location		= Dialog.DisplayOpen();
	}
	catch ( e )
	{
		return false;
	}

	var Extension = Location.substr( Location.lastIndexOf( '.' ) + 1 );

	/* XML
	 */
	if ( Extension == 'xml' )
	{
		var dom					= new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.async				= false;
		dom.resolveExternals	= false;
		dom.load( Location );

		if ( dom.documentElement )
		{
			var AddressList			= new Array();
			var ContactsToImport	= new Array();
			var ImportContacts		= dom.documentElement.selectNodes( '/query[@xmlns="jabber:iq:roster"]/item[@jid]' );

			if ( ImportContacts.length )
			{
				for ( var i = 0; i < ImportContacts.length; ++i )
				{
					var Address = new XMPPAddress( ImportContacts.item(i).getAttribute( 'jid' ) );
					if ( ! external.globals( 'ClientRoster' ).Items.Exists( Address.ShortAddress() ) )
					{
						AddressList.push( Address.ShortAddress() );
						ContactsToImport.push( ImportContacts.item(i) );
					}
				}

				if ( ContactsToImport.length )
				{
					if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_confirm', [ AddressList.join( ', ' ) ] ), external.globals( 'softwarename' ), 4 | 32 ) )
					{
						for ( var i = 0; i < ContactsToImport.length; ++i )
						{
							var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
							dom.loadXML( '<iq type="set"><query/></iq>' );
							dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
							dom.documentElement.firstChild.appendChild( ContactsToImport[i] );
							dom.documentElement.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
							warn( 'SENT: ' + dom.xml );
							external.XMPP.SendXML( dom );

							var Address = new XMPPAddress( ContactsToImport[i].getAttribute( 'jid' ) );
							dom.loadXML( '<presence type="subscribe"/>' );
							dom.documentElement.setAttribute( 'to', Address.ShortAddress() );
							warn( 'SENT: ' + dom.xml );
							external.XMPP.SendXML( dom );
						}
					}
				}
				else
					external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_no_new' ), external.globals( 'softwarename' ), 0 | 48 );
			}
			else
				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_invalid' ), external.globals( 'softwarename' ), 0 | 48 );
		}
		else
			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_invalid' ), external.globals( 'softwarename' ), 0 | 48 );
	}
	/* AIM
	 */
	else if ( Extension == 'blt' )
	{
		var Group				= '';
		var Transport			= '';
		var AddressList			= new Array();
		var ContactsToImport	= new ActiveXObject( 'Scripting.Dictionary' );

		var Transports = external.globals( 'ClientServices' ).FindByOptions( 0x0001 | 0x0008 );
		for ( var i = 0; i < Transports.length; ++i )
			if ( Transports[i].TransportMode )
			{
				Transport = '@' + Transports[i].JID.toLowerCase();
				break;
			}

		if ( Transport.length )
		{
			var File = external.File( Location );
			while ( ! File.AtEnd )
			{
				var Line = File.ReadLine();
				if ( Line == 'Buddy {' )
					break;
			}
			while ( ! File.AtEnd )
			{
				var Line = File.ReadLine();
				if ( Line == ' list {' )
					break;
			}
			if ( ! File.AtEnd )
			{
				while ( ! File.AtEnd )
				{
					var Line = File.ReadLine();
					if ( Line.length > 4 && Line.charAt( 2 ) != ' ' )
					{
						/* Group begin
						 */
						if ( Line.charAt( Line.length - 1 ) == '{' )
						{
							var Name = Line.substr( 2, Line.length - 4 );
							if ( Name.charAt( 0 ) == '"' )
								Name = Name.substr( 1, Name.length - 2 );
							Group = Name;
						}
						/* One-liner groups
						 * Possibly multiple on the same line
						 * First one is the group
						 */
						else
						{
							var Line = Line.substr( 2 );
							var LineChopped = new Array();
							for ( var Position = 0; Position < Line.length; ++Position )
							{
								while ( Line.charAt( Position ) == ' ' )
									++Position;
								var MatchLength = 1;
								if ( Line.charAt( Position ) == '"' )
								{
									++Position;
									while ( Position + MatchLength < Line.length && ( Line.charAt( Position + MatchLength ) != '"' || Line.charAt( Position + MatchLength - 1 ) == '\\' ) )
										++MatchLength;
								}
								else
									while ( Line.charAt( Position + MatchLength ) != ' ' && Position + MatchLength < Line.length )
										++MatchLength;
								var Name = Line.substr( Position, MatchLength ).replace( /\\"/gm, '"' );
								Position += MatchLength;
								AddressList.push( Name );
								Name = Name.replace( /\s/gm, '' ).toLowerCase();
								LineChopped.push( Name );
							}
							for ( var i = 1; i < LineChopped.length; ++i )
							{
								if ( ! external.globals( 'ClientRoster' ).Items.Exists( LineChopped[i] + Transport ) )
								{
									if ( ContactsToImport.Exists( LineChopped[i] ) )
										ContactsToImport( LineChopped[i] ).push( LineChopped[0] );
									else
										ContactsToImport.Add( LineChopped[i], [ LineChopped[0] ] );
								}
							}
						}
					}
					/* Contacts in a group
					 */
					else if ( Line.length > 4 && Line.charAt( 3 ) != ' ' )
					{
						var Name = Line.substr( 3 );
						if ( Name.charAt( Name.length - 1 ) == '{' )
							Name = Name.substr( 0, Name.length - 2 );
						if ( Name.charAt( 0 ) == '"' )
							Name = Name.substr( 1, Name.length - 2 );
						AddressList.push( Name );
						Name = Name.replace( /\s/gm, '' ).toLowerCase();
						if ( ! external.globals( 'ClientRoster' ).Items.Exists( Name + Transport ) )
						{
							if ( ContactsToImport.Exists( Name ) )
								ContactsToImport( Name ).push( Group );
							else
								ContactsToImport.Add( Name, [ Group ] );
						}
					}
					/* Group end
					 */
					else if ( Line == '  }' )
						Group = '';
					/* Contact list end
					 */
					else if ( Line == ' }' )
						break;
				}

				if ( ContactsToImport.Count )
				{
					if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_confirm', [ AddressList.join( ', ' ) ] ), external.globals( 'softwarename' ), 4 | 32 ) )
					{
						var ContactNames = ( new VBArray( ContactsToImport.Keys() ) ).toArray();
						for ( var i = 0; i < ContactNames.length; ++i )
						{
							var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
							dom.loadXML( '<iq type="set"><query><item/></query></iq>' );
							dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
							dom.documentElement.firstChild.firstChild.setAttribute( 'jid', ContactNames[i] + Transport );
							dom.documentElement.firstChild.firstChild.setAttribute( 'name', ContactNames[i] );
							for ( var j = 0; j < ContactsToImport( ContactNames[i] ).length; ++j )
							{
								var Group	= dom.createElement( 'group' );
								Group.text	= ContactsToImport( ContactNames[i] )[j];
								dom.documentElement.firstChild.firstChild.appendChild( Group );
							}
							dom.documentElement.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
							warn( 'SENT: ' + dom.xml );
							external.XMPP.SendXML( dom );

							dom.loadXML( '<presence type="subscribe"/>' );
							dom.documentElement.setAttribute( 'to', ContactNames[i] + Transport );
							warn( 'SENT: ' + dom.xml );
							external.XMPP.SendXML( dom );
						}
					}
				}
				else
					external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_no_new' ), external.globals( 'softwarename' ), 0 | 48 );
			}
			else
				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_invalid' ), external.globals( 'softwarename' ), 0 | 48 );
		}
		else
			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_aim_not_registered' ), external.globals( 'softwarename' ), 0 | 48 );
	}
	/* MSN
	 */
	else if ( Extension == 'ctt' )
	{
		var Transport		= '';
		var Transports		= external.globals( 'ClientServices' ).FindByOptions( 0x0001 | 0x0002 );
		for ( var i = 0; i < Transports.length; ++i )
			if ( Transports[i].TransportMode )
			{
				Transport = '@' + Transports[i].JID.toLowerCase();
				break;
			}

		if ( Transport.length )
		{
			var dom					= new ActiveXObject( 'MSXML2.DOMDocument' );
			dom.async				= false;
			dom.resolveExternals	= false;
			dom.load( Location );

			if ( dom.documentElement )
			{
				var AddressList			= new Array();
				var ContactsToImport	= new ActiveXObject( 'Scripting.Dictionary' );
				var ImportContacts		= dom.documentElement.selectNodes( '/messenger/service/contactlist/contact' );

				if ( ImportContacts.length )
				{
					for ( var i = 0; i < ImportContacts.length; ++i )
					{
						var Address = new XMPPAddress( ImportContacts.item(i).text );
						if ( Address.User.length && Address.Host.length && ! external.globals( 'ClientRoster' ).Items.Exists( Address.User + '%' + Address.Host + Transport ) )
						{
							AddressList.push( Address.ShortAddress() );
							if ( ! ContactsToImport.Exists( Address.User + '%' + Address.Host ) )
								ContactsToImport.Add( Address.User + '%' + Address.Host, null );
						}
					}

					if ( ContactsToImport.Count )
					{
						if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_confirm', [ AddressList.join( ', ' ) ] ), external.globals( 'softwarename' ), 4 | 32 ) )
						{
							var ContactNames = ( new VBArray( ContactsToImport.Keys() ) ).toArray();
							for ( var i = 0; i < ContactNames.length; ++i )
							{
								var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
								dom.loadXML( '<iq type="set"><query><item/></query></iq>' );
								dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
								dom.documentElement.firstChild.firstChild.setAttribute( 'jid', ContactNames[i] + Transport );
								dom.documentElement.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
								warn( 'SENT: ' + dom.xml );
								external.XMPP.SendXML( dom );

								dom.loadXML( '<presence type="subscribe"/>' );
								dom.documentElement.setAttribute( 'to', ContactNames[i] + Transport );
								warn( 'SENT: ' + dom.xml );
								external.XMPP.SendXML( dom );
							}
						}
					}
					else
						external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_no_new' ), external.globals( 'softwarename' ), 0 | 48 );
				}
				else
					external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_invalid' ), external.globals( 'softwarename' ), 0 | 48 );
			}
			else
				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_invalid' ), external.globals( 'softwarename' ), 0 | 48 );
		}
		else
			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_msn_not_registered' ), external.globals( 'softwarename' ), 0 | 48 );
	}
	/* Gadu-Gadu
	 */
	else if ( Extension == 'txt' )
	{
		var Group				= '';
		var Transport			= '';
		var AddressList			= new Array();
		var ContactsToImport	= new ActiveXObject( 'Scripting.Dictionary' );

		var Transports = external.globals( 'ClientServices' ).FindByOptions( 0x0001 | 0x0800 );
		for ( var i = 0; i < Transports.length; ++i )
			if ( Transports[i].TransportMode )
			{
				Transport = '@' + Transports[i].JID.toLowerCase();
				break;
			}

		if ( Transport.length )
		{
			var File = external.File( Location );
			if ( ! File.AtEnd )
			{
				while ( ! File.AtEnd )
				{
					var Record = File.ReadLine().split( ';' );
					if ( Record.length >= 6 && Record[3].length )
					{
						var Name	= Record[3];
						var Groups	= new Array();
						for ( var i = 5; i < Record.length; ++i )
						{
							if ( Record[i].match( /^\d{2,}$/ ) )
							{
								var Account = Record[i];
								AddressList.push( Name );
								if ( ! external.globals( 'ClientRoster' ).Items.Exists( Name + Transport ) && ! ContactsToImport.Exists( Account ) )
									ContactsToImport.Add( Account, [ Name, Groups ] );
								break;
							}
							else if ( Record[i].length )
								Groups.push( Record[i] );
						}
					}
				}

				if ( ContactsToImport.Count )
				{
					if ( 6 == external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_confirm', [ AddressList.join( ', ' ) ] ), external.globals( 'softwarename' ), 4 | 32 ) )
					{
						var Accounts = ( new VBArray( ContactsToImport.Keys() ) ).toArray();
						for ( var i = 0; i < Accounts.length; ++i )
						{
							var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
							dom.loadXML( '<iq type="set"><query><item/></query></iq>' );
							dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
							dom.documentElement.firstChild.firstChild.setAttribute( 'jid', Accounts[i] + Transport );
							dom.documentElement.firstChild.firstChild.setAttribute( 'name', ContactsToImport( Accounts[i] )[0] );
							for ( var j = 0; j < ContactsToImport( Accounts[i] )[1].length; ++j )
							{
								var Group	= dom.createElement( 'group' );
								Group.text	= ContactsToImport( Accounts[i] )[1][j];
								dom.documentElement.firstChild.firstChild.appendChild( Group );
							}
							dom.documentElement.firstChild.setAttribute( 'xmlns', 'jabber:iq:roster' );
							warn( 'SENT: ' + dom.xml );
							external.XMPP.SendXML( dom );

							dom.loadXML( '<presence type="subscribe"/>' );
							dom.documentElement.setAttribute( 'to', Accounts[i] + Transport );
							warn( 'SENT: ' + dom.xml );
							external.XMPP.SendXML( dom );
						}
					}
				}
				else
					external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_no_new' ), external.globals( 'softwarename' ), 0 | 48 );
			}
			else
				external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_invalid' ), external.globals( 'softwarename' ), 0 | 48 );
		}
		else
			external.wnd.messageBox( true, external.globals( 'Translator' ).Translate( 'main', 'import_gadu_not_registered' ), external.globals( 'softwarename' ), 0 | 48 );
	}
}
