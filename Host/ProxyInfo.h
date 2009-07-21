#pragma once

class CProxyInfo
{
private:
	CComBSTR m_Address;
	USHORT   m_Port;
	CComBSTR m_Username;
	CComBSTR m_Password;
public:
	CProxyInfo();

	void SetAddress ( BSTR address );
	void SetPort    ( USHORT port );
	void SetUsername( BSTR username );
	void SetPassword( BSTR password );
	void GetAddress ( BSTR *address );
	USHORT GetPort  ();
	void GetUsername( BSTR *username );
	void GetPassword( BSTR *password );
};
