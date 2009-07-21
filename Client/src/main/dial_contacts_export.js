function dial_contacts_export ()
{
	var Dialog, Location;
	try
	{
		Dialog			= external.ComCtrl.ComDlg.GetFileName;
		Dialog.FileName	= external.globals( 'cfg' )( 'username' ) + '@' + external.globals( 'cfg' )( 'server' ) + '.xml';
		Dialog.Flags	= 4 | 32768;
		Dialog.Filter	= 'XML (*.xml)|*.xml';
		Location		= Dialog.DisplaySave();
	}
	catch ( e )
	{
		return false;
	}

	var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
	dom.appendChild( dom.createProcessingInstruction( 'xml', 'version="1.0" encoding="UTF-8"' ) );
	dom.appendChild( dom.createElement( 'query' ) );
	var ContactAddresses = ( new VBArray( external.globals( 'ClientRoster' ).Items.Keys() ) ).toArray();
	for ( var i = 0; i < ContactAddresses.length; ++i )
	{
		var Contact = external.globals( 'ClientRoster' ).Items( ContactAddresses[i] );
		var Item = dom.createElement( 'item' );
		Item.setAttribute( 'jid',	ContactAddresses[i] );
		Item.setAttribute( 'name',	Contact.Name );
		for ( var j = 0; j < Contact.Groups.length; ++j )
		{
			Item.appendChild( dom.createTextNode( '\n\t\t' ) );
			var Group	= dom.createElement( 'group' );
			Group.text	= Contact.Groups[j];
			Item.appendChild( Group );
		}
		Item.appendChild( dom.createTextNode( '\n\t' ) );
		dom.documentElement.appendChild( dom.createTextNode( '\n\t' ) );
		dom.documentElement.appendChild( Item );
	}
	dom.documentElement.appendChild( dom.createTextNode( '\n' ) );
	dom.documentElement.setAttribute( 'xmlns', 'jabber:iq:roster' );
	dom.save( Location );
}
