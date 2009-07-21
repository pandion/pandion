function XMPPOnStartTLSFailed ()
{
	var Str = '<failure xmlns="urn:ietf:params:xml:ns:xmpp-tls"/>';
	warn( 'SENT: ' + Str );
	external.XMPP.SendText( Str );
	OnLoginAbort();
}
