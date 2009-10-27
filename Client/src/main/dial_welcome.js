/*
	This file is part of Pandion instant messenger
	Copyright (c) 2009 Sebastiaan Deckers
	License: GNU General Public License version 3 or later
*/
function dial_welcome ()
{
	if ( external.windows.Exists( 'welcome' ) )
		external.windows( 'welcome' ).focus();
	else
		with ( external.createWindow( 'welcome', external.globals( 'cwd' ) + 'welcome.html', window ) )
		{
			setTitle( external.globals( 'Translator' ).Translate( 'main', 'wnd_welcome', [ external.globals( 'softwarename' ) ] ) );
			setIcon( external.globals( 'cwd' ) + '..\\images\\brand\\default.ico' );
			resizeable( false );
			showMinBox( true );
			setSize( 750, 500 );
			setPos( ( screen.availWidth - 750 ) / 2, ( screen.availHeight - 500 ) / 2 );
		}
}
