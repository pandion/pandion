#pragma once

struct CTypeInfo
{
	LPOLESTR name;
	DWORD dispID;
};

class CPdnWnd;
class CPandionModule;

class CExternal :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IExternal >
{
private:
	CPdnWnd        *m_pWnd;
	IComCtrl       *m_pComCtrl;
	CPandionModule *m_pModule;
public:
	CExternal();
	~CExternal();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP( CExternal )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IExternal )
	END_COM_MAP()

	/* IExternal implementation */
	STDMETHOD(Init)( void *pWnd, void *pModule );
	STDMETHOD(get_wnd)( VARIANT *pDisp );
	STDMETHOD(get_mainWnd)( VARIANT *pDisp );
	STDMETHOD(get_windows)( VARIANT *pDisp );
	STDMETHOD(get_globals)( VARIANT *pDisp );
	STDMETHOD(get_ComCtrl)( VARIANT *pDisp );
	STDMETHOD(get_HTTPEngine)( VARIANT *pDisp );
	STDMETHOD(get_SASL)( VARIANT *pDisp );
	STDMETHOD(createWindow)( BSTR name, BSTR file, VARIANT *params, BOOL bPopUnder = TRUE, VARIANT *pDisp = NULL );
	STDMETHOD(shellExec)( BSTR verb, BSTR file, BSTR params, BSTR dir, DWORD nShowCmd );

	STDMETHOD(get_cursorX)( VARIANT *retVal );
	STDMETHOD(get_cursorY)( VARIANT *retVal );

	STDMETHOD(get_notifyIcon)( VARIANT *pDisp );
	STDMETHOD(get_newPopupMenu)( VARIANT *pDisp );

	STDMETHOD(get_IPs)( VARIANT *pStr );
	STDMETHOD(get_CmdLine)( VARIANT *pStr );

	STDMETHOD(sleep)( DWORD dwMilliseconds );

	STDMETHOD(File)( BSTR path, VARIANT *pDisp );
	STDMETHOD(FileExists)( BSTR path, BOOL *bExists );
	STDMETHOD(get_Directory)( VARIANT *pDisp );
	STDMETHOD(get_XMPP)( VARIANT *pDisp );
	STDMETHOD(StringToSHA1)( BSTR str, BSTR *strSHA1 );
	STDMETHOD(GetSpecialFolder)( int nFolder, BSTR* Path );
	
	CRegKey Crawl( CRegKey RegKey, BSTR strKey );
	STDMETHOD(RegRead)( BSTR strHKey, BSTR strKey, BSTR value, VARIANT* vRetVal );
	STDMETHOD(get_Shortcut)( VARIANT *pDisp );
	STDMETHOD(UnZip)( BSTR path, BSTR targetDir, int *nSuccess );
	STDMETHOD(Base64ToString)( BSTR b64String, BSTR *UTF16String );
	STDMETHOD(StringToBase64)( BSTR UTF16String, BSTR *b64String );
	STDMETHOD(Fullscreen)( BOOL *bFullscreen );
};
