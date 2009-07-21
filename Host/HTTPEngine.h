#pragma once

class CPile;
class CProxyInfo;
class CListenThread;

class CHTTP :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IHTTP >
{
private:	
	CPile* m_pPile;
	CProxyInfo* m_pProxyInfo;
	CListenThread* m_pLT;
	IEventRouter* m_pER;
public:
	CHTTP();
	~CHTTP();

BEGIN_COM_MAP(CHTTP)
	COM_INTERFACE_ENTRY( IHTTP )
	COM_INTERFACE_ENTRY( IDispatch )
END_COM_MAP()

// IHTTP
	STDMETHOD(ShareFile)( BSTR filename, BSTR URL, DWORD* retCookie );
	STDMETHOD(UnShareFile)( DWORD sfCookie );
	
	STDMETHOD(Listen)( USHORT port );
	STDMETHOD(StopListening)( USHORT port );

	STDMETHOD(Get)( BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID );
	STDMETHOD(Post)( BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID );

	STDMETHOD(SetProxyInfo)( BSTR proxyAddress, USHORT proxyPort, BSTR proxyUName, BSTR proxyPass );

	STDMETHOD(Subscribe)( IDispatch* wnd );
	STDMETHOD(Unsubscribe)( IDispatch* wnd );

	STDMETHOD(GetProgress)( DWORD sessionID, DWORD* retval );
	STDMETHOD(GetLength)( DWORD sessionID, DWORD* retval );

	STDMETHOD(Abort)( DWORD sessionID );
}; 