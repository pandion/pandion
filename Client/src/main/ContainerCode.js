/* Filter the status message for hyperlinks
 */
function DrawModeMessage ( HTMLElement, Status, ClassName )
{
	HTMLElement.innerText = '';
	if ( Status.indexOf( 'http://' ) > -1 )
	{
		var Offset	= 0;
		var FoundAt	= 0;
		while ( ( FoundAt = Status.indexOf( 'http://', Offset ) ) > -1 )
		{
			if ( Offset < FoundAt )
				HTMLElement.insertAdjacentText( 'beforeEnd', Status.substring( Offset, FoundAt ) );
			Offset = Status.indexOf( ' ', FoundAt + 7 );
			if ( Offset < 0 )
				Offset = Status.length;
			var Hyperlink		= document.createElement( 'A' );
			Hyperlink.className	= ClassName;
			Hyperlink.href		= Hyperlink.innerText = Status.substring( FoundAt, Offset );
			Hyperlink.attachEvent(
				'onclick',
				function ()
				{
					external.windows( 'MainWindow' ).Do( 'dial_webbrowser', event.srcElement.href );
					event.returnValue	= false;
					event.cancelBubble	= true;
				}
			);
			Hyperlink.attachEvent(
				'onmouseup',
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
			HTMLElement.insertAdjacentElement( 'beforeEnd', Hyperlink );
		}
		if ( Offset < Status.length - 1 )
			HTMLElement.insertAdjacentText( 'beforeEnd', Status.substr( Offset ) );
		HTMLElement.insertAdjacentText( 'beforeEnd', '\n' );
	}
	else
		HTMLElement.innerText = Status + '\n';
}

/* Flash the icon in the tab button
 */
function FlashTab ( Address, Times )
{
	if ( gContainer.Trackers.Exists( Address ) )
	{
		var Tracker	= gContainer.Trackers.Item( Address );
		var Name	= Tracker.HTMLButton.children.item(0).style;
		if ( Tracker.IsActive )
		{
			Tracker.IsFlashing	= false;
			Name.visibility		= 'visible';
		}
		else if ( Times != 0 )
		{
			Times--;
			Tracker.IsFlashing = true;
			Tracker.HTMLButton.children.item(1).style.fontStyle = 'italic';
			Name.visibility = Name.visibility == 'hidden' ? 'visible' : 'hidden';
			setTimeout( 'FlashTab( "' + Address + '", ' + Times + ' )', 500 );
		}
		else
			Tracker.IsFlashing = false;
	}
}

/* Display emoticons in a palette
 */
function ChooseEmoticons ()
{
	var AddressHost = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Address.Host;
	var EmoticonStyle = external.globals( 'cfg' ).Item( 'emoticonset' );
	if ( external.globals( 'ClientServices' ).Services.Exists( AddressHost ) && ( external.globals( 'ClientServices' ).Services.Item( AddressHost ).Options & 0x0001 ) )
		switch ( external.globals( 'ClientServices' ).Services.Item( AddressHost ).Options & 0x881E )
		{
			case 0x0002: EmoticonStyle = 'msn_messenger';	break;
			case 0x0004: EmoticonStyle = 'icq';				break;
			case 0x0008: EmoticonStyle = 'aim';				break;
			case 0x0010: EmoticonStyle = 'yahoo_messenger';	break;
			case 0x0800: EmoticonStyle = 'gadu-gadu';		break;
			case 0x8000: EmoticonStyle = 'tlen.pl';			break;
		}
	else if ( AddressHost == 'jabber.wp.pl' )
		EmoticonStyle = 'wpkontakt';

	if ( ! external.FileExists( external.globals( 'usersdir' ) + 'Emoticons\\' + EmoticonStyle + '\\icondef.xml' ) )
		EmoticonStyle = external.globals( 'cfg' ).Item( 'emoticonset' );

	var Language = external.globals( 'language' );
	if ( ! ( Language.length < 3 || Language.indexOf( 'i-' ) == 0 || Language.indexOf( 'x-' ) == 0 ) )
		Language = Language.substr( 0, Language.indexOf( '-' ) );

	var w				= 34;
	var h				= 34;
	var W				= 9;
	var H				= 5;
	var ExpandWidth		= 16;
	var BrowseHeight	= 24;
	var Popup			= window.createPopup();
	var Subset			= external.globals( 'EmoticonCache' ).GetSubset( EmoticonStyle, [ 'default', Language ] );
	var Strings			= new Array();
	var Graphics		= new Array();
	var Singularity		= new ActiveXObject( 'Scripting.Dictionary' );

	if ( Subset )
	{
		/* Expand palette to show all emoticons
		 */
		var ShowExpand = Subset.Style.Actions.length > W * H;
		if ( Subset.Style.Actions.length > W * H && external.globals( 'cfg' ).Item( 'emoticonviewall' ).toString() == 'true' )
		{
			W = Math.round( Math.pow( Subset.Style.Actions.length * W / H, 0.5 ) );
			H = 100;
		}

		/* Try some hardcoded icons
		 */
		var Guesses = new Array( ':-)', ':)', ':-D', ';-)', ':-P', ':-O', ':-(', ':\'(', ':*)', ':-$', ':-/', ':-|', ':-s', ':-x', '>:-)', 'B-)', '8-D', '::heart::', '::phone::', '::home::' );
		for ( var i = 0; i < Guesses.length; ++i )
			if ( Subset.StringToAction.Exists( Guesses[i] ) )
			{
				var Action = Subset.Style.Actions[ Subset.StringToAction.Item( Guesses[i] ) ];
				if ( Action.Graphic && ! Singularity.Exists( Action.Graphic ) )
				{
					Strings.push( Guesses[i] );
					Graphics.push( Action.Graphic );
					Singularity.Add( Action.Graphic, null );
				}
			}

		/* Fill up with random icons
		 */
		var SubsetStrings = ( new VBArray( Subset.StringToAction.Keys() ) ).toArray();
		for ( var i = 0; i < SubsetStrings.length && Strings.length < W * H; ++i )
		{
			var Action = Subset.Style.Actions[ Subset.StringToAction.Item( SubsetStrings[i] ) ];
			if ( Action.Graphic && ! Singularity.Exists( Action.Graphic ) )
			{
				Strings.push( SubsetStrings[i] );
				Graphics.push( Action.Graphic );
				Singularity.Add( Action.Graphic, null );
			}
		}

		/* Crop the palette if there are not enough icons
		 */
		H = Math.ceil( Strings.length / W );
		if ( H == 1 )
			W = Strings.length + 1;

		/* Draw icons in the palette window
		 */
		for ( var i = 0; i < Strings.length; ++i )
		{
			var X				= i % W;
			var Y				= ( i - X ) / W;
			var Icon			= Popup.document.createElement( '<DIV onmouseover="this.style.borderColor = \'highlight\'" onmouseout="this.style.borderColor = \'window\'">' );
			Icon.title			= Strings[i];
			with ( Icon.style )
			{
				top				= ( Y * ( h + 1 ) ) + 'px';
				left			= ( X * ( w + 1 ) ) + 'px';
				position		= 'absolute';
				width			= w + 'px';
				height			= h + 'px';
				border			= '1x solid window';
				background		= 'window no-repeat center center';
				backgroundImage	= 'url( ' + external.globals( 'usersdir' ) + 'Emoticons\\' + EmoticonStyle + '\\' + Graphics[i] + ' )';
			}
			Popup.document.body.insertAdjacentElement( 'beforeEnd', Icon );
		}

		/* Fill the gap between the last icon and the settings button
		 */
		if ( Strings.length < H * W )
		{
			var Filler			= Popup.document.createElement( 'DIV' );
			with ( Filler.style )
			{
				var X			= Strings.length % W;
				var Y			= ( Strings.length - X ) / W;
				top				= ( Y * ( h + 1 ) ) + 'px';
				left			= ( X * ( w + 1 ) ) + 'px';
				position		= 'absolute';
				width			= ( ( w + 1 ) * ( W - X ) - 1 ) + 'px';
				height			= h + 'px';
				textAlign		= 'center';
				color			= 'windowtext';
				border			= '1x solid window';
				background		= 'window no-repeat center center';
			}
			Popup.document.body.insertAdjacentElement( 'beforeEnd', Filler );
		}

		/* Expand button
		 */
		if ( ShowExpand )
		{
			var Expand			= Popup.document.createElement( '<DIV onmouseover="this.style.borderColor = \'highlight\'; this.style.backgroundImage = \'url( ../images/misc/arrow-\' + ( external.globals( \'cfg\' )( \'emoticonviewall\' ).toString() == \'true\' ? \'we\' : \'ea\' ) + \'st-dark.gif )\';" onmouseout="this.style.borderColor = \'window\'; this.style.backgroundImage = \'url( ../images/misc/arrow-\' + ( external.globals( \'cfg\' )( \'emoticonviewall\' ).toString() == \'true\' ? \'we\' : \'ea\' ) + \'st-light.gif )\';">' );
			Expand.title		= external.globals( 'Translator' ).Translate( 'chat-container', external.globals( 'cfg' ).Item( 'emoticonviewall' ).toString() == 'true' ? 'palette_compact' : 'palette_expand' );
			Expand.attachEvent(
				'onclick',
				function ( event )
				{
					event.cancelBubble = true;
					external.globals( 'cfg' ).Item( 'emoticonviewall' ) = ! ( external.globals( 'cfg' ).Item( 'emoticonviewall' ).toString() == 'true' );
					Popup.hide();
					document.getElementById( 'btn-emoticons' ).click();
				}
			);
			with ( Expand.style )
			{
				fontFamily		= 'Arial';
				fontSize		= '10px';
				paddingTop		= ( ( H * ( h + 1 ) - 1 - 10 ) / 2 ) + 'px';
				top				= '0px';
				left			= ( W * ( w + 1 ) ) + 'px';
				position		= 'absolute';
				width			= ExpandWidth + 'px';
				height			= ( H * ( h + 1 ) - 1 ) + 'px';
				textAlign		= 'center';
				color			= 'windowtext';
				border			= '1x solid window';
				background		= 'window no-repeat center center';
				backgroundImage	= 'url( ../images/misc/arrow-' + ( external.globals( 'cfg' ).Item( 'emoticonviewall' ).toString() == 'true' ? 'we' : 'ea' ) + 'st-light.gif )';
			}
			Popup.document.body.insertAdjacentElement( 'beforeEnd', Expand );
		}
		else
			ExpandWidth = -1;

		/* Browse emoticon styles button
		 */
		var Browse			= Popup.document.createElement( '<DIV onmouseover="this.style.backgroundColor = \'highlight\'; this.style.color = \'highlighttext\';" onmouseout="this.style.backgroundColor = \'window\'; this.style.color = \'windowtext\';">' );
		Browse.innerText	= external.globals( 'Translator' ).Translate( 'chat-container', 'palette_browse' );
		Browse.attachEvent(
			'onclick',
			function ( event )
			{
				event.cancelBubble = true;
				external.windows( 'MainWindow' ).Do( 'dial_emoticon_list', null );
				Popup.hide();
				Popup.document.body.innerHTML = '';
				if ( ! document.getElementById( 'send-text' ).disabled )
					document.getElementById( 'send-text' ).focus();
			}
		);
		with ( Browse.style )
		{
			top				= ( ( Y + 1 ) * ( h + 1 ) ) + 'px';
			left			= '0px';
			position		= 'absolute';
			width			= ( W * ( w + 1 ) ) + ExpandWidth + 'px';
			height			= BrowseHeight + 'px';
			cursor			= 'default';
			color			= 'windowtext';
			border			= '2x solid window';
			backgroundColor	= 'window';
			fontFamily		= 'Tahoma';
			fontSize		= '11px';
			paddingLeft		= '8px';
			paddingTop		= Math.floor( ( BrowseHeight - 18 ) / 2 ) + 'px';
		}
		Popup.document.body.insertAdjacentElement( 'beforeEnd', Browse );

		Popup.document.body.attachEvent(
			'onselectstart',
			function ( event )
			{
				return false;
			}
		);
		Popup.document.body.attachEvent(
			'onclick',
			function ( event )
			{
				if ( event.srcElement.tagName == 'DIV' )
					document.getElementById( 'send-text' ).value += event.srcElement.title;
				if ( ! document.getElementById( 'send-text' ).disabled )
					document.getElementById( 'send-text' ).focus();
				Popup.hide();
				Popup.document.body.innerHTML = '';
			}
		);
		Popup.document.body.style.border = '1px solid threeddarkshadow';
		Popup.document.body.style.backgroundColor = 'lavender';
		Popup.document.Popup = Popup;
		var LTR = !external.globals("Translator").Direction;
		var PopupWidth  = W * ( w + 1 ) + 1 + ExpandWidth + 1;
		var PopupHeight = H * ( h + 1 ) + 1 + BrowseHeight + 1;
		var PopupX = LTR ? (event.x - event.offsetX - 2) : (external.wnd.width - PopupWidth - 10);
		var PopupY = event.y - event.offsetY - PopupHeight - 3;
		var EdgeRight = external.wnd.left + event.x + PopupWidth;
		var EdgeTop = external.wnd.top + event.y - PopupHeight;
		if ( ! external.IsRectOnMonitor( EdgeTop, EdgeRight, EdgeTop + 1, EdgeRight - 1 ) )
			PopupX -= PopupWidth - 100;
		Popup.show(
			PopupX,
			PopupY,
			PopupWidth,
			PopupHeight,
			document.body
		);
	}
	else
	{
		external.windows( 'MainWindow' ).Do( 'dial_emoticon_list', null );
	}
}

function ChooseBackground ()
{
	var h						= 32;
	var w						= 192;
	var Popup					= window.createPopup();
	var BackgroudPath			= external.globals( 'usersdir' ) + 'Backgrounds\\';
	var BackgroudCounter		= 0;
	var BackgroundDirectories	= new VBArray( external.Directory.ListDirs( BackgroudPath ) ).toArray();

	for ( var i = 0; i < BackgroundDirectories.length; ++i )
	{
		var Path = BackgroudPath + BackgroundDirectories[i].Name + '\\';
		if ( external.FileExists( Path + 'preview.png' ) )
		{
			var Icon			= Popup.document.createElement( '<IMG onmouseover="this.style.borderColor = \'highlight\'" onmouseout="this.style.borderColor = \'window\'">' );
			Icon.BackgroundName	= BackgroundDirectories[i].Name;
			Icon.src			= Path + 'preview.png';
			Icon.height			= h;
			Icon.width			= w;
			with ( Icon.style )
			{
				top				= ( BackgroudCounter * ( h + 6 ) + 2 ) + 'px';
				left			= '2px';
				position		= 'absolute';
				borderWidth		= '3px 15px';
				borderStyle		= 'solid';
				borderColor		= 'window';
			}
			++BackgroudCounter;
			Popup.document.body.insertAdjacentElement( 'beforeEnd', Icon );
		}
	}

	if ( BackgroudCounter )
	{
		var Element			= Popup.document.createElement( '<DIV \
			onmouseover="with ( this.style ) {	borderWidth = \'4px 16px\';	marginLeft = \'0px\';	marginTop = \'0px\'; width = \'222px\'; height = \'38px\'; }" \
			onmouseout="with ( this.style ) {	borderWidth = \'1px\';		marginLeft = \'15px\';	marginTop = \'3px\'; width = \'192px\'; height = \'32px\'; }" \
		/>' );
		Element.innerText	= external.globals( 'Translator' ).Translate( 'chat-container', 'background_no' );
		with ( Element.style )
		{
			height			= h;
			width			= w;
			top				= ( BackgroudCounter * ( h + 6 ) + 2 ) + 'px';
			left			= '2px';
			position		= 'absolute';
			marginLeft		= '15px';
			marginTop		= '3px';
			borderWidth		= '1px';
			borderStyle		= 'solid';
			borderColor		= 'highlight';
			cursor			= 'default';
			paddingTop		= '8px';
			paddingLeft		= '8px';
			fontFamily		= 'Tahoma';
			fontSize		= '11px';
			overflow		= 'hidden';
			textOverflow	= 'ellipsis';
		}
		Element.attachEvent(
			'onclick',
			function ( event )
			{
				var Tracker = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress );
				if ( Tracker && Tracker.Background.length )
				{
					external.globals( 'cfg' ).Item( 'background' ) = '';
					Tracker.Background			= '';
					Tracker.BackgroundLoading	= false
					var ShortAddress			= Tracker.Address.ShortAddress();
					if ( typeof Tracker.Password == 'undefined' )
						gContainer.SessionPool.TrackersLoading.Add( ShortAddress, Tracker.Address );
					else
						gContainer.SessionPool.TrackersLoading.Add( ShortAddress, { 'address': Tracker.Address, 'password': Tracker.Password } );
					gContainer.SessionPool.DeleteTracker( Tracker );
					gContainer.Trackers.Remove( ShortAddress );

					Tracker.Deactivate();
					Tracker.Reloading	= true;
					Tracker.HTMLArea.removeNode( true );
					Tracker.HTMLArea	= document.createElement( 'IFRAME' );
					with ( Tracker.HTMLArea )
					{
						id				= 'iframe-' + ShortAddress;
						style.display	= 'none';
						height			= '100%';
						width			= '100%';
						src				= 'chat-messages.html';
					}
					Tracker.HTMLArea.SessionTracker = Tracker;
					gContainer.HTMLTabArea.insertAdjacentElement( 'beforeEnd', Tracker.HTMLArea );
					document.frames( 'iframe-' + ShortAddress ).SessionTracker = Tracker;
				}
			}
		);
		Popup.document.body.insertAdjacentElement( 'beforeEnd', Element );
		++BackgroudCounter;

		Element				= Popup.document.createElement( '<DIV \
			onmouseover="with ( this.style ) {	borderWidth = \'4px 16px\';	marginLeft = \'0px\';	marginTop = \'0px\'; width = \'222px\'; height = \'38px\'; }" \
			onmouseout="with ( this.style ) {	borderWidth = \'1px\';		marginLeft = \'15px\';	marginTop = \'3px\'; width = \'192px\'; height = \'32px\'; }" \
		/>' );
		Element.innerText	= external.globals( 'Translator' ).Translate( 'chat-container', 'background_list' );
		with ( Element.style )
		{
			height			= h;
			width			= w;
			top				= ( BackgroudCounter * ( h + 6 ) + 2 ) + 'px';
			left			= '2px';
			position		= 'absolute';
			marginLeft		= '15px';
			marginTop		= '3px';
			borderWidth		= '1px';
			borderStyle		= 'solid';
			borderColor		= 'highlight';
			cursor			= 'default';
			paddingTop		= '8px';
			paddingLeft		= '8px';
			fontFamily		= 'Tahoma';
			fontSize		= '11px';
			overflow		= 'hidden';
			textOverflow	= 'ellipsis';
		}
		Element.attachEvent(
			'onclick',
			function ( event )
			{
				external.windows( 'MainWindow' ).Do( 'dial_background_list', null );
			}
		);
		Popup.document.body.insertAdjacentElement( 'beforeEnd', Element );
		++BackgroudCounter;

		Popup.document.body.attachEvent(
			'onselectstart',
			function ( event )
			{
				return false;
			}
		);
		Popup.document.body.attachEvent(
			'ondragstart',
			function ( event )
			{
				return false;
			}
		);
		Popup.document.body.attachEvent(
			'onclick',
			function ( event )
			{
				if ( event.srcElement.tagName == 'IMG' )
				{
					var Tracker = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress );
					if ( Tracker && Tracker.Background != event.srcElement.BackgroundName )
					{
						external.globals( 'cfg' ).Item( 'background' ) = event.srcElement.BackgroundName;
						Tracker.Background			= event.srcElement.BackgroundName;
						Tracker.BackgroundLoading	= false;
						var ShortAddress			= Tracker.Address.ShortAddress();
						if ( typeof Tracker.Password == 'undefined' )
							gContainer.SessionPool.TrackersLoading.Add( ShortAddress, Tracker.Address );
						else
							gContainer.SessionPool.TrackersLoading.Add( ShortAddress, { 'address': Tracker.Address, 'password': Tracker.Password } );
						gContainer.SessionPool.DeleteTracker( Tracker );
						gContainer.Trackers.Remove( ShortAddress );

						Tracker.Deactivate();
						Tracker.Reloading	= true;
						Tracker.HTMLArea.removeNode( true );
						Tracker.HTMLArea	= document.createElement( 'IFRAME' );
						with ( Tracker.HTMLArea )
						{
							id				= 'iframe-' + ShortAddress;
							style.display	= 'none';
							height			= '100%';
							width			= '100%';
							src				= external.globals( 'usersdir' ) + 'Backgrounds\\' + event.srcElement.BackgroundName + '\\index.html';
						}
						Tracker.HTMLArea.SessionTracker = Tracker;
						gContainer.HTMLTabArea.insertAdjacentElement( 'beforeEnd', Tracker.HTMLArea );
						document.frames( 'iframe-' + ShortAddress ).SessionTracker = Tracker;
					}
				}
				if ( ! window.parent.document.getElementById( 'send-text' ).disabled )
					window.parent.document.getElementById( 'send-text' ).focus();
				Popup.hide();
			}
		);
		Popup.document.body.style.border = '1px solid threeddarkshadow';
		Popup.document.body.style.backgroundColor = 'window';
		Popup.document.Popup = Popup;
		var PopupWidth  = ( w + 30 ) + 6;
		var PopupHeight = ( h +  6 ) * BackgroudCounter + 6;
		Popup.show(
			event.x - event.offsetX - 2,
			event.y - event.offsetY - PopupHeight - 3,
			PopupWidth,
			PopupHeight,
			document.body
		);
	}
	else
	{
		external.windows( 'MainWindow' ).Do( 'dial_background_list', null );
	}
}

