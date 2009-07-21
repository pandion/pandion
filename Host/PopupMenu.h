#pragma once

class CMainWnd;

class CPopupMenu :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IPopupMenu >
{
private:
	HMENU m_Handle;
	DWORD m_Position;
	DWORD m_Choice;
public:
	CPopupMenu();
	~CPopupMenu();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP( CPopupMenu )
		COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IPopupMenu )
	END_COM_MAP()

// IPopupMenu
	STDMETHOD(AddItem)( BOOL bEnabled, BOOL bChecked, BOOL bRadio, BOOL bDefault, DWORD hSubMenu, BSTR label, DWORD ID );
	STDMETHOD(Show)( DWORD x, DWORD y, BOOL fromTray = FALSE );
	STDMETHOD(get_Handle)( VARIANT* pVal );
	STDMETHOD(get_Choice)( VARIANT* pVal );
	STDMETHOD(Clear)();
	STDMETHOD(AddSeparator)();
};