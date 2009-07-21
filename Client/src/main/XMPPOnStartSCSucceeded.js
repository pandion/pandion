function XMPPOnStartSCSucceeded ()
{
	// TODO: fix a strange bug in connection code so we don't need this timeout workaround
	window.setTimeout( 'XMPPOnConnected();', 50 );
}
