#pragma once

class CEventRouter :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IEventRouter >
{
private:
	std::vector<IPdnWnd*> m_pTargetWnds;
public:
	CEventRouter();
	~CEventRouter();

	STDMETHOD(Subscribe)( IDispatch* pDisp );
	STDMETHOD(Unsubscribe)( IDispatch* pDisp );
	STDMETHOD(UnsubscribeAll)();

	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE( CEventRouter );

	BEGIN_COM_MAP( CEventRouter )
		COM_INTERFACE_ENTRY( IEventRouter )
	END_COM_MAP()

	STDMETHOD(OnListening)( USHORT Port );
	STDMETHOD(OnAccept)( DWORD sessionID, BSTR remHost, USHORT remPort, USHORT localPort );
	STDMETHOD(OnConnected)( DWORD sessionID, BSTR Server, USHORT Port, USHORT LocalPort );

	STDMETHOD(OnGetRequest)( DWORD sessionID, DWORD fileID, BSTR URI, BSTR localPath );
	STDMETHOD(OnPostRequest)( DWORD sessionID, BSTR URI, BSTR* saveAs );

	STDMETHOD(OnTimeout)( DWORD sessionID );
	STDMETHOD(OnSockErr)( DWORD sessionID, DWORD err );
	STDMETHOD(OnHTTPErr)( DWORD sessionID, DWORD err );
	STDMETHOD(OnFileErr)( DWORD sessionID, DWORD err );

	STDMETHOD(OnTransferAborted)( DWORD sessionID );
	STDMETHOD(OnTransferComplete)( DWORD sessionID );
};