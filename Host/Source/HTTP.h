#pragma once

class CFile;
class CAtlArray<CComBSTR,CComBSTR>;

class CHTTP : 
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IHTTP >
{
private:
	IHTTPServer *m_pServer;
	IHTTPClient *m_pClient;
public:
	CHTTP();
	~CHTTP();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CHTTP )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IHTTP )
	END_COM_MAP()

	/* IHTTP */
	STDMETHOD(get_Server)( IDispatch **ppServer );
	STDMETHOD(get_Client)( IDispatch **ppClient );
};

class CEventEntry
{
public:
	CEventEntry( BSTR event, BSTR handler, IDispatch *pWnd );
	~CEventEntry();

	BSTR m_Event;
	BSTR m_Handler;
	IPdnWnd *m_pWnd;
};

class CHTTPServer :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IHTTPServer >
{
private:
	CAtlArray<CEventEntry*> m_Events;
	CAtlMap<CComBSTR,CComBSTR> m_SharedFiles;

	CAtlArray<CSocket*> m_ListenSockets;
	CComMultiThreadModel::AutoCriticalSection m_cs;
	BOOL m_bContinueListening;
public:
	CHTTPServer();
	~CHTTPServer();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CHTTPServer )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IHTTPServer )
	END_COM_MAP()

	DWORD static __stdcall AsyncListenProc( void *pThis );
private:
	DWORD AsyncListen();
	VOID DispatchEvent( BSTR event, VARIANT *parameters, DWORD nParameters );
public:
	/* IHTTPServer */
	STDMETHOD(RegisterEvent)( BSTR event, BSTR handler, IDispatch *pWnd );
	STDMETHOD(Listen)( USHORT port, BOOL useTLS, USHORT *retPort );
	STDMETHOD(StopListen)( USHORT port );
	STDMETHOD(ShareFile)( BSTR path, BSTR URL );
	STDMETHOD(UnShareFile)( BSTR URL );
};

class CServerSession :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IServerSession >
{
private:
	CAtlArray<CEventEntry*> m_Events;

	CComMultiThreadModel::AutoCriticalSection m_cs;
	BOOL m_bContinueTransfer;
	HANDLE m_hThread;

	CSocket *m_pSocket;
	CFile *m_pFile;
	DWORD m_fp;
	DWORD m_BytesToSend;
	DWORD m_TotalBytesToSend;

	DWORD m_Method;
	CComBSTR m_Status;
	CComBSTR m_RequestURI;
	CAtlMap<CComBSTR,CComBSTR> m_HTTPHeaders;
	CAtlMap<CComBSTR,CComBSTR> *m_pSharedFiles;
public:
	CServerSession();
	~CServerSession();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CServerSession )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IServerSession )
	END_COM_MAP()
    
	VOID SetSocket( CSocket *pSocket );
	VOID SetSharedFiles( CAtlMap<CComBSTR,CComBSTR> *pSharedFiles );
	DWORD static __stdcall AsyncTransferProc( void *pThis );
private:
	DWORD AsyncTransfer();
	DWORD RecvRequest();
	DWORD SendResponse();
	DWORD UploadFile();
	DWORD DownloadFile();
	VOID DispatchEvent( BSTR event, VARIANT *parameters, DWORD nParameters );
public:
	/* IServerSession */
	STDMETHOD(RegisterEvent)( BSTR event, BSTR handler, IDispatch *pWnd );
	STDMETHOD(get_Progress)( DWORD *Progress );
	STDMETHOD(get_Length)( DWORD *Length );
	STDMETHOD(get_Status)( BSTR *Status );
	STDMETHOD(Pause)();
	STDMETHOD(Resume)();
	STDMETHOD(Abort)();
	STDMETHOD(get_Method)( BSTR *Method );
	STDMETHOD(get_RequestURI)( BSTR *Request );
	STDMETHOD(get_HTTPHeaders)( BSTR *Headers );
};

class CHTTPClient :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IHTTPClient >
{
public:
	CHTTPClient();
	~CHTTPClient();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CHTTPClient )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IHTTPClient )
	END_COM_MAP()

	/* IHTTPClient */
	STDMETHOD(SetProxySettings)( BSTR Server, USHORT Port, BSTR Username, BSTR Password );
	STDMETHOD(Get)( BSTR URL, BSTR File, DWORD Offset, DWORD Length, IDispatch **ppSession );
	STDMETHOD(Post)( BSTR URL, BSTR File, DWORD Offset, DWORD Length, IDispatch **ppSession );
};

class CClientSession :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IClientSession >
{
private:
	VOID DispatchEvent( BSTR event, VARIANT *parameters, DWORD nParameters );
public:
	CClientSession();
	~CClientSession();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CClientSession )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IClientSession )
	END_COM_MAP()

	/* IClientSession */
	STDMETHOD(RegisterEvent)( BSTR event, BSTR handler, IDispatch *pWnd );
	STDMETHOD(GetProgress)( DWORD *Progress );
	STDMETHOD(GetLength)( DWORD *Length );
	STDMETHOD(GetStatus)( BSTR *Status );
};