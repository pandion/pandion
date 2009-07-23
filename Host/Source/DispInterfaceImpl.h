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
 * Filename:    DispInterfaceImpl.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: Implementation of the DispInterfaceImpl template.
 */
#pragma once

/*
 * This template implements an IUnknown and an IDispatch interface. 
 */
template<class T> class DispInterfaceImpl :
	public T
{
private:
	/*
	 * The internal COM reference counter, used by the IUnknown implementation.
	 */
	unsigned long			m_COMReferenceCounter;
	/*
	 * This object's TypeInfo, used by the IDispatch implementation.
	 */
	ITypeInfo*				m_TypeInfo;
public:
	/*
	 * Constructor
	 */
	DispInterfaceImpl()
	{
		m_COMReferenceCounter = 0;

		TCHAR moduleFileName[MAX_PATH];
		::GetModuleFileName(NULL, moduleFileName, MAX_PATH);

		ITypeLib *typeLib;
		::LoadTypeLibEx(moduleFileName, REGKIND_NONE, &typeLib);
		typeLib->GetTypeInfoOfGuid(__uuidof(T),&m_TypeInfo);
		typeLib->Release();
	}

	/*
	 * Destructor
	 */
	~DispInterfaceImpl()
	{
		m_TypeInfo->Release();
	}

	/*
	 * IUnknown::QueryInterface() implementation
	 */
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		if(ppvObject == NULL)
		{
			return E_POINTER;
		}
		else if(riid == IID_IUnknown || riid == IID_IDispatch)
		{
			*ppvObject = (void*) this;
			AddRef();
			return S_OK;
		}
		else
		{
			return E_NOINTERFACE;
		}
	}

	/*
	 * IUnknown::AddRef() implementation
	 */
	STDMETHODIMP_(ULONG) AddRef()
	{
		return ++m_COMReferenceCounter;
	}

	/*
	 * IUnknown::Release() implementation
	 */
	STDMETHODIMP_(ULONG) Release()
	{
		if(m_COMReferenceCounter > 1)
		{
			return --m_COMReferenceCounter;
		}
		else
		{
			delete this;
			return 0;
		}
	}

	/*
	 * IDispatch::GetTypeInfoCount() implementation
	 */
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo)
	{
		if(pctinfo == NULL)
		{
			return E_POINTER;
		}
		*pctinfo = 1;
		return S_OK;
	}

	/*
	 * IDispatch::GetTypeInfo() implementation
	 */
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{
		if(ppTInfo == NULL)
		{
			return E_POINTER;
		}
		m_TypeInfo->AddRef();
		*ppTInfo = m_TypeInfo;
		return S_OK;
	}

	/*
	 * IDispatch::GetIDsOfNames() implementation
	 */
	STDMETHODIMP GetIDsOfNames(REFIID riid,
		LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
	{
		return DispGetIDsOfNames(m_TypeInfo, rgszNames, cNames, rgDispId);
	}

	/*
	 * IDispatch::Invoke() implementation
	 */
	STDMETHODIMP Invoke(
		DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, 
		DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, 
		UINT* puArgErr)
	{
		HRESULT hr = DispInvoke(this, m_TypeInfo, dispidMember, wFlags,
			pDispParams, pVarResult, pExcepInfo, puArgErr);
		return hr;
	}
};
