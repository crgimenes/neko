/************************************
 
    Neko's configuration

 *************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NekoSettings.h"

CNekoSettings::CNekoSettings( LPCSTR key, BOOL fCreate /*TRUE*/ )
{
	m_hKey = NULL;
	if( fCreate )
	{
		DWORD dwDisposition;
		RegCreateKeyEx( HKEY_CURRENT_USER, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition );
	}
	else
	{
		if( RegOpenKeyEx( HKEY_CURRENT_USER, key, 0, KEY_READ|KEY_WRITE, &m_hKey ) != ERROR_SUCCESS )
			m_hKey = NULL;
	}
}

CNekoSettings::~CNekoSettings()
{
    if( m_hKey ) RegCloseKey( m_hKey );
}

BOOL CNekoSettings::IsOpen()
{
	return( m_hKey != NULL );
}

BOOL CNekoSettings::GetString( LPCSTR keyname, LPSTR result, ULONG size )
{
    DWORD dwType;
    return ( RegQueryValueEx( m_hKey, keyname, NULL, &dwType, (LPBYTE)result, &size ) == ERROR_SUCCESS );
}
    
BOOL CNekoSettings::GetInt( LPCSTR keyname, DWORD * result )
{
    DWORD dwType;
    ULONG ulSize = sizeof(DWORD);
    return ( RegQueryValueEx( m_hKey, keyname, NULL, &dwType, (LPBYTE)result, &ulSize ) == ERROR_SUCCESS );
}

BOOL CNekoSettings::GetBool( LPCSTR keyname, BOOL * result )
{
    DWORD dwType;
    ULONG ulSize = sizeof(BOOL);
    return ( RegQueryValueEx( m_hKey, keyname, NULL, &dwType, (LPBYTE)result, &ulSize ) == ERROR_SUCCESS );
}


BOOL CNekoSettings::SetString( LPCSTR keyname, LPSTR value )
{
    ULONG ulSize = strlen( value ) + 1;
    return ( RegSetValueEx( m_hKey, keyname, NULL, REG_SZ, (LPBYTE)value, ulSize) == ERROR_SUCCESS );
}

BOOL CNekoSettings::SetInt( LPCSTR keyname, DWORD value )
{
    ULONG ulSize = sizeof(DWORD);
    return ( RegSetValueEx( m_hKey, keyname, NULL, REG_DWORD, (LPBYTE)&value, ulSize ) == ERROR_SUCCESS );
}

BOOL CNekoSettings::SetBool( LPCSTR keyname, BOOL value )
{
    ULONG ulSize = sizeof(BOOL);
    return ( RegSetValueEx( m_hKey, keyname, NULL, REG_BINARY, (LPBYTE)&value, ulSize ) == ERROR_SUCCESS );
}
