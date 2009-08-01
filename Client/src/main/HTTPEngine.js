function HTTPOnListening ( localPort )
{
	warn( 'EVNT: HTTPEngine listening at port ' + localPort );
	external.globals( 'HTTPEnginePort' ) = localPort;
}
