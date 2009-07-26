/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    AxHostWnd.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "AxHostWnd.h"

AxHostWnd::AxHostWnd(IUnknown *parentWindow)
{
	m_COMReferenceCounter = 0;
	m_COMCannotSelfDelete = false;

	m_hWnd       = NULL;
	m_hWndParent = NULL;

	m_ActiveXControl = NULL;
	m_InPlaceObject  = NULL;
	m_ParentWindow   = parentWindow;
}

AxHostWnd::~AxHostWnd()
{
	m_ParentWindow->Release();
	m_InPlaceObject->Release();
	m_ActiveXControl->Release();
}

IOleObject* AxHostWnd::Create(HWND hWndParent, std::wstring controlName)
{
	m_hWndParent = hWndParent;

	WNDCLASSEX wc =
	{ 
		sizeof(WNDCLASSEX), CS_DBLCLKS, StartWindowProc, 0, 0, NULL, NULL,
		NULL, (HBRUSH)(COLOR_BTNFACE), NULL, controlName.c_str(), NULL 
	};
	::RegisterClassEx(&wc);

	RECT rect;
	::GetClientRect(m_hWndParent, &rect);
	m_hWnd = ::CreateWindowEx(0, controlName.c_str(), L"",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		m_hWndParent, 0, GetModuleHandle(NULL), this);
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	CLSID clsid;
	::CLSIDFromProgID(controlName.c_str(), &clsid);

	IStorage* storage = NULL;
	::StgCreateStorageEx(NULL,
		STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 
		STGFMT_STORAGE, 0, NULL, NULL, IID_IStorage, (LPVOID*) &storage);

	HRESULT hr = ::OleCreate(clsid, IID_IOleObject, OLERENDER_DRAW,
		NULL, dynamic_cast<IOleClientSite*>(this), storage,
		(LPVOID*) &m_ActiveXControl);
	
	hr = m_ActiveXControl->QueryInterface(IID_IOleInPlaceObject,
		(void**) &m_InPlaceObject);
	hr = m_ActiveXControl->QueryInterface(IID_IOleInPlaceActiveObject,
		(LPVOID*) &m_ActiveObject);

	hr = ::OleSetContainedObject(m_ActiveXControl, TRUE);
	hr = ::OleRun(m_ActiveXControl);
	hr = m_ActiveXControl->DoVerb(OLEIVERB_SHOW, NULL, 
		dynamic_cast<IOleClientSite*>(this), 0, m_hWndParent, NULL);

	return m_ActiveXControl;
}

LRESULT CALLBACK AxHostWnd::StartWindowProc(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CREATE)
	{
		CREATESTRUCT* cs = (CREATESTRUCT*) lParam;
		::SetWindowLong(hWnd, GWL_USERDATA, (LONG) cs->lpCreateParams);
	}
	AxHostWnd* axHostWindow = (AxHostWnd*) ::GetWindowLong(hWnd, GWL_USERDATA);
	if(axHostWindow != NULL)
	{
		return axHostWindow->WindowProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT AxHostWnd::WindowProc(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CREATE)
	{
		return OnCreate(uMsg, wParam, lParam);
	}
	else if(uMsg == WM_PAINT)
	{
		return OnPaint(uMsg, wParam, lParam);
	}
	else if(uMsg == WM_SIZE)
	{
		return OnSize(uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
}

LRESULT AxHostWnd::OnForwardMessage(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = m_ActiveObject->TranslateAccelerator((LPMSG)lParam);
	return hr == S_OK;
}
LRESULT AxHostWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
LRESULT AxHostWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
	RECT rPos;
	::GetClientRect(m_hWndParent, &rPos);
	RECT rClip = rPos;
	hr = m_InPlaceObject->SetObjectRects(&rPos, &rClip);

	return 0;
}
LRESULT AxHostWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT updateRect;
	if(::GetUpdateRect(m_hWnd, &updateRect, FALSE))
	{
		PAINTSTRUCT ps;
		HDC dc = ::BeginPaint(m_hWnd, &ps);
		HRESULT hr = ::OleDraw(m_ActiveXControl, DVASPECT_TRANSPARENT,
			dc, &updateRect);
		::EndPaint(m_hWnd, &ps);
	}
	return 0;
}

