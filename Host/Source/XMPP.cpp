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
 * Filename:    XMPP.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "XMPP.h"
#include "MainWnd.h"

static PARAMDATA SetProxyServerParamData[] =
{
	{L"server", VT_BSTR},
	{L"port", VT_UI2},
	{L"username", VT_BSTR},
	{L"password", VT_BSTR}
};

static PARAMDATA SetProxyPollURLParamData =
{
	L"pollURL", VT_BSTR
};

static PARAMDATA ConnectParamData[] =
{
	{L"server", VT_BSTR},
	{L"port", VT_UI2},
	{L"useSSL", VT_BOOL},
	{L"proxyMethod", VT_UI4}
};

static PARAMDATA SendXMLParamData =
{
	L"pDisp", VT_DISPATCH
};

static PARAMDATA SendTextParamData =
{
	L"strText", VT_BSTR
};

static PARAMDATA SetHandlerParamData =
{
	L"handler", VT_BSTR
};

static METHODDATA XMPPMethodData[] = 
{
	{L"SetProxyServer",				SetProxyServerParamData,
		8, 8, CC_STDCALL, 4, DISPATCH_METHOD, VT_EMPTY},
	{L"SetProxyPollURL",			&SetProxyPollURLParamData,
		9, 9, CC_STDCALL, 1, DISPATCH_METHOD, VT_EMPTY},
	{L"ConnectionIP",				NULL,
		10, 10, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR | VT_BYREF},
	{L"Connect",					ConnectParamData,
		11, 11, CC_STDCALL, 4, DISPATCH_METHOD, VT_EMPTY},
	{L"Disconnect",					NULL,
		12, 12, CC_STDCALL, 0, DISPATCH_METHOD, VT_EMPTY},
	{L"StartTLS",					NULL,
		13, 13, CC_STDCALL, 0, DISPATCH_METHOD, VT_EMPTY},
	{L"StartSC",					NULL,
		14, 14, CC_STDCALL, 0, DISPATCH_METHOD, VT_EMPTY},
	{L"SendXML",					&SendXMLParamData,
		15, 15, CC_STDCALL, 1, DISPATCH_METHOD, VT_EMPTY},
	{L"SendText",					&SendTextParamData,
		16, 16, CC_STDCALL, 1, DISPATCH_METHOD, VT_EMPTY},
	{L"ConnectedHandler",			&SetHandlerParamData,
		17, 17, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"DisconnectedHandler",		&SetHandlerParamData,
		18, 18, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"IQHandler",					&SetHandlerParamData,
		19, 19, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"MessageHandler",				&SetHandlerParamData,
		20, 20, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"PresenceHandler",			&SetHandlerParamData,
		21, 21, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"DocumentStartHandler",		&SetHandlerParamData,
		22, 22, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"DocumentEndHandler",			&SetHandlerParamData,
		23, 23, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"StreamHandler",				&SetHandlerParamData,
		24, 24, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"StartTLSSucceededHandler",	&SetHandlerParamData,
		25, 25, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"StartTLSFailedHandler",		&SetHandlerParamData,
		26, 26, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"StartSCSucceededHandler",	&SetHandlerParamData,
		27, 27, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY},
	{L"StartSCFailedHandler",		&SetHandlerParamData,
		28, 28, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_EMPTY}
};

static INTERFACEDATA XMPPInterfaceData =
{
	XMPPMethodData, sizeof(XMPPMethodData)
};

/*
 * Constructor
 */
CXMPP::CXMPP() :
	m_Handlers(), m_Logger(), m_ConnectionManager(m_Handlers, m_Logger)
{
	m_COMReferenceCounter = 0;
	::CreateDispTypeInfo(&XMPPInterfaceData,
		LOCALE_SYSTEM_DEFAULT, &m_TypeInfo);
}

/*
 * Destructor
 */
CXMPP::~CXMPP()
{
	m_TypeInfo->Release();
}

/*
 *
 */
void CXMPP::SetMainWnd(CMainWnd* pMainWnd)
{
	m_Handlers.SetMainWindow((CMainWnd*) pMainWnd);
}

