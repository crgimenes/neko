/************************************
 
    Neko's configuration header file

 *************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NekoCommon.h"

class CNekoSettings {
public:
    CNekoSettings( LPCSTR key, BOOL fCreate = TRUE );
    ~CNekoSettings();

	BOOL IsOpen();

    BOOL GetString( LPCSTR keyname, LPSTR result, ULONG size );
    BOOL GetInt( LPCSTR keyname, DWORD * result );
    BOOL GetBool( LPCSTR keyname, BOOL * result );

    BOOL SetString( LPCSTR keyname, LPSTR value );
    BOOL SetInt( LPCSTR keyname, DWORD value );
    BOOL SetBool( LPCSTR keyname, BOOL value );

private:
    HKEY m_hKey;
};
