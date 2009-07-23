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
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "ComCtrlWrapper.h"

/* CComDlg Implementation */
CComDlg::CComDlg()
{
}
CComDlg::~CComDlg()
{
	if(m_pWnd)
		m_pWnd->Release();
	else
		ATLASSERT(0);
}
STDMETHODIMP CComDlg::SetWnd(IPdnWnd *pWnd)
{
	pWnd->QueryInterface(IID_IPdnWnd, (void **) &m_pWnd); 
	return S_OK;
}
STDMETHODIMP CComDlg::get_GetFileName(IDispatch **ppGetFileName)
{
	(new CComObject<CGetFileName>)->QueryInterface(ppGetFileName);
	((IGetFileName *)*ppGetFileName)->SetWnd(m_pWnd);
	return S_OK;
}
STDMETHODIMP CComDlg::get_ChooseFont(IDispatch **ppChooseFont)
{
	(new CComObject<CChooseFont>)->QueryInterface(ppChooseFont);
	((IChooseFont *)*ppChooseFont)->SetWnd(m_pWnd);
	return S_OK;
}
/* CGetFileName Implementation */
CGetFileName::CGetFileName()
{
	LPTSTR buffer = new TCHAR[0xFFFF];
	buffer[0] = 0;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = 0;
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.lpstrFilter = NULL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = 0xFFFF;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}
CGetFileName::~CGetFileName()
{
	delete ofn.lpstrFile;
	if(ofn.lpstrFilter)
        delete ofn.lpstrFilter;
	if(ofn.lpstrInitialDir)
		delete ofn.lpstrInitialDir;
	if(ofn.lpstrTitle)
		delete ofn.lpstrTitle;
	if(ofn.lpstrDefExt)
		delete ofn.lpstrDefExt;
}
STDMETHODIMP CGetFileName::SetWnd(IPdnWnd *pWnd)
{
	pWnd->QueryInterface(IID_IPdnWnd, (void **) &m_pWnd);
	pWnd->get_Handle((DWORD *) &ofn.hwndOwner);
	return S_OK;
}
STDMETHODIMP CGetFileName::put_Filter(BSTR strFilter)
{
	LPTSTR buffer = new TCHAR[wcslen(strFilter)+2];
	StringCchCopy(buffer, wcslen(strFilter)+2, CW2T(strFilter));
	int i = 0;
	while(buffer[i] != 0)
	{
		if(buffer[i] == '|')
			buffer[i] = 0;
		i++;
	}
	buffer[i+1] = 0;
	ofn.lpstrFilter = buffer;
	return S_OK;
}
STDMETHODIMP CGetFileName::put_FilterIndex(DWORD dwFilterIndex)
{
	ofn.nFilterIndex = dwFilterIndex;
	return S_OK;
}
STDMETHODIMP CGetFileName::get_FilterIndex(DWORD *dwFilterIndex)
{
	*dwFilterIndex = ofn.nFilterIndex;
	return S_OK;
}
STDMETHODIMP CGetFileName::put_FileName(BSTR strFileName)
{
	StringCchCopy(ofn.lpstrFile, 0xFFFF, CW2T(strFileName));
	return S_OK;
}
STDMETHODIMP CGetFileName::get_FileName(BSTR *strFileName)
{
	*strFileName = SysAllocString(CT2W(ofn.lpstrFile));
	return S_OK;
}
STDMETHODIMP CGetFileName::put_InitDirectory(BSTR strInitDirectory)
{
	LPTSTR buffer = new TCHAR[wcslen(strInitDirectory)+1];
	StringCchCopy(buffer, wcslen(strInitDirectory)+1, CW2T(strInitDirectory));
	ofn.lpstrInitialDir = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::put_Title(BSTR strTitle)
{
	LPTSTR buffer = new TCHAR[wcslen(strTitle)+1];
	StringCchCopy(buffer, wcslen(strTitle)+1, CW2T(strTitle));
	ofn.lpstrTitle = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::put_Flags(DWORD dwFlags)
{
	ofn.Flags = dwFlags;
	return S_OK;
}
STDMETHODIMP CGetFileName::put_DefExt(BSTR strDefExt)
{
	LPTSTR buffer = new TCHAR[wcslen(strDefExt)+1];
	StringCchCopy(buffer, wcslen(strDefExt)+1, CW2T(strDefExt));
	ofn.lpstrDefExt = buffer;

	return S_OK;
}
STDMETHODIMP CGetFileName::DisplayOpen(BSTR *strFileName)
{
	if(GetOpenFileName(&ofn))
	{
		int i = 0;
		while(ofn.lpstrFile[i] != 0 && ofn.lpstrFile[i+1] != 0)
		{
			if(ofn.lpstrFile[i] == 0)
				ofn.lpstrFile[i] = '|';
			i++;
		}
        *strFileName = SysAllocString(CT2W(ofn.lpstrFile));
		return S_OK;
	}
	else
	{
		HRESULT hr = CommDlgExtendedError();
		return E_ABORT;
	}
}
STDMETHODIMP CGetFileName::DisplaySave(BSTR *strFileName)
{
	if(GetSaveFileName(&ofn))
	{
        *strFileName = SysAllocString(CT2W(ofn.lpstrFile));
		return S_OK;
	}
	else
	{
		HRESULT hr = CommDlgExtendedError();
		return E_ABORT;
	}
}
/* CChooseFont Implementation */
CChooseFont::CChooseFont()
{
	ZeroMemory(&lf, sizeof(lf));
	LONG lfWeight = FW_NORMAL;
	BYTE lfCharSet = DEFAULT_CHARSET;
	BYTE lfOutPrecision = OUT_DEFAULT_PRECIS;
	BYTE lfClipPrecision = CLIP_DEFAULT_PRECIS;
	BYTE lfQuality = DEFAULT_QUALITY;
	BYTE lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	lf.lfCharSet = OEM_CHARSET;
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = 0;
	cf.hDC = GetDC(0);
	cf.lpLogFont = &lf;
	cf.iPointSize = 0;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
	cf.rgbColors = 0;
	cf.lCustData = 0;
	cf.lpfnHook = 0;
	cf.lpTemplateName = 0;
	cf.hInstance = GetModuleHandle(NULL);
	cf.lpszStyle = 0;
	cf.nFontType = REGULAR_FONTTYPE;
	cf.nSizeMin = 10;
	cf.nSizeMax = 72;
}
CChooseFont::~CChooseFont()
{
	if(m_pWnd)
		m_pWnd->Release();
	else
		ATLASSERT(0);
}
STDMETHODIMP CChooseFont::SetWnd(IPdnWnd *pWnd)
{
	pWnd->QueryInterface(IID_IPdnWnd, (void **) &m_pWnd);
	pWnd->get_Handle((DWORD *) &cf.hwndOwner);
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontHeight(LONG *lfHeight)
{
	*lfHeight = lf.lfHeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontHeight(LONG lfHeight)
{
	lf.lfHeight = lfHeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontWidth(LONG *lfWidth)
{
	*lfWidth = lf.lfWidth;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontWidth(LONG lfWidth)
{
	lf.lfWidth = lfWidth;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontEscapement(LONG *lfEscapement)
{
	*lfEscapement = lf.lfEscapement;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontEscapement(LONG lfEscapement)
{
	lf.lfEscapement = lfEscapement;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontOrientation(LONG *lfOrientation)
{
	*lfOrientation = lf.lfOrientation;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontOrientation(LONG lfOrientation)
{
	lf.lfOrientation = lfOrientation;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontWeight(LONG *lfWeight)
{
	*lfWeight = lf.lfWeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontWeight(LONG lfWeight)
{
	lf.lfWeight = lfWeight;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontItalic(BYTE *lfItalic)
{
	*lfItalic = lf.lfItalic;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontItalic(BYTE lfItalic)
{
	lf.lfItalic = lfItalic;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontUnderline(BYTE *lfUnderline)
{
	*lfUnderline = lf.lfUnderline;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontUnderline(BYTE lfUnderline)
{
	lf.lfUnderline = lfUnderline;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontStrikeOut(BYTE *lfStrikeOut)
{
	*lfStrikeOut = lf.lfStrikeOut;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontStrikeOut(BYTE lfStrikeOut)
{
	lf.lfStrikeOut = lfStrikeOut;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontCharSet(BYTE *lfCharSet)
{
	*lfCharSet = lf.lfCharSet;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontCharSet(BYTE lfCharSet)
{
	lf.lfCharSet = lfCharSet;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontOutPrecision(BYTE *lfOutPrecision)
{
	*lfOutPrecision = lf.lfOutPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontOutPrecision(BYTE lfOutPrecision)
{
	lf.lfOutPrecision = lfOutPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontClipPrecision(BYTE *lfClipPrecision)
{
	*lfClipPrecision = lf.lfClipPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontClipPrecision(BYTE lfClipPrecision)
{
	lf.lfClipPrecision = lfClipPrecision;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontQuality(BYTE *lfQuality)
{
	*lfQuality = lf.lfQuality;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontQuality(BYTE lfQuality)
{
	lf.lfQuality = lfQuality;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontPitchAndFamily(BYTE *lfPitchAndFamily)
{
	*lfPitchAndFamily = lf.lfPitchAndFamily;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontPitchAndFamily(BYTE lfPitchAndFamily)
{
	lf.lfPitchAndFamily = lfPitchAndFamily;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontFace(BSTR *lfFaceName)
{
	*lfFaceName = SysAllocString(CT2W(lf.lfFaceName));
	return S_OK;
}
STDMETHODIMP CChooseFont::put_FontFace(BSTR lfFaceName)
{
	if(wcslen(lfFaceName) <(LF_FACESIZE - 1))
    	StringCchCopy(lf.lfFaceName, 32, CW2T(lfFaceName));
	return S_OK;
}
STDMETHODIMP CChooseFont::get_PointSize(INT *iPointSize)
{
	*iPointSize = cf.iPointSize / 10;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_PointSize(INT iPointSize)
{
	lf.lfHeight = - MulDiv(iPointSize, GetDeviceCaps(GetDC(0), LOGPIXELSY), 72);
	cf.iPointSize = iPointSize * 10;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_Flags(DWORD dwFlags)
{
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | dwFlags;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_Color(DWORD dwColor)
{
	cf.rgbColors = (COLORREF)dwColor;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_Color(DWORD *dwColor)
{
	*dwColor = cf.rgbColors;
	return S_OK;
}
STDMETHODIMP CChooseFont::get_FontType(WORD *dwType)
{
	*dwType = cf.nFontType;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_SizeMin(INT iSizeMin)
{
	cf.nSizeMin = iSizeMin;
	return S_OK;
}
STDMETHODIMP CChooseFont::put_SizeMax(INT iSizeMax)
{
	cf.nSizeMax = iSizeMax;
	return S_OK;
}
STDMETHODIMP CChooseFont::Display()
{
	if(ChooseFont(&cf))
		return S_OK;
	else
		return S_FALSE;
}
/* CComCtrl Implementation */
CComCtrl::CComCtrl()
{
	InitCommonControls();
	(new CComObject<CComDlg>)->QueryInterface(&m_pComDlg);
}
CComCtrl::~CComCtrl()
{
	if(m_pWnd)
		m_pWnd->Release();
	else
		ATLASSERT(0);
	m_pComDlg->Release();
}
STDMETHODIMP CComCtrl::SetWnd(IPdnWnd *pWnd)
{
	pWnd->QueryInterface(IID_IPdnWnd, (void **) &m_pWnd); 
	m_pComDlg->SetWnd(pWnd);
	return S_OK;
}
STDMETHODIMP CComCtrl::get_ComDlg(IDispatch **ppDisp)
{
	m_pComDlg->QueryInterface(IID_IDispatch, (void **)ppDisp);
	return S_OK;
}