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
 * Filename:    AxHostWnd.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once

class AxHostWnd :
	public IOleClientSite,
	public IOleInPlaceSiteEx,
	public IOleInPlaceFrame
{
private:
	ULONG              m_COMReferenceCounter;
	bool               m_COMCannotSelfDelete;

	HWND               m_hWnd;
	HWND               m_hWndParent;

	IUnknown*          m_ParentWindow;
	IOleObject*        m_ActiveXControl;
	IOleInPlaceObject* m_InPlaceObject;

public:
	AxHostWnd(IUnknown *parentWindow);
	~AxHostWnd();

	IOleObject* Create(HWND hWndParent, std::wstring controlName);

	static LRESULT CALLBACK StartWindowProc(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy,
		UINT uFlags);

	/* IUnknown */
	STDMETHOD(QueryInterface)(const IID &riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	/* IOleClientSite */
	STDMETHOD(SaveObject)();
	STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
	STDMETHOD(GetContainer)(IOleContainer **ppContainer);
	STDMETHOD(ShowObject)();
	STDMETHOD(OnShowWindow)(BOOL fShow);
	STDMETHOD(RequestNewObjectLayout)();

	/* IOleInPlaceSite */
	STDMETHOD(CanInPlaceActivate)();
	STDMETHOD(OnInPlaceActivate)();
	STDMETHOD(OnUIActivate)();
	STDMETHOD(GetWindowContext)(IOleInPlaceFrame **ppFrame,
		IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect,
		LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHOD(Scroll)(SIZE scrollExtant);
	STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
	STDMETHOD(OnInPlaceDeactivate)();
	STDMETHOD(DiscardUndoState)();
	STDMETHOD(DeactivateAndUndo)();
	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

	/* IOleInPlaceSiteEx */
	STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);
	STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);
	STDMETHOD(RequestUIActivate)();

	/* IOleInPlaceSiteWindowless */
/*	STDMETHOD(CanWindowlessActivate)();
	STDMETHOD(GetCapture)();
	STDMETHOD(SetCapture)(BOOL fCapture);
	STDMETHOD(GetFocus)();
	STDMETHOD(SetFocus)(BOOL fFocus);
	STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC *phDC);
	STDMETHOD(ReleaseDC)(HDC hDC);
	STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
	STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
	STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll,
		LPCRECT pRectClip);
	STDMETHOD(AdjustRect)(LPRECT prc);
	STDMETHOD(OnDefWindowMessage)(UINT msg, 
		WPARAM wParam, LPARAM lParam, LRESULT *plResult);*/
      
	/* IOleWindow */
	STDMETHOD(GetWindow)(HWND *phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

	/* IOleInPlaceUIWindow */
	STDMETHOD(GetBorder)(LPRECT lprectBorder);
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS pborderwidths);
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS pborderwidths);
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject *pActiveObject,
		LPCOLESTR pszObjName);

	/* IOleInPlaceFrame */
	STDMETHOD(InsertMenus)(HMENU hmenuShared,
		LPOLEMENUGROUPWIDTHS lpMenuWidths);
	STDMETHOD(SetMenu)(HMENU hmenuShared, HOLEMENU holemenu,
		HWND hwndActiveObject);
	STDMETHOD(RemoveMenus)(HMENU hmenuShared);
	STDMETHOD(SetStatusText)(LPCOLESTR pszStatusText);
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(TranslateAccelerator)(LPMSG lpmsg, WORD wID);

	/* IAdviseSink */
/*	STDMETHOD_(void, OnDataChange)(FORMATETC *pFormatetc, STGMEDIUM *pStgmed);
	STDMETHOD_(void, OnViewChange)(DWORD dwAspect, LONG lindex);
	STDMETHOD_(void, OnRename)(IMoniker *pmk);
	STDMETHOD_(void, OnSave)();
	STDMETHOD_(void, OnClose)();*/

	/* IOleControlSite */
/*	STDMETHOD(OnControlInfoChanged)();
	STDMETHOD(LockInPlaceActive)(BOOL fLock);
	STDMETHOD(GetExtendedControl)(IDispatch **ppDisp);
	STDMETHOD(TransformCoords)(
		POINTL *pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags);
	STDMETHOD(TranslateAccelerator)(MSG *pMsg, DWORD grfModifiers);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)();*/

	/* IParseDisplayName */
/*	STDMETHOD(ParseDisplayName)(IBindCtx *pbc,
		LPOLESTR pszDisplayName, ULONG *pchEaten, IMoniker **ppmkOut);*/

	/* IOleContainer */
/*	STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown **ppenum);
	STDMETHOD(LockContainer)(BOOL fLock);*/

	/* ISimpleFrameSite */
/*	STDMETHOD(PreMessageFilter)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT *plResult, DWORD *pdwCookie);
	STDMETHOD(PostMessageFilter)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT *plResult, DWORD dwCookie);*/

	/* IPropertyNotifySink */
/*	STDMETHOD(OnChanged)(DISPID dispID);
	STDMETHOD(OnRequestEdit)(DISPID dispID);*/

	/* IErrorInfo */
/*	STDMETHOD(GetGUID)(GUID *pGUID);
	STDMETHOD(GetSource)(BSTR *pBstrSource);
	STDMETHOD(GetDescription)(BSTR *pBstrDescription);
	STDMETHOD(GetHelpFile)(BSTR *pBstrHelpFile);
	STDMETHOD(GetHelpContext)(DWORD *pdwHelpContext);*/

	/* IClassFactory */
/*	STDMETHOD(CreateInstance)(IUnknown *pUnkOuter, 
		REFIID riid, void **ppvObject);
	STDMETHOD(LockServer)( BOOL fLock);*/

	/* IClassFactory2 */
/*	STDMETHOD(GetLicInfo)(LICINFO *pLicInfo);
	STDMETHOD(RequestLicKey)(DWORD dwReserved, BSTR *pBstrKey);
	STDMETHOD(CreateInstanceLic)(IUnknown *pUnkOuter, IUnknown *pUnkReserved,
		REFIID riid, BSTR bstrKey, PVOID *ppvObj);*/
};