/* Handle keyboard shortcuts
 */
function Keyboard ( EventData )
{
	var k = EventData.keyCode;

	var Function = 0x00;
	if ( EventData.altKey )		Function |= 0x01;
	if ( EventData.ctrlKey )	Function |= 0x02;
	if ( EventData.shiftKey )	Function |= 0x04;

	/* Activate next or previous tab
	 * Ctrl (+ Shift) + Tab
	 */
	if ( k == 9 && Function & 0x02 )
	{
		EventData.returnValue = false;
		var Buttons = gContainer.HTMLTabBar.children;
		for ( var i = 0; i < Buttons.length; ++i )
			if ( Buttons(i).SessionTracker.IsActive )
				break;
		if ( i == Buttons.length )
			i = ( Function & 0x04 ) ? 0 : Buttons.length - 1;
		if ( Function & 0x04 )
			Buttons( i == 0 ? Buttons.length - 1 : i-1 ).SessionTracker.Activate( true );
		else
			Buttons( i == Buttons.length - 1 ? 0 : i+1 ).SessionTracker.Activate( true );
	}

	/* Activate next tab
	 * Ctrl + PgUp
	 */
	else if ( k == 33 && Function == 0x02 )
	{
		EventData.returnValue = false;
		var Buttons = gContainer.HTMLTabBar.children;
		for ( var i = 0; i < Buttons.length; ++i )
			if ( Buttons(i).SessionTracker.IsActive )
				break;
		if ( i == Buttons.length )
			i = 0;
		Buttons( i == 0 ? Buttons.length - 1 : i-1 ).SessionTracker.Activate( true );
	}

	/* Activate previous tab
	 * Ctrl + PgDn
	 */
	else if ( k == 34 && Function == 0x02 )
	{
		EventData.returnValue = false;
		var Buttons = gContainer.HTMLTabBar.children;
		for ( var i = 0; i < Buttons.length; ++i )
			if ( Buttons(i).SessionTracker.IsActive )
				break;
		if ( i == Buttons.length )
			i = Buttons.length - 1;
		Buttons( i == Buttons.length - 1 ? 0 : i+1 ).SessionTracker.Activate( true );
	}

	/* Activate previous tab
	 * Ctrl + 1...9
	 */
	else if ( k >= 97 && k <= 105 && Function == 0x02 )
	{
		EventData.returnValue = false;
		var i = k - 97;
		var Buttons = gContainer.HTMLTabBar.children;
		if ( i < Buttons.length && ! Buttons(i).SessionTracker.IsActive )
			Buttons(i).SessionTracker.Activate( true );
	}

	/* Activate previous tab
	 * Ctrl + 1...9
	 */
	else if ( k >= 49 && k <= 57 && Function == 0x02 )
	{
		EventData.returnValue = false;
		var i = k - 49;
		var Buttons = gContainer.HTMLTabBar.children;
		if ( i < Buttons.length && ! Buttons(i).SessionTracker.IsActive )
			Buttons(i).SessionTracker.Activate( true );
	}

	/* Close currently active tab
	 * Esc
	 * Ctrl + F4
	 * Ctrl + W
	 */
	else if ( k == 27 || ( k == 115 && Function == 0x02 ) || ( k == 87 && Function == 0x02 ) )
	{
		if ( gContainer.Trackers.Exists( gContainer.ActiveTrackerAddress ) )
		{
			EventData.returnValue = false;
			if ( gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).IsActive )
				gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Close();
			else
				gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Activate();
		}
	}

	/* Undo closing of last tab
	 * Ctrl + Shift + T
	 */
	else if ( k == 84 && Function & 0x02 && Function & 0x04 )
	{
		if ( gContainer.SessionPool.RecentTrackers.length )
			external.wnd.params[0].dial_chat( gContainer.SessionPool.RecentTrackers.pop() );
	}

	/* Open a new tab
	 * Ctrl + T
	 */
	else if ( k == 84 && Function == 0x02 )
	{
		TabAddActivate();
	}

	/* Manual
	 * F1
	 */
	else if ( k == 112 )
	{
		if ( external.globals( 'helpmanual' ).length )
			external.windows( 'MainWindow' ).Do( 'dial_webbrowser', external.globals( 'ClientPluginContainer' ).ParseURL( external.globals( 'helpmanual' ) ) );
	}

	/* Network Traffic
	 * F12
	 */
	else if ( k == 123 )
	{
		external.wnd.params[0].dial_console();
	}

	/* Give focus to input area
	 */
	else if ( ( ! Function || Function == 0x04 ) && k != 9 && k != 32 )
	{
		if ( EventData.srcElement.id != 'send-text' && ! document.getElementById( 'send-text' ).disabled )
		{
			document.getElementById( 'send-text' ).focus();
			document.getElementById( 'send-text' ).value = document.getElementById( 'send-text' ).value;
			setTimeout( DisableButton, 1 );
		}
	}
}

