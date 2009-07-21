function XMPPOnStartTLSSucceeded ()
{
	external.globals( 'XMPPEncryption' ) = 'tls';
	document.getElementById( 'mode-secure'	).style.display = 'inline';
	// TODO: fix a strange bug in connection code so we don't need this timeout workaround
	window.setTimeout( 'XMPPOnConnected();', 50 );
}
