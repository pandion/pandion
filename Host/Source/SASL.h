#pragma once

#define PACKAGE_NAME	TEXT("Negotiate")

class CSASL :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< ISASL >
{
private:
	DWORD			m_dwMaxTokenSize;

	BOOL			m_fNewConversation;
	CredHandle		m_hCred;
	BOOL			m_fHaveCredHandle;
	SecHandle		m_hCtxt;
	BOOL			m_fHaveCtxtHandle;

public:
	CSASL();
	~CSASL();

BEGIN_COM_MAP(CSASL)
	COM_INTERFACE_ENTRY( IDispatch )
	COM_INTERFACE_ENTRY( ISASL )
END_COM_MAP()

	STDMETHOD(PlainGenerateResponse)( BSTR jid, BSTR username, BSTR password, BSTR *strBase64 );
	STDMETHOD(DigestGenerateResponse)( BSTR username, BSTR realm, BSTR password, BSTR nonce, BSTR cnonce, BSTR digest_uri, BSTR nc, BSTR qop, BSTR *strDigest );
	STDMETHOD(HexString)( const unsigned char *binaryData, char *hexString, int n );
	STDMETHOD(SSPIReset)();
	STDMETHOD(SSPIGenerateResponse)( BSTR Challenge, BOOL *Continue, BSTR *Response );

	void SSPIError( LPWSTR Where, LPWSTR WhenCalling, LPWSTR ErrorMessage );
};
