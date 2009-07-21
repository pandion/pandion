#include "stdafx.h"
#include "menubar.h"

#undef InsertMenuItem
extern BOOL InsertMenuItem( HMENU hMenu, UINT uItem, BOOL fByPosition, LPCMENUITEMINFOW lpmii );

CMenuBar::CMenuBar() : m_hWnd(0)
{
	m_Handle = CreateMenu();
}
CMenuBar::~CMenuBar()
{
	DestroyMenu( m_Handle );
}

STDMETHODIMP CMenuBar::AddItem( BSTR label, DWORD pos, DWORD ID, IDispatch *subMenu )
{
	if( !subMenu ) return S_FALSE;

	IPopupMenu* pPopupMenu = NULL;
	subMenu->QueryInterface( IID_IPopupMenu, (void**) &pPopupMenu );

	VARIANT subHandle;
	VariantInit( &subHandle );
	pPopupMenu->get_Handle( &subHandle );

	pPopupMenu->Release();


	MENUITEMINFOW nfo;
	nfo.cbSize      = sizeof( MENUITEMINFO );
	nfo.fMask       = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
	nfo.fType       = MFT_STRING;
	nfo.wID         = ID;
	nfo.hSubMenu    = (HMENU)subHandle.uintVal;
	nfo.cch         = wcslen( label ) + 1;
	nfo.dwTypeData  = label;

	return InsertMenuItem( m_Handle, pos, true, &nfo ) ? S_OK : GetLastError();
}
STDMETHODIMP CMenuBar::RemItem( DWORD ID )
{
	DeleteMenu( m_Handle, ID, MF_BYCOMMAND );

	return S_OK;
}
STDMETHODIMP CMenuBar::Update()
{
	if( DrawMenuBar( m_hWnd ) )
		return S_OK;
	else
		return GetLastError();
}
STDMETHODIMP CMenuBar::get_Items( DWORD* nItems )
{
	*nItems = GetMenuItemCount( m_Handle );
	return S_OK;
}
STDMETHODIMP CMenuBar::get_Handle( VARIANT* pHandle )
{
	pHandle->vt = VT_UI4;
	pHandle->uintVal = (UINT) m_Handle;
	return S_OK;
}
STDMETHODIMP CMenuBar::put_hwnd( DWORD hwnd )
{
	if( !m_hWnd )
        m_hWnd = (HWND)hwnd;
	return S_OK;
}