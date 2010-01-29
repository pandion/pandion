function sound_play ( file, override )
{
	if ( external.globals( 'soundallowed' ) && ( override || ( new Date() ).getTime() >= external.globals( 'soundtime' ) ) )
	{
		external.PlaySound( file );
		external.globals( 'soundtime' ) = ( new Date() ).getTime() + 500;
	}
}
