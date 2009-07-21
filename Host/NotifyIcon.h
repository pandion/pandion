#pragma once

class CIcon
{
	HICON m_hIcon;
public:
	CIcon();
	CIcon( BSTR strIcon, int nIndex );
	CIcon( CIcon & );
	~CIcon();

	HICON getHandle();
};

class CNotifyIcon :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< INotifyIcon >
{
	/* Describes the owner */
	HWND	m_hWnd;
	UINT	m_CBMsg;

	/* Describes the icon */
	CComBSTR m_text;
	CAtlMap<CComBSTR,CIcon>	 m_icons;
	CComBSTR                 m_currentIcon;

	/* The callback function to send events to the JScript */
    CComBSTR m_handler;
public:
	/* Do not register this class in the registry */
	DECLARE_NO_REGISTRY()

	/* QueryInterface implementation for INotifyIcon and IDispatch */
	BEGIN_COM_MAP( CNotifyIcon )
		COM_INTERFACE_ENTRY2( IDispatch, INotifyIcon )
	END_COM_MAP()

	/* Constructor and Destructor */
	CNotifyIcon();
	~CNotifyIcon();
	STDMETHOD(init)( HWND hWnd, UINT CBMsg );

	/* INotifyIcon implementation */
	STDMETHOD(show)();
	STDMETHOD(update)();
	STDMETHOD(remove)();
	STDMETHOD(shellNotify)( DWORD dwMessage );

	STDMETHOD(setText)( BSTR text );
	STDMETHOD(setIcon)( BSTR strIcon, int nIndex );

	STDMETHOD(setHandler)( BSTR handler );

	BSTR getHandler();
};