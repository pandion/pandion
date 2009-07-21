function sound_play ( file, override )
{
	if ( external.globals( 'soundallowed' ) && ( override || ( new Date() ).getTime() >= external.globals( 'soundtime' ) ) )
	{
		document.getElementById( 'snd' ).src = file;
		external.globals( 'soundtime' ) = ( new Date() ).getTime() + 4000;
	}
}
