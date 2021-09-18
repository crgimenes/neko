/************************************
 
   Tray Class: Very simple interface
   onto the system tray for Neko95

 *************************************/

#include <windows.h>
#include <string.h>
#include "tray.h"
#include "resource.h"

BOOL CTray::AddIcon( HWND hWnd, HICON hIcon, UINT uID )
{
    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    //fill in the structure
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hWnd;
    tnid.uID = uID;
    tnid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP; 
    tnid.uCallbackMessage = MY_NOTIFYICON; 
    tnid.hIcon = hIcon;
    strncpy( tnid.szTip, "Neko", sizeof(tnid.szTip) ); 

    //give the command
    res = Shell_NotifyIcon( NIM_ADD, &tnid ); 
 
    //delete the icon
    DestroyIcon( tnid.hIcon ); 
 
    //increment the counter
    if( res ) m_uIconCount++;

    return res; 
}

BOOL CTray::RemoveIcon( HWND hWnd, UINT uID )
{
    BOOL res;
    NOTIFYICONDATA tnid;

    //fill in the structure
    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd = hWnd;
    tnid.uID = uID;

    //give the notify command
    res = Shell_NotifyIcon( NIM_DELETE, &tnid );

    //decrement the counter
    if( res ) m_uIconCount--;

    return res;
}

void CTray::ShowPopupMenu( HWND hWnd )
{
    //get the mouse position
    POINT mouse;
    GetCursorPos( &mouse );

    //create the popup menu
    HMENU hMenu = CreatePopupMenu();
    AppendMenu( hMenu, MF_STRING, ID_SETTINGS,"&Settings" );
    AppendMenu( hMenu, MF_STRING, ID_EXIT,    "E&xit" );
    SetMenuDefaultItem( hMenu, ID_SETTINGS, FALSE );

    //display the menu
    TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, mouse.x, mouse.y, 0, hWnd, NULL );
 
    //destroy it
    DestroyMenu(hMenu); 
}

