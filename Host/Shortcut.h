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
 * Filename:    Shortcut.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares a COM class that offers the agent
 *              facilities for creating and editing Windows shortcuts.
 */
#pragma once

/*
 * This COM class provides the agent with facilities for editing and creating
 * Windows shortcuts.
 */
class Shortcut :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IShortcut>
{
	/*
	 * A pointer to an IShellLink object used to access shortcut properties.
	 */
	CComPtr<IShellLink> m_pShellLink;
public:
	Shortcut();
	~Shortcut();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(Shortcut)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IShortcut)
	END_COM_MAP()

	STDMETHOD(Save)(BSTR path);
	STDMETHOD(Load)(BSTR path);

	STDMETHOD(get_Arguments)(BSTR *arguments);
	STDMETHOD(put_Arguments)(BSTR arguments);

	STDMETHOD(get_Description)(BSTR *description);
	STDMETHOD(put_Description)(BSTR description);

	STDMETHOD(get_IconLocation)(BSTR *iconLocation);
	STDMETHOD(put_IconLocation)(BSTR iconLocation);

	STDMETHOD(get_Path)(BSTR *path);
	STDMETHOD(put_Path)(BSTR path);
};
