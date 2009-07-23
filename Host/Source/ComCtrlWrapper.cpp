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
 * Filename:    ComCtrlWrapper.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: Implementation of the Common Control Wrappers.
 */
#include "stdafx.h"
#include "ComCtrlWrapper.h"
#include "PdnWnd.h"

/* CComDlg Implementation */
CComDlg::CComDlg(CPdnWnd& wnd) :
	m_wnd(wnd)
{
}
CComDlg::~CComDlg()
{
}
STDMETHODIMP CComDlg::get_GetFileName(IDispatch **ppCGetFileName)
{
	return (new CGetFileName(m_wnd))->QueryInterface(
		IID_IDispatch, (LPVOID*) ppCGetFileName);
}
STDMETHODIMP CComDlg::get_ChooseFont(IDispatch **ppCChooseFont)
{
	return (new CChooseFont(m_wnd))->QueryInterface(
		IID_IDispatch, (LPVOID*) ppCChooseFont);
}
/* CGetFileName Implementation */
CGetFileName::CGetFileName(CPdnWnd& wnd) :
	m_wnd(wnd)
{
	::ZeroMemory(&m_ofn, sizeof(m_ofn));
	m_ofn.lStructSize = sizeof(m_ofn);
	wnd.get_Handle((DWORD*)&m_ofn.hwndOwner);
	m_ofn.hInstance = GetModuleHandle(NULL);
	m_ofn.lpstrFilter = NULL;
	m_ofn.lpstrCustomFilter = NULL;
	m_ofn.nMaxCustFilter = 0;
	m_ofn.nFilterIndex = 1;
	m_ofn.lpstrFile = m_filenameBuffer;
	m_ofn.nMaxFile = sizeof(m_filenameBuffer);
	m_ofn.lpstrFileTitle = NULL;
	m_ofn.nMaxFileTitle = 0;
	m_ofn.lpstrInitialDir = NULL;
	m_ofn.lpstrTitle = NULL;
	m_ofn.Flags = 0;
	m_ofn.lpstrDefExt = NULL;
	m_ofn.lCustData = 0;
	m_ofn.lpfnHook = NULL;
	m_ofn.lpTemplateName = NULL;
	m_filenameBuffer[0] = 0;
}
CGetFileName::~CGetFileName()
{
	if(m_ofn.lpstrFilter)
	{
        delete m_ofn.lpstrFilter;
	}
	if(m_ofn.lpstrInitialDir)
	{
		delete m_ofn.lpstrInitialDir;
	}
	if(m_ofn.lpstrTitle)
	{
		delete m_ofn.lpstrTitle;
	}
	if(m_ofn.lpstrDefExt)
	{
		delete m_ofn.lpstrDefExt;
	}
}
STDMETHODIMP CGetFileName::put_Filter(BSTR strFilter)
{
	if(m_ofn.lpstrFilter)
	{
        delete m_ofn.lpstrFilter;
	}

	LPTSTR buffer = new TCHAR[::SysStringLen(strFilter) + 2];
	int i = 0;
	for(; strFilter[i] != 0; i++)
	{
		buffer[i] = strFilter[i];
		if(buffer[i] == '|')
		{
			buffer[i] = 0;
		}
	}
	buffer[i+1] = 0;
	buffer[i+2] = 0;
	m_ofn.lpstrFilter = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::put_FilterIndex(DWORD dwFilterIndex)
{
	m_ofn.nFilterIndex = dwFilterIndex;
	return S_OK;
}
STDMETHODIMP CGetFileName::get_FilterIndex(DWORD *dwFilterIndex)
{
	*dwFilterIndex = m_ofn.nFilterIndex;
	return S_OK;
}
STDMETHODIMP CGetFileName::put_FileName(BSTR strFileName)
{
	StringCchCopy(m_ofn.lpstrFile, 
		sizeof(m_filenameBuffer), strFileName);
	return S_OK;
}
STDMETHODIMP CGetFileName::get_FileName(BSTR *strFileName)
{
	*strFileName = ::SysAllocString(m_ofn.lpstrFile);
	return S_OK;
}
STDMETHODIMP CGetFileName::put_InitDirectory(BSTR strInitDirectory)
{
	if(m_ofn.lpstrInitialDir)
	{
		delete m_ofn.lpstrInitialDir;
	}

	LPTSTR buffer = new TCHAR[::SysStringLen(strInitDirectory) + 1];
	::StringCchCopy(buffer, ::SysStringLen(strInitDirectory) + 1,
		strInitDirectory);
	m_ofn.lpstrInitialDir = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::put_Title(BSTR strTitle)
{
	if(m_ofn.lpstrTitle)
	{
		delete m_ofn.lpstrTitle;
	}

	LPTSTR buffer = new TCHAR[::SysStringLen(strTitle) + 1];
	::StringCchCopy(buffer, ::SysStringLen(strTitle) + 1,
		strTitle);
	m_ofn.lpstrTitle = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::put_Flags(DWORD dwFlags)
{
	m_ofn.Flags = dwFlags;
	return S_OK;
}
STDMETHODIMP CGetFileName::put_DefExt(BSTR strDefExt)
{
	if(m_ofn.lpstrDefExt)
	{
		delete m_ofn.lpstrDefExt;
	}

	LPTSTR buffer = new TCHAR[::SysStringLen(strDefExt) + 1];
	::StringCchCopy(buffer, ::SysStringLen(strDefExt) + 1, strDefExt);
	m_ofn.lpstrDefExt = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::DisplayOpen(BSTR *strFileName)
{
	if(::GetOpenFileName(&m_ofn))
	{
		int i = 0;
		while(m_ofn.lpstrFile[i] != 0 && m_ofn.lpstrFile[i+1] != 0)
		{
			if(m_ofn.lpstrFile[i] == 0)
			{
				m_ofn.lpstrFile[i] = '|';
			}
			i++;
		}
		*strFileName = ::SysAllocString(m_ofn.lpstrFile);
		return S_OK;
	}
	else
	{
		return E_ABORT;
	}
}
STDMETHODIMP CGetFileName::DisplaySave(BSTR *strFileName)
{
	if(::GetSaveFileName(&m_ofn))
	{
		*strFileName = ::SysAllocString(m_ofn.lpstrFile);
		return S_OK;
	}
	else
	{
		return E_ABORT;
	}
}
/* CChooseFont Implementation */
CChooseFont::CChooseFont(CPdnWnd& wnd) :
	m_wnd(wnd)
{
	::ZeroMemory(&m_lf, sizeof(m_lf));
	m_lf.lfWeight = FW_NORMAL;
	m_lf.lfCharSet = DEFAULT_CHARSET;
	m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lf.lfQuality = DEFAULT_QUALITY;
	m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	m_lf.lfCharSet = OEM_CHARSET;

	::ZeroMemory(&m_cf, sizeof(m_cf));
	m_cf.lStructSize = sizeof(m_cf);
	m_cf.hwndOwner = 0;
	m_cf.hDC = GetDC(0);
	m_cf.lpLogFont = &m_lf;
	m_cf.iPointSize = 0;
	m_cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
	m_cf.rgbColors = 0;
	m_cf.lCustData = 0;
	m_cf.lpfnHook = 0;
	m_cf.lpTemplateName = 0;
	m_cf.hInstance = GetModuleHandle(NULL);
	m_cf.lpszStyle = 0;
	m_cf.nFontType = REGULAR_FONTTYPE;
	m_cf.nSizeMin = 10;
	m_cf.nSizeMax = 72;
}
CChooseFont::~CChooseFont()
{
}
STDMETHODIMP CChooseFont::get_FontHeight(LONG *lfHeight)
{
	*lfHeight = m_lf.lfHeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontHeight(LONG lfHeight)
{
	m_lf.lfHeight = lfHeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontWidth(LONG *lfWidth)
{
	*lfWidth = m_lf.lfWidth;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontWidth(LONG lfWidth)
{
	m_lf.lfWidth = lfWidth;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontEscapement(LONG *lfEscapement)
{
	*lfEscapement = m_lf.lfEscapement;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontEscapement(LONG lfEscapement)
{
	m_lf.lfEscapement = lfEscapement;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontOrientation(LONG *lfOrientation)
{
	*lfOrientation = m_lf.lfOrientation;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontOrientation(LONG lfOrientation)
{
	m_lf.lfOrientation = lfOrientation;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontWeight(LONG *lfWeight)
{
	*lfWeight = m_lf.lfWeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontWeight(LONG lfWeight)
{
	m_lf.lfWeight = lfWeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontItalic(BYTE *lfItalic)
{
	*lfItalic = m_lf.lfItalic;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontItalic(BYTE lfItalic)
{
	m_lf.lfItalic = lfItalic;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontUnderline(BYTE *lfUnderline)
{
	*lfUnderline = m_lf.lfUnderline;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontUnderline(BYTE lfUnderline)
{
	m_lf.lfUnderline = lfUnderline;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontStrikeOut(BYTE *lfStrikeOut)
{
	*lfStrikeOut = m_lf.lfStrikeOut;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontStrikeOut(BYTE lfStrikeOut)
{
	m_lf.lfStrikeOut = lfStrikeOut;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontCharSet(BYTE *lfCharSet)
{
	*lfCharSet = m_lf.lfCharSet;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontCharSet(BYTE lfCharSet)
{
	m_lf.lfCharSet = lfCharSet;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontOutPrecision(BYTE *lfOutPrecision)
{
	*lfOutPrecision = m_lf.lfOutPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontOutPrecision(BYTE lfOutPrecision)
{
	m_lf.lfOutPrecision = lfOutPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontClipPrecision(BYTE *lfClipPrecision)
{
	*lfClipPrecision = m_lf.lfClipPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontClipPrecision(BYTE lfClipPrecision)
{
	m_lf.lfClipPrecision = lfClipPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontQuality(BYTE *lfQuality)
{
	*lfQuality = m_lf.lfQuality;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontQuality(BYTE lfQuality)
{
	m_lf.lfQuality = lfQuality;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontPitchAndFamily(BYTE *lfPitchAndFamily)
{
	*lfPitchAndFamily = m_lf.lfPitchAndFamily;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontPitchAndFamily(BYTE lfPitchAndFamily)
{
	m_lf.lfPitchAndFamily = lfPitchAndFamily;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontFace(BSTR *lfFaceName)
{
	*lfFaceName = ::SysAllocString(m_lf.lfFaceName);
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontFace(BSTR lfFaceName)
{
	if(::SysStringLen(lfFaceName) < (LF_FACESIZE - 1))
	{
		::StringCchCopy(m_lf.lfFaceName, 32, lfFaceName);
	}
	return S_OK;
}
STDMETHODIMP CChooseFont::get_PointSize(INT *iPointSize)
{
	*iPointSize = m_cf.iPointSize / 10;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_PointSize(INT iPointSize)
{
	m_lf.lfHeight = - MulDiv(iPointSize,
		::GetDeviceCaps(GetDC(0), LOGPIXELSY), 72);
	m_cf.iPointSize = iPointSize * 10;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_Flags(DWORD dwFlags)
{
	m_cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | dwFlags;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_Color(DWORD dwColor)
{
	m_cf.rgbColors = (COLORREF)dwColor;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_Color(DWORD *dwColor)
{
	*dwColor = m_cf.rgbColors;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontType(WORD *dwType)
{
	*dwType = m_cf.nFontType;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_SizeMin(INT iSizeMin)
{
	m_cf.nSizeMin = iSizeMin;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_SizeMax(INT iSizeMax)
{
	m_cf.nSizeMax = iSizeMax;
	return S_OK;
}
STDMETHODIMP CChooseFont::Display()
{
	if(::ChooseFont(&m_cf))
	{
		return S_OK;
	}
	else
	{
		return E_ABORT;
	}
}
/* CComCtrl Implementation */
CComCtrl::CComCtrl(CPdnWnd& wnd) :
	m_ComDlg(wnd)
{
	::InitCommonControls();
	m_ComDlg.DisableSelfDelete();
}
CComCtrl::~CComCtrl()
{
}
STDMETHODIMP CComCtrl::get_ComDlg(IDispatch** ppDisp)
{
	m_ComDlg.QueryInterface(IID_IDispatch, (LPVOID*)ppDisp);
	return S_OK;
}