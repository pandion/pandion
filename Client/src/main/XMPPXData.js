/* Helper functions for working with jabber:x:data forms
 */
function XMPPXData ()
{
	this.BuildXML	= BuildXML;
	this.RenderForm	= RenderForm;

	function RenderForm ( XMLNode, HTMLContainer, HTMLTitle, HTMLInstructions )
	{
		var Node		= null;

		if ( ( Node = XMLNode.selectSingleNode( 'title' ) ) && Node.text && HTMLTitle )
			HTMLTitle.innerText = Node.text;
		if ( ( Node = XMLNode.selectSingleNode( 'instructions' ) ) && Node.text && HTMLInstructions )
			HTMLInstructions.innerText = Node.text;

		var Fields = XMLNode.selectNodes( 'field[@var and @type]' );
		for ( var i = 0; i < Fields.length; ++i )
		{
			var Field = Fields.item(i);

			var Wrapper			= document.createElement( 'DIV' );
			Wrapper.className	= 'x-data-field';

			switch ( Field.getAttribute( 'type' ) )
			{
				case 'text-single':
					var elem		= document.createElement( 'DIV' );
					elem.innerText	= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					if ( elem.innerText.charAt( elem.innerText.length - 1 ) != ':' )
						elem.innerText += ':';
					Wrapper.appendChild( elem );
					var value		= Field.selectSingleNode( 'value' );
					value			= value ? value.text : '';
					elem			= document.createElement( 'INPUT' );
					elem.type		= 'text';
					elem.className	= 'x-data-text-single';
					elem.name		= Field.getAttribute( 'var' );
					elem.value		= value;
					Wrapper.appendChild( elem );
				break;
				case 'text-private':
					var elem		= document.createElement( 'DIV' );
					elem.innerText	= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					if ( elem.innerText.charAt( elem.innerText.length - 1 ) != ':' )
						elem.innerText += ':';
					Wrapper.appendChild( elem );
					var value		= Field.selectSingleNode( 'value' );
					value			= value ? value.text : '';
					elem			= document.createElement( 'INPUT' );
					elem.type		= 'password';
					elem.className	= 'x-data-text-private';
					elem.name		= Field.getAttribute( 'var' );
					elem.value		= value;
					Wrapper.appendChild( elem );
				break;
				case 'text-multi':
					var elem		= document.createElement( 'DIV' );
					elem.innerText	= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					if ( elem.innerText.charAt( elem.innerText.length - 1 ) != ':' )
						elem.innerText += ':';
					Wrapper.appendChild( elem );
					var value		= Field.selectSingleNode( 'value' );
					value			= value ? value.text : '';
					elem			= document.createElement( 'TEXTAREA' );
					elem.className	= 'x-data-text-multi';
					elem.name		= Field.getAttribute( 'var' );
					elem.value		= value;
					Wrapper.appendChild( elem );
				break;
				case 'list-single':
					var elem		= document.createElement( 'DIV' );
					elem.innerText	= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					if ( elem.innerText.charAt( elem.innerText.length - 1 ) != ':' )
						elem.innerText += ':';
					Wrapper.appendChild( elem );
					var value		= Field.selectSingleNode( 'value' );
					value			= value ? value.text : '';
					elem			= document.createElement( 'SELECT' );
					elem.className	= 'x-data-list-single';
					elem.name		= Field.getAttribute( 'var' );
					var opts		= Field.selectNodes( 'option' );
					for ( var j = 0; j < opts.length; ++j )
					{
						var opt			= opts.item( j );
						var val			= opt.selectSingleNode( 'value' );
						val				= val ? val.text : '';
						var option		= document.createElement( 'OPTION' );
						option.text		= opt.getAttribute( 'label' ) ? opt.getAttribute( 'label' ) : val;
						option.value	= val;
						elem.add( option );
						if ( val == value )
							elem.selectedIndex = j;
					}
					Wrapper.appendChild( elem );
				break;
				case 'list-multi':
					var elem			= document.createElement( 'DIV' );
					elem.innerText		= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					if ( elem.innerText.charAt( elem.innerText.length - 1 ) != ':' )
						elem.innerText += ':';
					Wrapper.appendChild( elem );
					elem				= document.createElement( '<SELECT multiple>' );
					elem.className		= 'x-data-list-multi';
					elem.name			= Field.getAttribute( 'var' );
					var opts			= Field.selectNodes( 'option' );
					for ( var j = 0; j < opts.length; ++j )
					{
						var opt			= opts.item( j );
						var val			= opt.selectSingleNode( 'value' );
						val				= val ? val.text : '';
						var option		= document.createElement( 'OPTION' );
						option.text		= opt.getAttribute( 'label' ) ? opt.getAttribute( 'label' ) : val;
						option.value	= val;
						option.selected	= Field.selectSingleNode( 'value[. = "' + val + '"]' ) ? true : false;
						elem.options.add( option );
					}
					elem.size = Math.min( 10, elem.options.length );
					Wrapper.appendChild( elem );
				break;
				case 'boolean':
					var elem		= document.createElement( '<INPUT type=checkbox' + ( Field.selectSingleNode( 'value[.="1"]' ) ? ' checked' : '' ) + '>' );
					elem.className	= 'x-data-boolean';
					elem.name		= Field.getAttribute( 'var' );
					elem.id			= 'form-xdata-' + ( Math.random() ).toString();
					Wrapper.appendChild( elem );
					var label		= document.createElement( 'LABEL' );
					label.htmlFor	= elem.id;
					label.innerText	= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					Wrapper.appendChild( label );
				break;
				case 'fixed':
					var value		= Field.selectSingleNode( 'value' );
					value			= value ? value.text : ( Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : '' );
					var elem		= document.createElement( 'DIV' );
					elem.className	= 'x-data-fixed';
					elem.innerText	= value;
					Wrapper.appendChild( elem );
				break;
				case 'hidden':
					var value				= Field.selectSingleNode( 'value' );
					value					= value ? value.text : '';
					var elem				= document.createElement( 'INPUT' );
					elem.type				= 'hidden';
					elem.className			= 'x-data-hidden';
					elem.name				= Field.getAttribute( 'var' );
					elem.value				= value;
					Wrapper.style.display	= 'none';
					Wrapper.appendChild( elem );
				break;
				case 'jid-single':
					var elem		= document.createElement( 'DIV' );
					elem.innerText	= Field.getAttribute( 'label' ) ? Field.getAttribute( 'label' ) : MakeReadable( Field.getAttribute( 'var' ) );
					if ( elem.innerText.charAt( elem.innerText.length - 1 ) != ':' )
						elem.innerText += ':';
					Wrapper.appendChild( elem );
					var value		= Field.selectSingleNode( 'value' );
					value			= value ? value.text : '';
					elem			= document.createElement( 'INPUT' );
					elem.className	= 'x-data-jid-single';
					elem.type		= 'text';
					elem.name		= Field.getAttribute( 'var' );
					elem.value		= value;
					Wrapper.appendChild( elem );
				break;
				case 'jid-multi':
					/* TODO: Display this somehow
					 */
				break;
			}

			if ( Field.selectSingleNode( 'desc' ) )
			{
				if ( Field.getAttribute( 'type' ) == 'fixed' )
				{
					var elem		= document.createElement( 'DIV' );
					elem.innerText	= Field.selectSingleNode( 'desc' ).text;
					Wrapper.appendChild( elem );
				}
				else
				{
					Wrapper.title = Field.selectSingleNode( 'desc' ).text;
				}
			}

			HTMLContainer.appendChild( Wrapper );
		}
	}

	function MakeReadable ( Label )
	{
		if ( Label.substr( 0, 15 ) == 'muc#roomconfig_' )
		{
			var Standardized = external.globals( 'Translator' ).Translate( 'conference-configuration', Label );
			return Standardized ? Standardized : Label;
		}
		else
			return Label;
	}

	function BuildXML ( HTMLElement, XMLNode )
	{
		var dom = new ActiveXObject( 'Msxml2.DOMDocument' );
		var Collection = HTMLElement.getElementsByTagName( 'INPUT' );
		for ( var i = 0; i < Collection.length; i++ )
		{
			var Field = dom.createElement( 'field' );
			Field.setAttribute( 'var', Collection[i].name );
			var value = dom.createElement( 'value' );
			if ( Collection[i].type == 'checkbox' )
				value.text = Collection[i].checked ? 1 : 0;
			else if ( Collection[i].value.length )
				value.text = Collection[i].value;
			Field.appendChild( value );
			XMLNode.appendChild( Field );
		}
		Collection = HTMLElement.getElementsByTagName( 'SELECT' );
		for ( var i = 0; i < Collection.length; i++ )
		{
			var Field = dom.createElement( 'field' );
			Field.setAttribute( 'var', Collection[i].name );
			for ( var j = 0; j < Collection[i].options.length; ++j )
				if ( Collection[i].options.item(j).selected )
				{
					var value = dom.createElement( 'value' );
					value.text = Collection[i].options.item(j).value;
					Field.appendChild( value );
				}
			XMLNode.appendChild( Field );
		}
		Collection = HTMLElement.getElementsByTagName( 'TEXTAREA' );
		for ( var i = 0; i < Collection.length; i++ )
		{
			var Field = dom.createElement( 'field' );
			Field.setAttribute( 'var', Collection[i].name );
			var value = dom.createElement( 'value' );
			if ( Collection[i].value.length )
				value.text = Collection[i].value;
			Field.appendChild( value );
			XMLNode.appendChild( Field );
		}
	}
}
