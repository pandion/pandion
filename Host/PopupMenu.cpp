#include "stdafx.h"
#include "MainWnd.h"
#include "PopupMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CPopupMenu

#undef InsertMenuItem
BOOL InsertMenuItem( HMENU hMenu, UINT uItem, BOOL fByPosition, LPCMENUITEMINFOW lpmii )
{
	if( ( GetVersion() & 0x80000000 ) )
	{
		MENUITEMINFOA nfo;
		nfo.cbSize = sizeof(nfo);
		nfo.cch = lpmii->cch;
		nfo.dwItemData = lpmii->dwItemData;
		nfo.dwTypeData = CW2A( (LPWSTR) lpmii->dwTypeData );
		nfo.fMask = lpmii->fMask;
		nfo.fState = lpmii->fState;
		nfo.fType = lpmii->fType;
		nfo.hbmpChecked = lpmii->hbmpChecked;
		nfo.hbmpUnchecked = lpmii->hbmpUnchecked;
		nfo.hSubMenu = lpmii->hSubMenu;
		nfo.wID = lpmii->wID;
		return InsertMenuItemA( hMenu, uItem, fByPosition, &nfo );
	}
	else
        return InsertMenuItemW( hMenu, uItem, fByPosition, lpmii );
}


CPopupMenu::CPopupMenu()
{
	m_Handle = CreatePopupMenu();
	m_Position = 1;
	m_Choice = 0;
}
CPopupMenu::~CPopupMenu()
{
}

STDMETHODIMP CPopupMenu::AddItem( BOOL bEnabled, BOOL bChecked, BOOL bRadio, BOOL bDefault,
	DWORD hSubMenu, BSTR label, DWORD ID )
{
	MENUITEMINFOW nfo;
	nfo.cbSize     = sizeof( MENUITEMINFO );
	nfo.fMask      = MIIM_STATE | MIIM_TYPE | MIIM_ID |
					 ( hSubMenu ? MIIM_SUBMENU : 0 );
	nfo.fState     = ( bEnabled ? MFS_ENABLED : MFS_DISABLED ) | 
					 ( ( bRadio || bChecked ) ? MFS_CHECKED : MFS_UNCHECKED ) |
					 ( bDefault ? MFS_DEFAULT : 0 );
	nfo.fType      = MFT_STRING | 
			         ( bRadio ? MFT_RADIOCHECK : 0 );
	nfo.wID        = ID;
	nfo.hSubMenu   = (HMENU) hSubMenu;
	nfo.cch        = wcslen( label ) + 1;
	nfo.dwTypeData = label;

	return InsertMenuItem( m_Handle, m_Position++, true, &nfo ) ? S_OK : GetLastError();
}

STDMETHODIMP CPopupMenu::Show( DWORD x, DWORD y, BOOL fromTray )
{
	if( fromTray )
		SetForegroundWindow( CMainWnd::GetMainWindow() );
	m_Choice = TrackPopupMenu( m_Handle,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
		x, y, NULL, CMainWnd::GetMainWindow(), NULL );
	if( fromTray )
		PostMessage( CMainWnd::GetMainWindow(), WM_NULL, 0, 0 );

	DestroyMenu( m_Handle );
	if( m_Choice )
		return S_OK;
	return S_FALSE;
}

STDMETHODIMP CPopupMenu::Clear()
{
	DestroyMenu( m_Handle );
	m_Handle = CreatePopupMenu();
	m_Position = 0;
	m_Choice = 0;

	return S_OK;
}

STDMETHODIMP CPopupMenu::AddSeparator()
{
	MENUITEMINFOW nfo;
	nfo.cbSize = sizeof( MENUITEMINFO );
	nfo.fMask = MIIM_TYPE;
	nfo.fType = MFT_SEPARATOR;

	if( InsertMenuItem( m_Handle, m_Position, TRUE, &nfo ) )
	{
		m_Position++;
		return S_OK;
	}
	else 
		return S_FALSE;
}

STDMETHODIMP CPopupMenu::get_Handle( VARIANT* pVal )
{
	pVal->uintVal = (long)m_Handle;
	pVal->vt = VT_UI4;
	return S_OK;
}

STDMETHODIMP CPopupMenu::get_Choice( VARIANT* pVal )
{
	pVal->intVal = (long)m_Choice;
	pVal->vt = VT_I4;
	return S_OK;
}