function XMPPOnStream ( ReceivedXML )
{
	warn( 'RECV: ' + ReceivedXML.xml );

	/* Server supports stream compression
	 */
	if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/compression[@xmlns="http://jabber.org/features/compress"]/method[ . = "zlib" ]' ) )
	{
		var Str = '<compress xmlns="http://jabber.org/protocol/compress"><method>zlib</method></compress>';
		warn( 'SENT: ' + Str );
		external.XMPP.SendText( Str );
	}

	/* Server is ready to start stream compression
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/compressed[@xmlns="http://jabber.org/protocol/compress"]' ) )
	{
		external.XMPP.StartSC();
		//XMPPOnConnected();
	}

	/* Error during compression
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/failure[@xmlns="http://jabber.org/protocol/compress"]' ) )
	{
		OnLoginAbort();
	}

	/* Server supports stream encryption
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/starttls[@xmlns="urn:ietf:params:xml:ns:xmpp-tls"]' ) 
		&& external.globals( 'encryption' ) != 'none' )
	{
		var Str = '<starttls xmlns="urn:ietf:params:xml:ns:xmpp-tls"/>';
		warn( 'SENT: ' + Str );
		external.XMPP.SendText( Str );
	}

	/* Server is ready to start stream encryption handshake
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/proceed[@xmlns="urn:ietf:params:xml:ns:xmpp-tls"]' ) )
	{
		external.XMPP.StartTLS();
	}

	/* Error during encryption
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/failure[@xmlns="urn:ietf:params:xml:ns:xmpp-tls"]' ) )
	{
		OnLoginAbort();
	}

	/* Authentication
	 */
	else if ( ( external.globals( 'encryption' ) != 'tls' || external.globals( 'XMPPEncryption' ) == 'tls' )
		&& ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' )
			|| ReceivedXML.documentElement.selectSingleNode( '/stream:features/auth[@xmlns="http://jabber.org/features/iq-auth"]' )
			|| ( ReceivedXML.documentElement.selectSingleNode( '/stream:features' )
				&& ! ReceivedXML.documentElement.childNodes.length
			)
		)
	)
	{
		/* Request challenge if the server supports GSSAPI, GSS-SPNEGO, or NTLM mechanism
		 */
		if ( external.globals( 'sspiserver' ).length || external.globals( 'authentication' ) == 'ntlm' )
		{
			if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]/mechanism[ . = "GSSAPI" ]' ) )
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<auth xmlns="urn:ietf:params:xml:ns:xmpp-sasl" mechanism="GSSAPI"/>' );
				external.SASL.SSPIReset();
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
			else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]/mechanism[ . = "GSS-SPNEGO" ]' ) )
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<auth xmlns="urn:ietf:params:xml:ns:xmpp-sasl" mechanism="GSS-SPNEGO"/>' );
				external.SASL.SSPIReset();
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
			else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]/mechanism[ . = "NTLM" ]' ) )
			{
				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<auth xmlns="urn:ietf:params:xml:ns:xmpp-sasl" mechanism="NTLM"/>' );
				external.SASL.SSPIReset();
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );
			}
			else
			{
				var Str = '<abort xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>';
				warn( 'SENT: ' + Str );
				external.XMPP.SendText( Str );
				OnLoginAbort();
			}
		}

		/* Request MD5 challenge
		 */
		else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]/mechanism[ . = "DIGEST-MD5" ]' ) )
		{
			var Str = '<auth xmlns="urn:ietf:params:xml:ns:xmpp-sasl" mechanism="DIGEST-MD5"/>';
			warn( 'SENT: ' + Str );
			external.XMPP.SendText( Str );
		}

		/* Send plaintext credentials
		 */
		else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]/mechanism[ . = "PLAIN" ]' ) )
		{
			var User = external.globals( 'cfg' )( 'username' );
			var Addr = User + '@' + external.globals( 'cfg' )( 'server' );
			var Pass = external.globals( 'cfg' )( 'password' );

			/* Plaintext algorithm:
			 * Base64( UTF8( Addr ) + 0x00 + UTF8( User ) + 0x00 + UTF8( Pass ) )
			 */
			var Data = external.SASL.PlainGenerateResponse( Addr, User, Pass );

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<auth xmlns="urn:ietf:params:xml:ns:xmpp-sasl" mechanism="PLAIN"/>' );
			dom.documentElement.text = Data;
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		/* Use old protocol to log in
		 */
		else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/auth[@xmlns="http://jabber.org/features/iq-auth"]' )
			|| ( ReceivedXML.documentElement.selectSingleNode( '/stream:features' )
				&& ! ReceivedXML.documentElement.childNodes.length
			)
		)
		{
			var hook		= new XMPPHookIQ();
			hook.Window		= external.wnd;
			hook.Callback	= 'OnLoginAuthSend';

			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<iq type="get"><query xmlns="jabber:iq:auth"><username/></query></iq>' );
			dom.documentElement.firstChild.firstChild.text = external.globals( 'cfg' )( 'username' );
			dom.documentElement.setAttribute( 'to', external.globals( 'cfg' )( 'server' ) );
			dom.documentElement.setAttribute( 'id', hook.Id );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}

		/* Unsupported authentication mechanisms
		 */
		else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/mechanisms[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' ) )
		{
			var Str = '<abort xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>';
			warn( 'SENT: ' + Str );
			external.XMPP.SendText( Str );
			OnLoginAuthError();
		}

		/* Unsupported stream features
		 */
		else
		{
			OnLoginAbort();
		}
	}

	/* Decode authentication challenge and answer with correct credentials
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/challenge[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' ) )
	{
		if ( external.globals( 'sspiserver' ).length || external.globals( 'authentication' ) == 'ntlm'  )
		{
			var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
			dom.loadXML( '<response xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>' );
			dom.documentElement.text = external.SASL.SSPIGenerateResponse( ReceivedXML.documentElement.selectSingleNode( '/challenge[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' ).text, true );
			warn( 'SENT: ' + dom.xml );
			external.XMPP.SendXML( dom );
		}
		else
		{
			var Encoded	= ReceivedXML.documentElement.selectSingleNode( '/challenge[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' ).text;
			var Pairs	= external.Base64ToString( Encoded ).replace( /\\\n/gm, '' ).split( ',' );
			var Dataset	= new ActiveXObject( 'Scripting.Dictionary' );

			warn( 'SASL: ' + Pairs );

			for ( var i = 0; i < Pairs.length; ++i )
			{
				if ( Pairs[i].indexOf( '=' ) > 0 )
				{
					var Name	= Pairs[i].substr( 0, Pairs[i].indexOf( '=' ) );
					var Value	= Pairs[i].substr( Pairs[i].indexOf( '=' ) + 1 );

					if ( Value.charAt( 0 ) == '"' && Value.charAt( Value.length - 1 ) == '"' )
						Value	= Value.substr( 1, Value.length - 2 );

					if ( Dataset.Exists( Name ) )
						Dataset( Name ).push( Value );
					else
						Dataset.Add( Name, [ Value ] );
				}
			}

			/* Successful authentication
			 */
			if ( Dataset.Exists( 'rspauth' ) && Dataset.Count == 1 )
			{
				var Str = '<response xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>';
				warn( 'SENT: ' + Str );
				external.XMPP.SendText( Str );
			}

			/* Solve the MD5 challenge
			 */
			else if ( Dataset.Exists( 'algorithm' ) && Dataset( 'algorithm' )[0].toLowerCase() == 'md5-sess' && Dataset.Exists( 'nonce' ) && Dataset.Exists( 'qop' ) )
			{
				var Response = new ActiveXObject( 'Scripting.Dictionary' );

				Response.Add( 'username',	external.globals( 'cfg' )( 'username' ) );
				Response.Add( 'realm',		Dataset.Exists( 'realm' ) ? Dataset( 'realm' )[0] : '' );
				Response.Add( 'nonce',		Dataset( 'nonce' )[0] );
				Response.Add( 'cnonce',		external.StringToSHA1( Math.random().toString() ) );
				Response.Add( 'nc',			'00000001' );
				Response.Add( 'serv-type',	'xmpp' );
				Response.Add( 'host',		external.globals( 'cfg' )( 'server' ) );
				Response.Add( 'digest-uri',	'xmpp/' + Response( 'host' ) );
				Response.Add( 'charset',	'utf-8' );
//				Response.Add( 'authzid',	Response( 'username' ) + '@' + Response( 'host' ) );
				Response.Add( 'qop',		Dataset( 'qop' )[0] );
				Response.Add( 'password',	external.globals( 'cfg' )( 'password' ) );

				/* MD5-Session algorithm:
				var md5	= new MD5();
				var X	= Response( 'username' ) + ':' + Response( 'realm' ) + ':' + Response( 'password' );
				var Y	= md5.digest( X ); // This should be binary data instead of the hexadecimal string
				var A1	= Y + ':' + Response( 'nonce' ) + ':' + Response( 'cnonce' ); // + ':' + Response( 'authzid' )
				var A2	= 'AUTHENTICATE:' + Response( 'digest-uri' );
				var HA1	= md5.digest( A1 );
				var HA2	= md5.digest( A2 );
				var KD	= HA1 + ':' + Response( 'nonce' ) + ':' + Response( 'nc' ) + ':' + Response( 'cnonce' ) + ':' + Response( 'qop' ) + ':' + HA2;
				var Z	= md5.digest( KD );
				*/

				var Z = external.SASL.DigestGenerateResponse(
					Response( 'username'	),
					Response( 'realm'		),
					Response( 'password'	),
					Response( 'nonce'		),
					Response( 'cnonce'		),
					Response( 'digest-uri'	),
					Response( 'nc'			),
					Response( 'qop'			)
				);

				Response.Add( 'response', Z );

				var Output =	'username="'	+ Response( 'username'		) + '",' +
								'realm="'		+ Response( 'realm'			) + '",' +
								'nonce="'		+ Response( 'nonce'			) + '",' +
								'cnonce="'		+ Response( 'cnonce'		) + '",' +
								'nc='			+ Response( 'nc'			) + ','  +
								'qop='			+ Response( 'qop'			) + ','  +
								'digest-uri="'	+ Response( 'digest-uri'	) + '",' +
								'charset='		+ Response( 'charset'		) + ','  +
//								'authzid="'		+ Response( 'authzid'		) + '",' +
								'response='		+ Response( 'response'		);

				var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
				dom.loadXML( '<response xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>' );
				dom.documentElement.text = external.StringToBase64( Output );
				warn( 'SENT: ' + dom.xml );
				external.XMPP.SendXML( dom );

				warn( 'SASL: ' + Output );
			}

			/* Return an error message and abort the connection
			 */
			else
			{
				var Str = '<abort xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>';
				warn( 'SENT: ' + Str );
				external.XMPP.SendText( Str );
				OnLoginAuthError();
			}
		}
	}

	/* Re-initialise the stream
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/success[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' ) )
	{
		XMPPOnConnected();
	}

	/* Error during authentication
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/failure[@xmlns="urn:ietf:params:xml:ns:xmpp-sasl"]' ) )
	{
		OnLoginAuthError();
	}

	/* Bind a resource to the stream
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:features/bind[@xmlns="urn:ietf:params:xml:ns:xmpp-bind"]' ) )
	{
		var hook		= new XMPPHookIQ();
		hook.Window		= external.wnd;
		hook.Callback	= 'OnLoginBind';

		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		dom.loadXML( '<iq type="set"><bind xmlns="urn:ietf:params:xml:ns:xmpp-bind"><resource/></bind></iq>' );
		dom.documentElement.setAttribute( 'id', hook.Id );
		dom.documentElement.firstChild.firstChild.text = external.globals( 'cfg' )( 'resource' );
		warn( 'SENT: ' + dom.xml );
		external.XMPP.SendXML( dom );
	}

	/* Generate a different resource and auto-reconnect
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:conflict[@xmlns="urn:ietf:params:xml:ns:xmpp-streams"]' ) )
	{
		external.globals( 'cfg' )( 'resource' ) = external.globals( 'cfg' )( 'resource' ) + ' (' + Math.round( Math.random() * 0xffff ) + ')';
		var Str = '</stream:stream>';
		warn( 'SENT: ' + Str );
		external.XMPP.SendText( Str );
		external.XMPP.Disconnect();
	}

	/* Something has gone wrong, do not attempt to reconnect automatically
	 */
	else if ( ReceivedXML.documentElement.selectSingleNode( '/stream:error' ) )
	{
		if ( external.windows.Exists( 'signup' ) )
			OnLoginAbort();
		else
		{
			external.globals( 'XMPPReconnect' ) = false;
			var Str = '</stream:stream>';
			warn( 'SENT: ' + Str );
			external.XMPP.SendText( Str );
			external.XMPP.Disconnect();
		}
	}
}
