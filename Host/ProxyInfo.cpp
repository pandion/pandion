#include "stdafx.h"
#include "ProxyInfo.h"

CProxyInfo::CProxyInfo()
{
	m_Address = "";
	m_Port = 0;
	m_Username = "";
	m_Password = "";
}

void CProxyInfo::SetAddress( BSTR address )
{
	if( wcslen( address ) < 128 )
		m_Address = address;
}
void CProxyInfo::SetPort( USHORT port )
{
	m_Port = port;
}
void CProxyInfo::SetUsername( BSTR username )
{
	if( wcslen( username ) < 128 )
		m_Username = username;
}
void CProxyInfo::SetPassword( BSTR password )
{
	if( wcslen( password ) < 128 )
		m_Password = password;
}

void CProxyInfo::GetAddress( BSTR *address )
{
	*address = m_Address.Copy();
}
USHORT CProxyInfo::GetPort()
{
	return m_Port;
}
void CProxyInfo::GetUsername( BSTR *username )
{
	*username = m_Username.Copy();
}
void CProxyInfo::GetPassword( BSTR *password )
{
	*password = m_Password.Copy();
}