STDMETHODIMP CXMPP::QueryInterface(REFIID riid, void** ppvObject)
{
	if(ppvObject == NULL)
	{
		return E_POINTER;
	}
	else if(riid == IID_IUnknown ||
		riid == IID_IDispatch)
	{
		*ppvObject = (void*) this;
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) CXMPP::AddRef()
{
	return ++m_COMReferenceCounter;
}

/*
 * IUnknown::Release() implementation.
 * Does not actually delete the object as this object is only created once on
 * the stack.
 */
STDMETHODIMP_(ULONG) CXMPP::Release()
{
	if(m_COMReferenceCounter > 1)
	{
		return --m_COMReferenceCounter;
	}
	else
	{
		return 0;
	}
}

STDMETHODIMP CXMPP::GetTypeInfoCount(UINT* pctinfo)
{
	if(pctinfo == NULL)
	{
		return E_POINTER;
	}
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP CXMPP::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	if(ppTInfo == NULL)
	{
		return E_POINTER;
	}
	m_TypeInfo->AddRef();
	*ppTInfo = m_TypeInfo;
	return S_OK;
}

STDMETHODIMP CXMPP::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames,
	UINT cNames, LCID lcid, DISPID* rgDispId)
{
	return DispGetIDsOfNames(m_TypeInfo, rgszNames, cNames, rgDispId);
}

STDMETHODIMP CXMPP::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, 
	WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult,
	EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return DispInvoke(this, m_TypeInfo, dispidMember, wFlags, pDispParams,
		pVarResult, pExcepInfo, puArgErr);
}

/*
 *
 */
STDMETHODIMP CXMPP::SetProxyServer(BSTR server, USHORT port, 
								   BSTR username, BSTR password)
{
	m_ConnectionManager.SetProxyServer(server, port, username, password);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SetProxyPollURL(BSTR pollURL)
{
	m_ConnectionManager.SetProxyPollURL(std::wstring(pollURL));
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::get_ConnectionIP(BSTR *strIP)
{
	*strIP = ::SysAllocString(m_ConnectionManager.GetConnectionIP().c_str());
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::Connect(BSTR server,
	USHORT port, BOOL useSSL, DWORD proxyMethod)
{
	if(proxyMethod == 0)
	{
		m_ConnectionManager.Connect(server, port, useSSL, ProxyMethodDontUse);
	}
	else if(proxyMethod == 1)
	{
		m_ConnectionManager.Connect(server, port, useSSL, ProxyMethodConnect);
	}
	else if(proxyMethod == 3)
	{
		m_ConnectionManager.Connect(server, port, useSSL, ProxyMethodPoll);
	}
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::Disconnect()
{
	m_ConnectionManager.Disconnect();
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::StartTLS()
{
	m_ConnectionManager.StartTLS();
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::StartSC()
{
	m_ConnectionManager.StartSC();
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SendXML(IDispatch* pDisp)
{
	MSXML2::IXMLDOMDocument *pXMLDoc = NULL;
	BSTR xmlString;

	if(pDisp != NULL &&
		SUCCEEDED(pDisp->QueryInterface(IID_IXMLDOMDocument,
			(LPVOID*) &pXMLDoc)) &&
		SUCCEEDED(pXMLDoc->get_xml(&xmlString)))
	{
		m_ConnectionManager.SendText(xmlString);

		::SysFreeString(xmlString);
		pXMLDoc->Release();
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SendText(BSTR strText)
{
	m_ConnectionManager.SendText(std::wstring(strText));
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_ConnectedHandler(BSTR handler)
{
	m_Handlers.SetConnectedHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_DisconnectedHandler(BSTR handler)
{
	m_Handlers.SetDisconnectedHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_IQHandler(BSTR handler)
{
	m_Handlers.SetIQHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_MessageHandler(BSTR handler)
{
	m_Handlers.SetMessageHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_PresenceHandler(BSTR handler)
{
	m_Handlers.SetPresenceHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_DocumentStartHandler(BSTR handler)
{
	m_Handlers.SetDocumentStartHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_DocumentEndHandler(BSTR handler)
{
	m_Handlers.SetDocumentEndHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StreamHandler(BSTR handler)
{
	m_Handlers.SetStreamHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartTLSSucceededHandler(BSTR handler)
{
	m_Handlers.SetStartTLSSucceededHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartTLSFailedHandler(BSTR handler)
{
	m_Handlers.SetStartTLSFailedHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartSCSucceededHandler(BSTR handler)
{
	m_Handlers.SetStartSCSucceededHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartSCFailedHandler(BSTR handler)
{
	m_Handlers.SetStartSCFailedHandler(handler);
	return S_OK;
}