/* Handle keyboard input in the text input area
 */
function OnTypeInput ( HTMLElement )
{
	external.globals("idle").keepalive();
	if ( event.keyCode == 13 && ! event.shiftKey )
	{
		if ( ! document.getElementById( 'btn-send' ).disabled )
			SendMessage();
		return false;
	}
}

/* Stores reference to input resizing tracker
 */
var gResizeInterval = null;

/* Set hooks and begin the size tracker interval
 */
function ResizeStartTracking ()
{
	if ( ! gResizeInterval )
	{
		if ( ! gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Occupants )
			document.getElementById( 'send-text' ).detachEvent( 'onpropertychange', Typing );
		document.attachEvent( 'onmouseup', ResizeStopTracking );
		ResizePosition();
		gResizeInterval = setInterval( 'ResizePosition()', 10 );
		document.frames( 'iframe-' + gContainer.ActiveTrackerAddress ).document.attachEvent( 'onmouseup', ResizeStopTracking );
		document.body.style.cursor = 'n-resize';
		document.getElementById( 'toolbar-wrapper' ).childNodes(0).style.backgroundColor = 'black';
		document.getElementById( 'toolbar-wrapper' ).childNodes(1).style.cursor = 'n-resize';
		document.getElementById( 'toolbar-wrapper' ).childNodes(2).style.cursor = 'n-resize';
		document.getElementById( 'toolbar-wrapper' ).childNodes(3).style.cursor = 'n-resize';
		document.getElementById( 'mode-bar' ).style.cursor = 'n-resize';
		document.getElementById( 'send-text' ).style.cursor = 'n-resize';
		document.frames( 'iframe-' + gContainer.ActiveTrackerAddress ).document.body.style.cursor = 'n-resize';
	}
}

