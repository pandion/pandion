function dial_service_register ( address )
{
	var type = 'default';
	if ( external.globals( 'ClientServices' ).Services.Exists( address ) )
	{
		var service = external.globals( 'ClientServices' ).Services( address );
		if ( service.Options & 0x0002 )
			type = 'msn';
		else if ( service.Options & 0x0004 )
			type = 'icq';
		else if ( service.Options & 0x0008 )
			type = 'aim';
		else if ( service.Options & 0x0010 )
			type = 'yahoo';
		else if ( service.Options & 0x0800 )
			type = 'gadu';
	}

	if ( external.windows.Exists( address + '/transport_register' ) )
		external.windows( address + '/transport_register' ).focus();
	else
		with ( external.createWindow( address + '/transport_register', external.globals( 'cwd' ) + 'transport_register.html', new Array( address, type ) ) )
		{
			resizeable( false );
			showMinBox( true );
			setIcon( external.globals( 'cwd' ) + '..\\images\\dials\\bulb.ico' );
			setSize( 450, 302 );
			setPos( ( screen.availWidth - 450 ) / 2, ( screen.availHeight - 302 ) / 2 );
			focus();
		}
}
