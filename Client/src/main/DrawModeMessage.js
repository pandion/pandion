/* Filter the status message for hyperlinks
 */
function DrawModeMessage ( HTMLElement, Status, ClassName )
{
	HTMLElement.innerText = '';
	var foundA = false;

	if ( Status.indexOf( '<a href="' ) > -1 )
	{
		var Offset	= 0;
		var FoundAt	= 0;
		while ( ( FoundAt = Status.indexOf( '<a href="', Offset ) ) > -1 )
		{
			var x = (foundA ? 4 : 0);
			
			if ( Offset + x < FoundAt )
				HTMLElement.insertAdjacentText( 'beforeEnd', Status.substring( Offset + x, FoundAt ) );
		
			foundA = true;
			
			Offset 		= Status.indexOf( '</a>', FoundAt + 9 );
			if ( Offset < 0 )
				Offset  = Status.length;
			
			var fimHref = Status.indexOf('"', FoundAt + 9 );
			if ( fimHref < 0 )
				fimHref = Status.length;
			
			var href 	= Status.substring( FoundAt + 9, fimHref );			
			var texto	= Status.substring( fimHref + 2, Offset);		
			
			var Hyperlink 		= createHyperlink(href, texto, ClassName);
			HTMLElement.insertAdjacentElement( 'beforeEnd', Hyperlink );
		}
		if ( Offset + 4 < Status.length - 1 )
			HTMLElement.insertAdjacentText( 'beforeEnd', Status.substr( Offset + 4 ) );
		
		HTMLElement.insertAdjacentText( 'beforeEnd', '\n' );
	}		
	
	if ( Status.indexOf( 'http' ) > -1 && foundA == false)
	{
		var Offset	= 0;
		var FoundAt	= 0;
		while ( ( FoundAt = Status.indexOf( 'http', Offset ) ) > -1 )
		{
			if ( Offset < FoundAt )
				HTMLElement.insertAdjacentText( 'beforeEnd', Status.substring( Offset, FoundAt ) );
			
			Offset = Status.indexOf( ' ', FoundAt + 4 );
			if ( Offset < 0 )
				Offset = Status.length;
			
			var Hyperlink 		= createHyperlink(Status.substring( FoundAt, Offset ), Status.substring( FoundAt, Offset ), ClassName);
			
			HTMLElement.insertAdjacentElement( 'beforeEnd', Hyperlink );
		}
		if ( Offset < Status.length - 1 )
			HTMLElement.insertAdjacentText( 'beforeEnd', Status.substr( Offset ) );
		HTMLElement.insertAdjacentText( 'beforeEnd', '\n' );
	}
	else if (foundA == false)
		HTMLElement.innerText = Status + '\n';
}
/** Create Hyperlinks
*/
function createHyperlink(href, txt, ClassName)
{
	var Hyperlink		= document.createElement( 'A' );
	Hyperlink.className	= ClassName;
	Hyperlink.href		= href;
	Hyperlink.innerText = txt;
	Hyperlink.attachEvent(	'onclick',
									function ()
									{
										external.windows( 'MainWindow' ).Do( 'dial_webbrowser', event.srcElement.href );
										event.returnValue	= false;
										event.cancelBubble	= true;
									}
								);
	Hyperlink.attachEvent(	'onmouseup',
									function ()
									{
										event.returnValue	= false;
										event.cancelBubble	= true;
										if ( event.button != 2 )
											return true;
										var Menu = external.newPopupMenu;
										Menu.AddItem( true, false, false, true, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_link_open' ), 1 );
										Menu.AddItem( true, false, false, false, 0, external.globals( 'Translator' ).Translate( 'main', 'cl_link_copy' ), 2 );
										Menu.Show( event.screenX, event.screenY, external.globals( 'Translator' ).Direction );
										switch ( Menu.Choice )
										{
											case 1:
												external.windows( 'MainWindow' ).Do( 'dial_webbrowser', event.srcElement.href );
											break;
											case 2:
												window.clipboardData.setData( 'Text', event.srcElement.href );
											break;
										}
									}
								);
			
	return Hyperlink;
}