/* Remove hooks and stop the size tracker interval
 */
function ResizeStopTracking ()
{
	if ( gResizeInterval )
	{
		clearTimeout( gResizeInterval );
		gResizeInterval = null;
		document.detachEvent( 'onmouseup', ResizeStopTracking );
		document.frames( 'iframe-' + gContainer.ActiveTrackerAddress ).document.detachEvent( 'onmouseup', ResizeStopTracking );
		document.body.style.cursor = '';
		document.getElementById( 'toolbar-wrapper' ).childNodes(0).style.backgroundColor = '';
		document.getElementById( 'toolbar-wrapper' ).childNodes(1).style.cursor = '';
		document.getElementById( 'toolbar-wrapper' ).childNodes(2).style.cursor = '';
		document.getElementById( 'toolbar-wrapper' ).childNodes(3).style.cursor = '';
		document.getElementById( 'mode-bar' ).style.cursor = gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Occupants ? 'default' : 'hand';
		document.getElementById( 'send-text' ).style.cursor = '';
		document.frames( 'iframe-' + gContainer.ActiveTrackerAddress ).document.body.style.cursor = '';
		if ( ! gContainer.Trackers.Item( gContainer.ActiveTrackerAddress ).Occupants )
			document.getElementById( 'send-text' ).attachEvent( 'onpropertychange', Typing );
	}
}

