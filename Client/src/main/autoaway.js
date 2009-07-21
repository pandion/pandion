function autoaway ( oldX, oldY )
{
	var cfg = external.globals( 'cfg' );
	var interval = 3000;

	var newX = external.cursorX;
	var newY = external.cursorY;

	/* fullscreen app is running
	 */
/*	if ( external.Fullscreen() )
	{
		warn( 'fullscreen' );
		if ( external.globals( 'autoawaymode' ) != 4 )
			mode_new( external.globals( 'autoawaymode' ) = 4, cfg( 'lastmsg' ) );
	}
*/	/* fullscreen app is no longer running
	 */
/*	else if ( external.globals( 'autoawaymode' ) == 4 )
	{
		warn( 'no more fullscreen' );
		external.globals( 'autoawaytime' ) = external.globals( 'autoawaymode' ) = 0;
		mode_new( cfg( 'lastmode' ), cfg( 'lastmsg' ) );
	}
*/	/* not moving
	 */
/*	else*/ if ( oldX == newX && oldY == newY )
	{
		external.globals( 'autoawaytime' ) += interval;
		if ( cfg( 'lastmode' ) <= 1 )
		{
			if ( external.globals( 'autoawaytime' ) >= cfg( 'autoawaytime' ) && external.globals( 'autoawaymode' ) == 0 )
				mode_new( external.globals( 'autoawaymode' ) = 2, cfg( 'autoawaytext' ) );
			else if ( external.globals( 'autoawaytime' ) >= cfg( 'autoxawaytime' ) && external.globals( 'autoawaymode' ) == 2 )
				mode_new( external.globals( 'autoawaymode' ) = 3, cfg( 'autoxawaytext' ) );
		}
	}
	/* moving but still auto-away or auto-xaway
	 */
	else if ( external.globals( 'autoawaymode' ) == 2 || external.globals( 'autoawaymode' ) == 3 )
	{
		external.globals( 'autoawaytime' ) = external.globals( 'autoawaymode' ) = 0;
		mode_new( cfg( 'lastmode' ), cfg( 'lastmsg' ) );
	}
	/* normal movement
	 */
	else
	{
		external.globals( 'autoawaytime' ) = 0;
	}

	/* Keep checking
	 */
	external.globals( 'autoaway' ) = setTimeout( 'autoaway( ' + newX + ', ' + newY + ' )', interval );
}
