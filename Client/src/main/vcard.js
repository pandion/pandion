/* Parses vCards, stores the values in a Dictionary, and spits them back out as an XML DOM object.
 */
function VCARD ()
{
	this.FromIQ = FromIQ;
	this.IntoXMLDOM = IntoXMLDOM;
	this.get = get;
	this.set = set;

	var a = new ActiveXObject( 'Scripting.Dictionary' );
	a.Add( 'fn',			'' );
	a.Add( 'given',			'' );
	a.Add( 'family',		'' );
	a.Add( 'middle',		'' );
	a.Add( 'nickname',		'' );
	a.Add( 'url',			'' );
	a.Add( 'bday',			'' );
	a.Add( 'location',		'' );
	a.Add( 'age',			'' );
	a.Add( 'gender',		'' );
	a.Add( 'maritalstatus',	'' );
	a.Add( 'workcell',		'' );
	a.Add( 'homecell',		'' );
	a.Add( 'orgname',		'' );
	a.Add( 'orgunit',		'' );
	a.Add( 'title',			'' );
	a.Add( 'role',			'' );
	a.Add( 'homevoice',		'' );
	a.Add( 'homefax',		'' );
	a.Add( 'homemsg',		'' );
	a.Add( 'homeextadd',	'' );
	a.Add( 'homestreet',	'' );
	a.Add( 'homelocality',	'' );
	a.Add( 'homeregion',	'' );
	a.Add( 'homepcode',		'' );
	a.Add( 'homecountry',	'' );
	a.Add( 'workvoice',		'' );
	a.Add( 'workfax',		'' );
	a.Add( 'workmsg',		'' );
	a.Add( 'workextadd',	'' );
	a.Add( 'workstreet',	'' );
	a.Add( 'worklocality',	'' );
	a.Add( 'workregion',	'' );
	a.Add( 'workpcode',		'' );
	a.Add( 'workcountry',	'' );
	a.Add( 'email1',		'' );
	a.Add( 'email2',		'' );
	a.Add( 'desc',			'' );
	a.Add( 'jabberid',		'' );
	this.data = a;
	delete a;

	function get ( name )
	{
		return this.data.Exists( name ) ? this.data.Item( name ) : '';
	}

	function set ( name, value )
	{
		return this.data.Exists( name ) ? this.data.Item( name ) = value : this.data.Add( name, value );
	}

	/* This array maps the XML elements to the Dictionary values.
	 */
	var list = [
		'FN',									'fn',
		'NICKNAME',								'nickname',
		'URL',									'url',
		'BDAY',									'bday',
		'LOCATION',								'location',
		'AGE',									'age',
		'GENDER',								'gender',
		'MARITALSTATUS',						'maritalstatus',
		'WORKCELL',								'workcell',
		'HOMECELL',								'homecell',
		'TITLE',								'title',
		'ROLE',									'role',

		'N/GIVEN',								'given',
		'N/FAMILY',								'family',
		'N/MIDDLE',								'middle',

		'ORG/ORGNAME',							'orgname',
		'ORG/ORGUNIT',							'orgunit',

		'ADR[WORK]/EXTADD',						'workextadd',
		'ADR[WORK]/STREET',						'workstreet',
		'ADR[WORK]/LOCALITY',					'worklocality',
		'ADR[WORK]/REGION',						'workregion',
		'ADR[WORK]/PCODE',						'workpcode',
		'ADR[WORK]/COUNTRY',					'workcountry',

		'ADR[HOME or not(WORK)]/EXTADD',		'homeextadd',
		'ADR[HOME or not(WORK)]/STREET',		'homestreet',
		'ADR[HOME or not(WORK)]/LOCALITY',		'homelocality',
		'ADR[HOME or not(WORK)]/REGION',		'homeregion',
		'ADR[HOME or not(WORK)]/PCODE',			'homepcode',
		'ADR[HOME or not(WORK)]/COUNTRY',		'homecountry',

		'TEL[WORK and VOICE]/NUMBER',			'workvoice',
		'TEL[WORK and FAX]/NUMBER',				'workfax',
		'TEL[WORK and MSG]/NUMBER',				'workmsg',

		'TEL[HOME and VOICE]/NUMBER',			'homevoice',
		'TEL[HOME and FAX]/NUMBER',				'homefax',
		'TEL[HOME and MSG]/NUMBER',				'homemsg',

		'EMAIL[PREF]/USERID',					'email1',
		'EMAIL[INTERNET and not(PREF)]/USERID',	'email2',

		'DESC',									'desc',
		'JABBERID',								'jabberid'
	];

	function FromIQ ( iq )
	{
		var vcard = iq.XMLDOM.selectSingleNode( '/iq/*[@xmlns = "vcard-temp"]' );
		if ( iq.Type != 'result' || ! vcard )
			return;
		for ( var i = 0; i + 1 < list.length; i += 2 )
		{
			var node = vcard.selectSingleNode( list[i] );
			if ( node )
				this.data.Item( list[i+1] ) = node.text;
		}
	}

	function IntoXMLDOM ()
	{
		var dom = new ActiveXObject( 'MSXML2.DOMDocument' );
		dom.loadXML( '<iq><vCard xmlns="vcard-temp"><FN/><N><GIVEN/><FAMILY/><MIDDLE/></N><NICKNAME/><URL/><BDAY/><AGE/><GENDER/><MARITALSTATUS/><LOCATION/><WORKCELL/><HOMECELL/><ORG><ORGNAME/><ORGUNIT/></ORG><TITLE/><ROLE/><TEL><HOME/><VOICE/><NUMBER/></TEL><TEL><HOME/><FAX/><NUMBER/></TEL><TEL><HOME/><MSG/><NUMBER/></TEL><ADR><HOME/><EXTADD/><STREET/><LOCALITY/><REGION/><PCODE/><COUNTRY/></ADR><TEL><WORK/><VOICE/><NUMBER/></TEL><TEL><WORK/><FAX/><NUMBER/></TEL><TEL><WORK/><MSG/><NUMBER/></TEL><ADR><WORK/><EXTADD/><STREET/><LOCALITY/><REGION/><PCODE/><COUNTRY/></ADR><EMAIL><INTERNET/><PREF/><USERID/></EMAIL><EMAIL><INTERNET/><USERID/></EMAIL><DESC/><JABBERID/></vCard></iq>' );
		dom.documentElement.setAttribute( 'id', 'sd' + ( ++external.globals( 'uniqueid' ) ) );
		vcard = dom.firstChild.firstChild;
		for ( var i = 0; i + 1 < list.length; i += 2 )
		{
			try
			{
				if ( this.data.Item( list[i+1] ).length )
					vcard.selectSingleNode( list[i] ).appendChild( dom.createTextNode( this.data.Item( list[i+1] ) ) );
			}
			catch ( e )
			{
				/* Errors occur when the tag does not exist, so the data object doesn't change
				 */
			}
		}
		return dom;
	}
}
