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
 * Filename:    Shortcut.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file defines a COM class that offers the agent
 *              facilities for creating and editing Windows shortcuts.
 */
#include "stdafx.h"
#include "Shortcut.h"

_COM_SMARTPTR_TYPEDEF(IPersistFile, __uuidof(IPersistFile)); 

/*
 * Shortcut Constructor
 */
Shortcut::Shortcut()
{
	::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL,
		__uuidof(IShellLink), (LPVOID*) &m_ShellLink);
}

/*
 * Shortcut Destructor
 */
Shortcut::~Shortcut()
{
	m_ShellLink->Release();
}

/*
 * Saves the shortcut to the file pointed to by path.
 */
STDMETHODIMP Shortcut::Save(BSTR path)
{
	IPersistFilePtr pPersistFile(m_ShellLink);
	if(pPersistFile)
	{
		return pPersistFile->Save(path, TRUE);
	}
	else
	{
        return E_FAIL;
	}
}

/*
 * Loads the shortcut from a file pointed to by path.
 */
STDMETHODIMP Shortcut::Load(BSTR path)
{
	IPersistFilePtr pPersistFile(m_ShellLink);
	if(pPersistFile)
	{
		return pPersistFile->Load(path, STGM_READ);
	}
	else
	{
        return E_FAIL;
	}
}

/*
 * Getter for the command-line argument of the shortcut.
 */
STDMETHODIMP Shortcut::get_Arguments(BSTR *arguments)
{
	if(m_ShellLink)
	{
		TCHAR tArguments[INFOTIPSIZE] = TEXT("");
		HRESULT res = m_ShellLink->GetArguments(tArguments, INFOTIPSIZE);
		*arguments = ::SysAllocString(CT2W(tArguments));
		return res;
	}
	else
	{
		return E_FAIL;
	}

}

/*
 * Setter for the command-line argument of the shortcut.
 */
STDMETHODIMP Shortcut::put_Arguments(BSTR arguments)
{
	if(m_ShellLink)
	{
		return m_ShellLink->SetArguments(CW2T(arguments));
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Getter for the description of the shortcut.
 */
STDMETHODIMP Shortcut::get_Description(BSTR *description)
{
	if(m_ShellLink)
	{
		TCHAR tDescription[INFOTIPSIZE] = TEXT("");
		HRESULT res = m_ShellLink->GetDescription(tDescription, INFOTIPSIZE);
		*description = ::SysAllocString(CT2W(tDescription));
		return res;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Setter for the description of the shortcut.
 */
STDMETHODIMP Shortcut::put_Description(BSTR description)
{
	if(m_ShellLink)
	{
		return m_ShellLink->SetDescription(CW2T(description));
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Getter for the path to the icon of the shortcut.
 */
STDMETHODIMP Shortcut::get_IconLocation(BSTR *iconLocation)
{
	if(m_ShellLink)
	{
		TCHAR tIconLoc[MAX_PATH + 10] = TEXT("");
		int iIcon;
		HRESULT res = m_ShellLink->GetIconLocation(tIconLoc,
			MAX_PATH, &iIcon);
		StringCbPrintf(tIconLoc, MAX_PATH + 10, TEXT("%s,%d"),
			tIconLoc, iIcon);
		*iconLocation = SysAllocString(CT2W(tIconLoc));
		return res;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Setter for the path of the icon of the shortcut.
 * The argument has to contain an icon number separated by a comma.
 * e.g.: c:\path\to\icon.ico,4
 */
STDMETHODIMP Shortcut::put_IconLocation(BSTR iconLocation)
{
	LPWSTR comma = wcsrchr(iconLocation, L',');
	if(!comma)
	{
		return E_INVALIDARG;
	}
	else if(m_ShellLink)
	{
		*comma = 0;

		return m_ShellLink->SetIconLocation(CW2T(iconLocation), 
			atoi(CW2A(comma+1)));
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Getter for the path the shortcut points to.
 */
STDMETHODIMP Shortcut::get_Path(BSTR *path)
{
	if(m_ShellLink)
	{
		TCHAR tPath[MAX_PATH] = TEXT("");
		HRESULT res = m_ShellLink->GetPath(tPath,
			MAX_PATH, NULL, SLGP_UNCPRIORITY);
		*path = ::SysAllocString(CT2W(tPath));
		return res;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Setter for the path the shortcut points to.
 */
STDMETHODIMP Shortcut::put_Path(BSTR path)
{
	if(m_ShellLink)
	{
		return m_ShellLink->SetPath(CW2T(path));
	}
	else
	{
		return E_FAIL;
	}
}