BOOL AxHostWnd::SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, 
	int cy, UINT uFlags)
{
	return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

/*
 * IUnknown::QueryInterface() implementation
 */
STDMETHODIMP AxHostWnd::QueryInterface(REFIID riid, void** ppvObject)
{
	if(ppvObject == NULL)
		return E_POINTER;
	else
		*ppvObject = NULL;
	
	if(::IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = dynamic_cast<IPdnWnd*>(this);
	else if(::IsEqualGUID(riid, IID_IOleClientSite))
		*ppvObject = dynamic_cast<IOleClientSite*>(this);
	else if(::IsEqualGUID(riid, IID_IOleInPlaceSite))
		*ppvObject = dynamic_cast<IOleInPlaceSite*>(this);
	else if(::IsEqualGUID(riid, IID_IOleInPlaceSiteEx))
		*ppvObject = dynamic_cast<IOleInPlaceSiteEx*>(this);
	else if(::IsEqualGUID(riid, IID_IOleInPlaceUIWindow))
		*ppvObject = dynamic_cast<IOleInPlaceUIWindow*>(this);
	else if(::IsEqualGUID(riid, IID_IOleInPlaceFrame))
		*ppvObject = dynamic_cast<IOleInPlaceFrame*>(this);
	else
		return m_ParentWindow->QueryInterface(riid, ppvObject);

	AddRef();
	return S_OK;
}

/*
 * IUnknown::AddRef() implementation
 */
STDMETHODIMP_(ULONG) AxHostWnd::AddRef()
{
	return ++m_COMReferenceCounter;
}

/*
 * IUnknown::Release() implementation
 */
STDMETHODIMP_(ULONG) AxHostWnd::Release()
{
	if(m_COMReferenceCounter> 1)
	{
		return --m_COMReferenceCounter;
	}
	else
	{
		if(!m_COMCannotSelfDelete)
		{
			delete this;
		}
		return 0;
	}
}
/* IOleClientSite */
STDMETHODIMP AxHostWnd::SaveObject()
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetContainer(IOleContainer **ppContainer)
{
	return QueryInterface(IID_IOleContainer, (void**) ppContainer);
}
STDMETHODIMP AxHostWnd::ShowObject()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::OnShowWindow(BOOL fShow)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

/* IOleInPlaceSite */
STDMETHODIMP AxHostWnd::CanInPlaceActivate()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::OnInPlaceActivate()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::OnUIActivate()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::GetWindowContext(IOleInPlaceFrame **ppFrame,
	IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect,
	LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppFrame = dynamic_cast<IOleInPlaceFrame*>(this);
	ppDoc = 0;
	
	RECT r;
	::GetClientRect(m_hWnd, &r);
	*lprcPosRect = r;
	*lprcClipRect = r;

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = m_hWnd;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}
STDMETHODIMP AxHostWnd::Scroll(SIZE scrollExtant)
{
	return S_FALSE;
}
STDMETHODIMP AxHostWnd::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::OnInPlaceDeactivate()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::DiscardUndoState()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::DeactivateAndUndo()
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::OnPosRectChange(LPCRECT lprcPosRect)
{
	return S_OK;
}

/* IOleInPlaceSiteEx */
STDMETHODIMP AxHostWnd::OnInPlaceActivateEx(BOOL *pfNoRedraw, DWORD dwFlags)
{
	if(pfNoRedraw)
	{
		*pfNoRedraw = FALSE;
	}
	return S_OK;
}
STDMETHODIMP AxHostWnd::OnInPlaceDeactivateEx(BOOL fNoRedraw)
{
	return S_FALSE;
}
STDMETHODIMP AxHostWnd::RequestUIActivate()
{
	return S_OK;
}

/* IOleWindow */
STDMETHODIMP AxHostWnd::GetWindow(HWND *phwnd)
{
	*phwnd = m_hWnd;
	return S_OK;
}
STDMETHODIMP AxHostWnd::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

/* IAdviseSink */
/*STDMETHODIMP_(void) AxHostWnd::OnDataChange(FORMATETC *pFormatetc, STGMEDIUM *pStgmed)
{
}
STDMETHODIMP_(void) AxHostWnd::OnViewChange(DWORD dwAspect, LONG lindex)
{
}
STDMETHODIMP_(void) AxHostWnd::OnRename(IMoniker *pmk)
{
}
STDMETHODIMP_(void) AxHostWnd::OnSave()
{
}
STDMETHODIMP_(void) AxHostWnd::OnClose()
{
}*/

/* IOleControlSite */
/*STDMETHODIMP AxHostWnd::OnControlInfoChanged()
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::LockInPlaceActive(BOOL fLock)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetExtendedControl(IDispatch **ppDisp)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::TransformCoords(
	POINTL *pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::OnFocus(BOOL fGotFocus)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::ShowPropertyFrame()
{
	return E_NOTIMPL;
}*/

/* IOleInPlaceFrame */
STDMETHODIMP AxHostWnd::InsertMenus(HMENU hmenuShared,
	LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::SetMenu(HMENU hmenuShared, HOLEMENU holemenu,
	HWND hwndActiveObject)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::RemoveMenus(HMENU hmenuShared)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::SetStatusText(LPCOLESTR pszStatusText)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::EnableModeless(BOOL fEnable)
{
	return S_OK;
}
STDMETHODIMP AxHostWnd::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	return S_FALSE;
}

/* IOleInPlaceUIWindow */
STDMETHODIMP AxHostWnd::GetBorder(LPRECT lprectBorder)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::SetActiveObject(IOleInPlaceActiveObject *pActiveObject,
	LPCOLESTR pszObjName)
{
	return S_OK;
}

/* IParseDisplayName */
/*STDMETHODIMP AxHostWnd::ParseDisplayName(IBindCtx *pbc,
	LPOLESTR pszDisplayName, ULONG *pchEaten, IMoniker **ppmkOut)
{
	return E_NOTIMPL;
}*/

/* IOleContainer */
/*STDMETHODIMP AxHostWnd::EnumObjects(DWORD grfFlags, IEnumUnknown **ppenum)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::LockContainer(BOOL fLock)
{
	return E_NOTIMPL;
}*/

/* ISimpleFrameSite */
/*STDMETHODIMP AxHostWnd::PreMessageFilter(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
	LRESULT *plResult, DWORD *pdwCookie)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::PostMessageFilter(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
	LRESULT *plResult, DWORD dwCookie)
{
	return E_NOTIMPL;
}*/

/* IPropertyNotifySink */
/*STDMETHODIMP AxHostWnd::OnChanged(DISPID dispID)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::OnRequestEdit(DISPID dispID)
{
	return E_NOTIMPL;
}*/

/* IErrorInfo */
/*STDMETHODIMP AxHostWnd::GetGUID(GUID *pGUID)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetSource(BSTR *pBstrSource)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetDescription(BSTR *pBstrDescription)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetHelpFile(BSTR *pBstrHelpFile)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::GetHelpContext(DWORD *pdwHelpContext)
{
	return E_NOTIMPL;
}*/

/* IClassFactory */
/*STDMETHODIMP AxHostWnd::CreateInstance(IUnknown *pUnkOuter, 
		REFIID riid, void **ppvObject)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::LockServer( BOOL fLock)
{
	return E_NOTIMPL;
}*/

/* IClassFactory2 */
/*STDMETHODIMP AxHostWnd::GetLicInfo(LICINFO *pLicInfo)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::RequestLicKey(DWORD dwReserved, BSTR *pBstrKey)
{
	return E_NOTIMPL;
}
STDMETHODIMP AxHostWnd::CreateInstanceLic(IUnknown *pUnkOuter, IUnknown *pUnkReserved,
		REFIID riid, BSTR bstrKey, PVOID *ppvObj)
{
	return E_NOTIMPL;
}*/
