// DesktopHack.cpp: implementation of the CDesktopHack class.
//
//////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "DesktopHack.h"

//constants used by callback function
#define ENUM_ALLWINDOWS     1
#define ENUM_SHELLDEFVIEW   2
#define ENUM_GETDESKTOP     3

//variables used by callback function
HWND g_hWndDesktop = NULL;
HWND g_hWndShellDefView = NULL;
BOOL g_fActiveDesktop = FALSE;

//callback function prototype
BOOL CALLBACK DesktopHunter( HWND hWnd, LPARAM lParam );



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDesktopHack::CDesktopHack()
{
	//find the desktop
	FindDesktopHandle();
}

CDesktopHack::~CDesktopHack()
{

}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CDesktopHack::FindDesktopHandle()
{
    //reset everything
	g_hWndDesktop = NULL;
	g_hWndShellDefView = NULL;
	g_fActiveDesktop = FALSE;
	m_fNoChicagoDesktop = FALSE;

    //enumerate all of the windows
    EnumWindows( (WNDENUMPROC)DesktopHunter, ENUM_ALLWINDOWS );

    //if no desktop was found, we're running on a non-chicago style shell (win32s/NT3)
    if( g_hWndDesktop == NULL )
    {
        m_fNoChicagoDesktop = TRUE;
        g_hWndDesktop = GetDesktopWindow();
    }

	//store the handle and whether it's an active desktop or not
	m_hWndDesktop = g_hWndDesktop;
	m_fActiveDesktop = g_fActiveDesktop;
}

void CDesktopHack::GetDesktopRect( RECT& rcDesktop )
{
	//retrieve the screen rect of the desktop
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesktop, FALSE );
}




//////////////////////////////////////////////////////////////////////
// Callback Function
//////////////////////////////////////////////////////////////////////

BOOL CALLBACK DesktopHunter( HWND hWnd, LPARAM lParam )
{
    char szBuffer[26];

    switch( lParam )
    {
        case ENUM_ALLWINDOWS: 
			//try and find program manager window
            GetClassName( hWnd, szBuffer, 25 );
            if( stricmp( szBuffer, "Progman" ) == 0 )
            {
                EnumChildWindows( hWnd, (WNDENUMPROC)DesktopHunter, ENUM_SHELLDEFVIEW );
                return ( g_hWndDesktop == NULL ); //keep looking if it's not found
            }
            break;

        case ENUM_SHELLDEFVIEW:
            GetClassName( hWnd, szBuffer, 25 );
            if( stricmp( szBuffer, "SHELLDLL_DefView" ) == 0 )
            {
                g_hWndShellDefView = hWnd;
                EnumChildWindows( hWnd, (WNDENUMPROC)DesktopHunter, ENUM_GETDESKTOP );
                return FALSE;
            }
            break;

        case ENUM_GETDESKTOP: 
            //only look at child windows of the shell default view
            if( GetParent( hWnd ) != g_hWndShellDefView ) break;

            GetClassName( hWnd, szBuffer, 25 );

            //check for normal desktop
            if( stricmp( szBuffer, "SysListView32" ) == 0 )
            {
				//store handle but continue looking
                g_hWndDesktop = hWnd;
                return TRUE;
            }

            //check for IE4
            if( stricmp( szBuffer, "Internet Explorer_Server" ) == 0 )
            {
                g_hWndDesktop = hWnd;
                g_fActiveDesktop = TRUE;
                return FALSE;
            }
            break;
    }
    return TRUE;
}
