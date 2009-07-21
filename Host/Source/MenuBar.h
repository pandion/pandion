#pragma once

class CMenuBar :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IMenuBar >
{
private:
	HMENU m_Handle;
	HWND  m_hWnd;
public:
	CMenuBar();
	~CMenuBar();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CMenuBar )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IMenuBar )
	END_COM_MAP()

	STDMETHOD(AddItem)( BSTR label, DWORD pos, DWORD ID, IDispatch* subMenu );
	STDMETHOD(RemItem)( DWORD ID );
	STDMETHOD(Update)();
	STDMETHOD(get_Items)( DWORD* nItems );
	STDMETHOD(get_Handle)( VARIANT* pHandle );
	STDMETHOD(put_hwnd)( DWORD hwnd );
};