/* Set the input area size according to the current mouse location within certain constraints
 */
function ResizePosition ()
{
	var l = external.wnd.left + 4;
	var t = external.wnd.top + 54;
	var r = external.wnd.left + external.wnd.width;
	var b = external.wnd.top + external.wnd.height + 50;

	if ( external.cursorX > l && external.cursorX < r && external.cursorY > t && external.cursorY < b )
	{
		var InputRow = document.getElementById( 'send-text' ).parentNode.parentNode;
		var TabbarRow = document.getElementById( 'tab-bar-row' );
		var InputBottomY = external.wnd.top + external.wnd.height - TabbarRow.height + 8;
		var MaxHeight = InputBottomY - ( external.wnd.top + 200 );
		external.globals( 'cfg' ).Item( 'textinputheight' ) = Math.max( 60, Math.min( MaxHeight, InputBottomY - external.cursorY ) );
		InputRow.height = external.globals( 'cfg' ).Item( 'textinputheight' );
	}
	else
		ResizeStopTracking();
}

/* Only allow text selections for textarea and input fields
 */
function SelectionFilter ()
{
	return gResizeInterval == null && event.srcElement.tagName == 'TEXTAREA' || event.srcElement.tagName == 'INPUT';
}

/* Ensure the input size area does not stay too large when the window is resized
 */
function ResizeCheck ()
{
	var Height = parseInt( external.globals( 'cfg' ).Item( 'textinputheight' ), 10 );
	var TabbarRow = document.getElementById( 'tab-bar-row' );
	var InputBottomY = external.wnd.top + external.wnd.height - TabbarRow.height + 50;
	var MaxHeight = InputBottomY - ( external.wnd.top + 200 );
	if ( Height > MaxHeight )
	{
		external.globals( 'cfg' ).Item( 'textinputheight' ) = Math.max( 60, Math.min( MaxHeight, InputBottomY - external.cursorY ) );
		document.getElementById( 'send-text' ).parentNode.parentNode.height = external.globals( 'cfg' ).Item( 'textinputheight' );
	}
}

/* Draw hover effect
 */
function ToolbarButtonMouseOver ( Button )
{
	if ( ! gResizeInterval )
		Button.className = 'toolbar-button-over';
}

/* Hide hover effect
 */
function ToolbarButtonMouseOut ( Button )
{
	if ( ! gResizeInterval )
		Button.className = 'toolbar-button';
}

/* Toggle button state
 */
function DisableButton ()
{
	document.getElementById( 'btn-send' ).disabled = ( /^\s*$/ ).test( document.getElementById( 'send-text' ).value );
}
