function XMPPOnStartTLSSucceeded ()
{
	external.globals( 'XMPPEncryption' ) = 'tls';
	document.getElementById( 'mode-secure'	).style.display = 'inline';
	XMPPOnConnected();
}
