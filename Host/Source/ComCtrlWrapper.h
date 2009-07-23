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
 * Filename:    ComCtrlWrapper.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CComDlg :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IComDlg>
{
private:
	IPdnWnd  *m_pWnd;
public:
	CComDlg();
	~CComDlg();
	
	BEGIN_COM_MAP(CComDlg)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IComDlg)
	END_COM_MAP()

	// IComDlg Implementation
	STDMETHOD(SetWnd)(IPdnWnd *pWnd);
	STDMETHOD(get_GetFileName)(IDispatch **ppGetFileName);
	STDMETHOD(get_ChooseFont)(IDispatch **ppChooseFont);
};

class CGetFileName :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IGetFileName>
{
private:
	IPdnWnd  *m_pWnd;
	OPENFILENAME ofn;
public:
	CGetFileName();
	~CGetFileName();
	
	BEGIN_COM_MAP(CGetFileName)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IGetFileName)
	END_COM_MAP()

	// IGetFileName Implementation
	STDMETHOD(SetWnd)(IPdnWnd *pWnd);
	STDMETHOD(put_Filter)(BSTR strFilter);
	STDMETHOD(put_FilterIndex)(DWORD dwFilterIndex);
	STDMETHOD(get_FilterIndex)(DWORD *dwFilterIndex);
	STDMETHOD(put_FileName)(BSTR strFileName);
	STDMETHOD(get_FileName)(BSTR *strFileName);
	STDMETHOD(put_InitDirectory)(BSTR strInitDirectory);
	STDMETHOD(put_Title)(BSTR strTitle);
	STDMETHOD(put_Flags)(DWORD dwFlags);
	STDMETHOD(put_DefExt)(BSTR strDefExt);
	STDMETHOD(DisplayOpen)(BSTR *strFileName);
	STDMETHOD(DisplaySave)(BSTR *strFileName);
};
class CChooseFont :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IChooseFont>
{
private:
	IPdnWnd  *m_pWnd;
	CHOOSEFONT cf;
	LOGFONT lf;
public:
	CChooseFont();
	~CChooseFont();
	
	BEGIN_COM_MAP(CChooseFont)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IChooseFont)
	END_COM_MAP()

	// IChooseFont Implementation
	STDMETHOD(SetWnd)(IPdnWnd *pWnd);
	STDMETHOD(get_FontHeight)(LONG *lfHeight);
	STDMETHOD(put_FontHeight)(LONG lfHeight);
	STDMETHOD(get_FontWidth)(LONG *lfWidth);
	STDMETHOD(put_FontWidth)(LONG lfWidth);
	STDMETHOD(get_FontEscapement)(LONG *lfEscapement);
	STDMETHOD(put_FontEscapement)(LONG lfEscapement);
	STDMETHOD(get_FontOrientation)(LONG *lfOrientation);
	STDMETHOD(put_FontOrientation)(LONG lfOrientation);
	STDMETHOD(get_FontWeight)(LONG *lfWeight);
	STDMETHOD(put_FontWeight)(LONG lfWeight);
	STDMETHOD(get_FontItalic)(BYTE *lfItalic);
	STDMETHOD(put_FontItalic)(BYTE lfItalic);
	STDMETHOD(get_FontUnderline)(BYTE *lfUnderline);
	STDMETHOD(put_FontUnderline)(BYTE lfUnderline);
	STDMETHOD(get_FontStrikeOut)(BYTE *lfStrikeOut);
	STDMETHOD(put_FontStrikeOut)(BYTE lfStrikeOut);
	STDMETHOD(get_FontCharSet)(BYTE *lfCharSet);
	STDMETHOD(put_FontCharSet)(BYTE lfCharSet);
	STDMETHOD(get_FontOutPrecision)(BYTE *lfOutPrecision);
	STDMETHOD(put_FontOutPrecision)(BYTE lfOutPrecision);
	STDMETHOD(get_FontClipPrecision)(BYTE *lfClipPrecision);
	STDMETHOD(put_FontClipPrecision)(BYTE lfClipPrecision);
	STDMETHOD(get_FontQuality)(BYTE *lfQuality);
	STDMETHOD(put_FontQuality)(BYTE lfQuality);
	STDMETHOD(get_FontPitchAndFamily)(BYTE *lfPitchAndFamily);
	STDMETHOD(put_FontPitchAndFamily)(BYTE lfPitchAndFamily);
	STDMETHOD(get_FontFace)(BSTR *lfFaceName);
	STDMETHOD(put_FontFace)(BSTR lfFaceName);
	STDMETHOD(get_PointSize)(INT *iPointSize);
	STDMETHOD(put_PointSize)(INT iPointSize);
	STDMETHOD(put_Flags)(DWORD dwFlags);
	STDMETHOD(put_Color)(DWORD dwColor);
	STDMETHOD(get_Color)(DWORD *dwColor);
	STDMETHOD(get_FontType)(WORD *dwType);
	STDMETHOD(put_SizeMin)(INT iSizeMin);
	STDMETHOD(put_SizeMax)(INT iSizeMax);
	STDMETHOD(Display)();
};
class CComCtrl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IComCtrl>
{
private:
	IPdnWnd *m_pWnd;
	IComDlg *m_pComDlg;
public:
	CComCtrl();
	~CComCtrl();

	BEGIN_COM_MAP(CComCtrl)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IComCtrl)
	END_COM_MAP()
	
	// IComCtrl Implementation
	STDMETHOD(SetWnd)(IPdnWnd *pWnd);
	STDMETHOD(get_ComDlg)(IDispatch **ppComDlg);
};