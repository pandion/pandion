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
 * Description: Declaration of the Common Control Wrappers.
 */
#pragma once
#include "DispInterfaceImpl.h"

class CPdnWnd;

class CComDlg :
	public DispInterfaceImpl<IComDlg>
{
private:
	CPdnWnd& m_wnd;
public:
	CComDlg(CPdnWnd& wnd);
	~CComDlg();

	/* ICComDlg Implementation */
	STDMETHOD(get_GetFileName)(IDispatch** ppCGetFileName);
	STDMETHOD(get_ChooseFont)(IDispatch** ppCChooseFont);
};

class CGetFileName :
	public DispInterfaceImpl<IGetFileName>
{
private:
	CPdnWnd&     m_wnd;
	OPENFILENAME m_ofn;
	TCHAR        m_filenameBuffer[0xFFFF];
public:
	CGetFileName(CPdnWnd& wnd);
	~CGetFileName();

	/* ICGetFileName Implementation */
	STDMETHOD(put_Filter)(BSTR strFilter);
	STDMETHOD(put_FilterIndex)(DWORD dwFilterIndex);
	STDMETHOD(get_FilterIndex)(DWORD* dwFilterIndex);
	STDMETHOD(put_FileName)(BSTR strFileName);
	STDMETHOD(get_FileName)(BSTR* strFileName);
	STDMETHOD(put_InitDirectory)(BSTR strInitDirectory);
	STDMETHOD(put_Title)(BSTR strTitle);
	STDMETHOD(put_Flags)(DWORD dwFlags);
	STDMETHOD(put_DefExt)(BSTR strDefExt);
	STDMETHOD(DisplayOpen)(BSTR* strFileName);
	STDMETHOD(DisplaySave)(BSTR* strFileName);
};

class CChooseFont :
	public DispInterfaceImpl<IChooseFont>
{
private:
	CPdnWnd&   m_wnd;
	CHOOSEFONT m_cf;
	LOGFONT    m_lf;
public:
	CChooseFont(CPdnWnd& wnd);
	~CChooseFont();

	/* ICChooseFont Implementation */
	STDMETHOD(get_FontHeight)(LONG* lfHeight);
	STDMETHOD(put_FontHeight)(LONG lfHeight);
	STDMETHOD(get_FontWidth)(LONG* lfWidth);
	STDMETHOD(put_FontWidth)(LONG lfWidth);
	STDMETHOD(get_FontEscapement)(LONG* lfEscapement);
	STDMETHOD(put_FontEscapement)(LONG lfEscapement);
	STDMETHOD(get_FontOrientation)(LONG* lfOrientation);
	STDMETHOD(put_FontOrientation)(LONG lfOrientation);
	STDMETHOD(get_FontWeight)(LONG* lfWeight);
	STDMETHOD(put_FontWeight)(LONG lfWeight);
	STDMETHOD(get_FontItalic)(BYTE* lfItalic);
	STDMETHOD(put_FontItalic)(BYTE lfItalic);
	STDMETHOD(get_FontUnderline)(BYTE* lfUnderline);
	STDMETHOD(put_FontUnderline)(BYTE lfUnderline);
	STDMETHOD(get_FontStrikeOut)(BYTE* lfStrikeOut);
	STDMETHOD(put_FontStrikeOut)(BYTE lfStrikeOut);
	STDMETHOD(get_FontCharSet)(BYTE* lfCharSet);
	STDMETHOD(put_FontCharSet)(BYTE lfCharSet);
	STDMETHOD(get_FontOutPrecision)(BYTE* lfOutPrecision);
	STDMETHOD(put_FontOutPrecision)(BYTE lfOutPrecision);
	STDMETHOD(get_FontClipPrecision)(BYTE* lfClipPrecision);
	STDMETHOD(put_FontClipPrecision)(BYTE lfClipPrecision);
	STDMETHOD(get_FontQuality)(BYTE* lfQuality);
	STDMETHOD(put_FontQuality)(BYTE lfQuality);
	STDMETHOD(get_FontPitchAndFamily)(BYTE* lfPitchAndFamily);
	STDMETHOD(put_FontPitchAndFamily)(BYTE lfPitchAndFamily);
	STDMETHOD(get_FontFace)(BSTR* lfFaceName);
	STDMETHOD(put_FontFace)(BSTR lfFaceName);
	STDMETHOD(get_PointSize)(INT* iPointSize);
	STDMETHOD(put_PointSize)(INT iPointSize);
	STDMETHOD(put_Flags)(DWORD dwFlags);
	STDMETHOD(put_Color)(DWORD dwColor);
	STDMETHOD(get_Color)(DWORD* dwColor);
	STDMETHOD(get_FontType)(WORD* dwType);
	STDMETHOD(put_SizeMin)(INT iSizeMin);
	STDMETHOD(put_SizeMax)(INT iSizeMax);
	STDMETHOD(Display)();
};

class CComCtrl :
	public DispInterfaceImpl<IComCtrl>
{
private:
	CComDlg    m_ComDlg;
public:
	CComCtrl(CPdnWnd& wnd);
	~CComCtrl();
	
	/* IComCtrl Implementation */
	STDMETHOD(get_ComDlg)(IDispatch** ppCComDlg);
};