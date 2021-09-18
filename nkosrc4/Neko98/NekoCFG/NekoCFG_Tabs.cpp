/* property sheet functions */
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "NekoCFG.h"
#include "..\NekoCommon.h"
#include "NekoCFG_resource.h"
#include "NekoCFG_resource.hm"


/* help ID lookup table */
static DWORD dwHelpID[] = { 
    IDC_SCALESLIDER,    HIDC_SCALESLIDER,
    IDC_SCALEDISPLAY,   HIDC_SCALEDISPLAY,
    IDC_SET100,         HIDC_SET100,
    IDC_IMAGELIB,       HIDC_IMAGELIB,
    IDC_CHANGE,         HIDC_CHANGE,
    IDC_DEFAULT,        HIDC_DEFAULT,

    IDC_SPEEDSLIDER,    HIDC_SPEEDSLIDER,
    IDC_SENSESLIDER,    HIDC_SENSESLIDER,

    IDC_SOUNDFREQ,      HIDC_SOUNDFREQ,
    IDC_SOUNDSAVAIL,    HIDC_SOUNDSAVAIL,
    IDC_SOUNDNAME,      HIDC_SOUNDNAME,
    IDC_PREVIEW,        HIDC_PREVIEW,
    IDC_NONE,           HIDC_NONE,
    IDC_BROWSE,         HIDC_BROWSE,

    IDC_ACTION,         HIDC_ACTION,
    IDC_ACTIONDESC,     HIDC_ACTIONDESC,

	IDC_ALWAYSONTOP,	HIDC_ALWAYSONTOP,
    IDC_FOOTPRINTS,     HIDC_FOOTPRINTS,
    0,0
};


/**************************************************************************************/

//cat settings
LPCATSETTINGS g_lpCurrentCat = NULL;
#define MY_WRITESETTINGS (WM_USER+32)
#define MY_READSETTINGS  (WM_USER+33)

/**************************************************************************************/
//tab control data

#define NUM_PAGES 5
BOOL CALLBACK PropPage_Display( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Movement( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Sound( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Independence( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Effects( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
TABINITDATA tibTabs[NUM_PAGES] = {
    { "Display",        IDD_DISPLAY,        (DLGPROC)PropPage_Display,      IDI_TAB_DISPLAY,        NULL },
    { "Movement",       IDD_MOVEMENT,       (DLGPROC)PropPage_Movement,     IDI_TAB_MOVEMENT,       NULL },
    { "Sound",          IDD_SOUND,          (DLGPROC)PropPage_Sound,        IDI_TAB_SOUNDS,         NULL },
    { "Independence",   IDD_INDEPENDENCE,   (DLGPROC)PropPage_Independence, IDI_TAB_INDEPENDENCE,   NULL },
    { "Effects",        IDD_EFFECTS,        (DLGPROC)PropPage_Effects,      IDI_TAB_EFFECTS,        NULL },
};

HWND g_hDlgDisplay = NULL;      //current child dialog
HWND g_hWndTab = NULL;          //tab control
RECT g_rcTabStart;              //offset in dialog to draw tab pages

HIMAGELIST g_himgTabIcons = NULL;

/**************************************************************************************/


/* InitialisePropertyDialog - loads all of the property sheet pages into the property sheet control */
void WINAPI InitialisePropertyDialog( HWND hWnd )
{
    int i;

    //create the image list
    g_himgTabIcons = ImageList_Create( 16, 16, ILC_COLOR4|ILC_MASK, NUM_PAGES, 0 );
    for( i = 0; i < NUM_PAGES; i++ )
    {
        HICON hIcon = (HICON)LoadImage( g_hInstance, MAKEINTRESOURCE(tibTabs[i].uIDIcon), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE );
        tibTabs[i].uIDIcon = ImageList_AddIcon( g_himgTabIcons, hIcon );
        DestroyIcon( hIcon );
    }
    TabCtrl_SetImageList( hWnd, g_himgTabIcons );


    //insert tabs for each of the dialog items and create the (hidden) dialog
    TC_ITEM tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    for( i = 0; i < NUM_PAGES; i++ )
    {
        tie.iImage = tibTabs[i].uIDIcon;
        tie.pszText = tibTabs[i].szTitle;
        TabCtrl_InsertItem( hWnd, i, &tie );

        tibTabs[i].hDlg = CreateDialog( g_hInstance, MAKEINTRESOURCE(tibTabs[i].uIDDialog), GetParent(hWnd), tibTabs[i].lpfnDlgProc );
    }

    //calculate where to stick each dialog
    GetWindowRect( hWnd, &g_rcTabStart );
    ScreenToClient( GetParent(hWnd), (LPPOINT)&g_rcTabStart.left );
    ScreenToClient( GetParent(hWnd), (LPPOINT)&g_rcTabStart.right );
    g_rcTabStart.top += 42;
    g_rcTabStart.left += 3;

    //store the handle and show the first page
    g_hWndTab = hWnd;
    OnSelChanged( hWnd );
}

/* ShutdownPropertyDialog - releases any resources owned by the propery sheet */
void WINAPI ShutdownPropertyDialog()
{
    ImageList_Destroy( g_himgTabIcons );
}

/* OnTabSelection - updates the tabs when one is selected */
void WINAPI OnSelChanged( HWND hWnd ) 
{ 
    int iSel = TabCtrl_GetCurSel( hWnd ); 
 
    //destroy the current child dialog box, if any. 
    if( g_hDlgDisplay != NULL) ShowWindow( g_hDlgDisplay, SW_HIDE ); 
 
    //create the new child dialog box. 
    g_hDlgDisplay = tibTabs[iSel].hDlg;
    ShowWindow( g_hDlgDisplay, SW_SHOW );
    
}

/* SetActiveCat - selects which cat to configure */
void WINAPI SetActiveCat( LPCATSETTINGS lpCat )
{
    GetDialogSettings();
    g_lpCurrentCat = lpCat;
    for( int i = 0; i < NUM_PAGES; i++ ) SendMessage( tibTabs[i].hDlg, MY_READSETTINGS, 0, 0 );
}

/* GetDialogSettings - causes all property pages to write their settings into the data structure */
void WINAPI GetDialogSettings()
{
    if( g_lpCurrentCat ) 
        for( int i = 0; i < NUM_PAGES; i++ ) SendMessage( tibTabs[i].hDlg, MY_WRITESETTINGS, 0, 0 );
}

/**************************************************************************************/

/* PropPage_Display - message processsing function for the 'display' property page */
BOOL CALLBACK PropPage_Display( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessage( hPage, IDC_SCALESLIDER, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SCALE/10,MAX_SCALE/10) );
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uScale = SendDlgItemMessage( hPage, IDC_SCALESLIDER, TBM_GETPOS, 0, 0 ) * 10;
            GetDlgItemText( hPage, IDC_IMAGELIB, g_lpCurrentCat->szLibrary, MAX_PATH );
			g_lpCurrentCat->fAlwaysOnTop = IsDlgButtonChecked( hPage, IDC_ALWAYSONTOP );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessage( hPage, IDC_SCALESLIDER, TBM_SETPOS, TRUE, g_lpCurrentCat->uScale / 10 );
            SetDlgItemText( hPage, IDC_IMAGELIB, g_lpCurrentCat->szLibrary );
			CheckDlgButton( hPage, IDC_ALWAYSONTOP, g_lpCurrentCat->fAlwaysOnTop );
            SendMessage( hPage, WM_HSCROLL, 0, 0 );
            break;

        case WM_HSCROLL:
        {
            //update the % indicator
            char szBuffer[32];
            sprintf( szBuffer, "%d%%", SendDlgItemMessage( hPage, IDC_SCALESLIDER, TBM_GETPOS, 0, 0 ) * 10 );
            SetDlgItemText( hPage, IDC_SCALEDISPLAY, szBuffer );
            break;
        }

        case WM_COMMAND:
        
            switch( LOWORD(wParam) )
            {
                case IDC_SET100:
                    SendDlgItemMessage( hPage, IDC_SCALESLIDER, TBM_SETPOS, TRUE, 10 );
                    SendMessage( hPage, WM_HSCROLL, 0, 0 );
                    break;

                case IDC_DEFAULT:
                    SetDlgItemText( hPage, IDC_IMAGELIB, "" );
                    break;

                case IDC_CHANGE:
                {
                    OPENFILENAME ofn;
                    char lpstrLibrary[MAX_PATH];
                    LPCTSTR lpstrFileMasks = "Icon Libraries\0*.icl;*.exe;*.dll\0All Files (*.*)\0*.*\0\0";

                    //get the current file
                    GetDlgItemText( hPage, IDC_IMAGELIB, lpstrLibrary, MAX_PATH-1 );
                    
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hPage;
                    ofn.hInstance = NULL;
                    ofn.lpstrFilter = lpstrFileMasks;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = lpstrLibrary;
                    ofn.nMaxFile = MAX_PATH-1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = "Select Image Library";
                    ofn.Flags = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                    ofn.lpstrDefExt = "icl";
                    if( GetOpenFileName( &ofn ) )
                    {
                        //check to make sure it's got enough icons in...
                        if( (UINT)ExtractIcon( g_hInstance, ofn.lpstrFile, (UINT)-1 ) < 32 )
                            MessageBox( hPage, "That file does not have enough icons in it - it must have at least 32", "Change Image Library", MB_ICONEXCLAMATION );
                        else
                            SetDlgItemText( hPage, IDC_IMAGELIB, ofn.lpstrFile );
                    }
                    break;
                }
            }
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE ); 
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (DWORD)(LPSTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelp( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (DWORD)(LPVOID)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}


/* PropPage_Movement - message processsing function for the 'movement' property page */
BOOL CALLBACK PropPage_Movement( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessage( hPage, IDC_SPEEDSLIDER, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SPEED,MAX_SPEED) );
            SendDlgItemMessage( hPage, IDC_SENSESLIDER, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SENSE,MAX_SENSE) );
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uSpeed = SendDlgItemMessage( hPage, IDC_SPEEDSLIDER, TBM_GETPOS, 0, 0 );
            g_lpCurrentCat->uMouseSensitivity = MAX_SENSE - SendDlgItemMessage( hPage, IDC_SENSESLIDER, TBM_GETPOS, 0, 0 );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessage( hPage, IDC_SPEEDSLIDER, TBM_SETPOS, TRUE, g_lpCurrentCat->uSpeed );
            SendDlgItemMessage( hPage, IDC_SENSESLIDER, TBM_SETPOS, TRUE, MAX_SENSE - g_lpCurrentCat->uMouseSensitivity );
            break;


        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE ); 
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (DWORD)(LPSTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelp( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (DWORD)(LPVOID)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

/* PropPage_Sound - message processsing function for the 'sound' property page */
BOOL CALLBACK PropPage_Sound( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    static HCURSOR s_hcPlaying = NULL;

    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessage( hPage, IDC_PREVIEW, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadImage( g_hInstance, MAKEINTRESOURCE(IDR_PLAYBITMAP), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS ) );
            SendDlgItemMessage( hPage, IDC_SOUNDFREQ, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SNDFREQ,MAX_SNDFREQ) );

            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)"Idle One" );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)"Idle Two" );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)"Idle Three" );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)"Sleep" );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)"Awake" );

            //load the playing cursor
            s_hcPlaying = LoadCursor( g_hInstance, MAKEINTRESOURCE(IDC_PLAYING) );

            //select first item in the list & update
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_SETCURSEL, 0, 0 );
            break;

        case WM_DESTROY:
            DestroyCursor( s_hcPlaying );
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uSndFrequency = SendDlgItemMessage( hPage, IDC_SOUNDFREQ, TBM_GETPOS, 0, 0 );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessage( hPage, IDC_SOUNDFREQ, TBM_SETPOS, TRUE, g_lpCurrentCat->uSndFrequency );

            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 0, (LPARAM)g_lpCurrentCat->szSndIdle1 );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 1, (LPARAM)g_lpCurrentCat->szSndIdle2 );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 2, (LPARAM)g_lpCurrentCat->szSndIdle3 );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 3, (LPARAM)g_lpCurrentCat->szSndSleep );
            SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 4, (LPARAM)g_lpCurrentCat->szSndAwake );
            PostMessage( hPage, WM_COMMAND, MAKEWPARAM( IDC_SOUNDSAVAIL, 0 ), 0 );
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_SOUNDSAVAIL:
                {
                    int iCurSel = SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );
                    char * lpszFile = (char*)SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 );

                    if( strcmp( lpszFile, "" ) == 0 )
                    {
                        SetDlgItemText( hPage, IDC_SOUNDNAME, "" );
                        EnableWindow( GetDlgItem( hPage, IDC_PREVIEW ), FALSE );
                        EnableWindow( GetDlgItem( hPage, IDC_NONE ), FALSE );
                    }
                    else
                    {
                        SetDlgItemText( hPage, IDC_SOUNDNAME, lpszFile );
                        EnableWindow( GetDlgItem( hPage, IDC_PREVIEW ), TRUE );
                        EnableWindow( GetDlgItem( hPage, IDC_NONE ), TRUE );
                    }

                    break;
                }

                case IDC_PREVIEW:
                {
                    int iCurSel = SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );
                    char * lpszFile = (char*)SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 );

                    if( strcmp( lpszFile, "" ) != 0 )
                    {
                        SetCursor( s_hcPlaying );
                        if( !PlaySound( lpszFile, NULL, SND_FILENAME|SND_NODEFAULT|SND_SYNC ) ) MessageBox( hPage, "Unable to start playing wave file!", "Preview Sound", MB_ICONEXCLAMATION );
                        SetCursor( LoadCursor( NULL, MAKEINTRESOURCE(IDC_ARROW) ) );
                    }
                    break;
                }

                case IDC_NONE:
                {
                    int iCurSel = SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );

                    strcpy( (char*)SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 ), "" );

                    //update the display
                    PostMessage( hPage, WM_COMMAND, MAKEWPARAM( IDC_SOUNDSAVAIL, 0 ), 0 );
                    break;
                }

                case IDC_BROWSE:
                {
                    OPENFILENAME ofn;
                    char szPrompt[256] = "Browse for ", szSoundName[256];
                    LPCTSTR lpstrFileMasks ="Sounds (*.wav)\0*.wav\0All Files (*.*)\0*.*\0\0";
                    int iCurSel = SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );

                    //build browse dialog prompt
                    SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETTEXT, iCurSel, (LPARAM)szSoundName );
                    strcat( szPrompt, szSoundName );
                    strcat( szPrompt, " sound" );

                    strcpy( szSoundName, (char*)SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 ) );

                    //get the current file
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hPage;
                    ofn.hInstance = NULL;
                    ofn.lpstrFilter = lpstrFileMasks;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = szSoundName;
                    ofn.nMaxFile = MAX_PATH-1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = szPrompt;
                    ofn.Flags = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                    ofn.lpstrDefExt = "WAV";
                    if( GetOpenFileName( &ofn ) )
                    {
                        //update everything with the new sound
                        SetDlgItemText( hPage, IDC_SOUNDNAME, ofn.lpstrFile );
                        strcpy( (char*)SendDlgItemMessage( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 ), szSoundName );

                        //update the display
                        PostMessage( hPage, WM_COMMAND, MAKEWPARAM( IDC_SOUNDSAVAIL, 0 ), 0 );

						//make sure it will make a sound
						if( SendDlgItemMessage( hPage, IDC_SOUNDFREQ, TBM_GETPOS, 0, 0 ) == 0 )
							SendDlgItemMessage( hPage, IDC_SOUNDFREQ, TBM_SETPOS, TRUE, 2 );

                    }
                    break;
                }
            }
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE ); 
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (DWORD)(LPSTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelp( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (DWORD)(LPVOID)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}


/* PropPage_Independence - message processsing function for the 'independence' property page */
BOOL CALLBACK PropPage_Independence( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessage( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)"Chase the mouse (default)" );
            SendDlgItemMessage( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)"Run away from the mouse");
            SendDlgItemMessage( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)"Run around randomly");
            SendDlgItemMessage( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)"Pace around the screen");
            SendDlgItemMessage( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)"Run around the screen");
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uAction = SendDlgItemMessage( hPage, IDC_ACTION, CB_GETCURSEL, 0, 0 );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessage( hPage, IDC_ACTION, CB_SETCURSEL, g_lpCurrentCat->uAction, 0 );
            SendMessage( hPage, WM_COMMAND, MAKEWPARAM(IDC_ACTION, CBN_SELCHANGE), 0 );
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE ); 
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_ACTION:
                    if( HIWORD(wParam) == CBN_SELCHANGE )
                    {
                        const char* szActionDesc[] = {
                            "Neko will follow the mouse cursor wherever it goes",
                            "Neko will run away when the mouse cursor gets too near, and sleep when it isn't near",
                            "Neko will run to a random spot on the desktop, sleep for a while, and then continue running around",
                            "Neko will run around the outside of the desktop",
                            "Neko will run around the desktop, without stopping"
                        };

                        SetDlgItemText( hPage, IDC_ACTIONDESC, szActionDesc[SendDlgItemMessage( hPage, IDC_ACTION, CB_GETCURSEL, 0, 0 )] );
                    }
                    break;
            }
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (DWORD)(LPSTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelp( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (DWORD)(LPVOID)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}




/* PropPage_Effects - message processsing function for the 'effects' property page */
BOOL CALLBACK PropPage_Effects( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case MY_WRITESETTINGS:
            GetDlgItemText( hPage, IDC_IMAGELIB, g_lpCurrentCat->szFootprintLib, MAX_PATH );
			g_lpCurrentCat->bFootprints = IsDlgButtonChecked( hPage, IDC_FOOTPRINTS );
            break;

        case MY_READSETTINGS:
            SetDlgItemText( hPage, IDC_IMAGELIB, g_lpCurrentCat->szFootprintLib );
			CheckDlgButton( hPage, IDC_FOOTPRINTS, g_lpCurrentCat->bFootprints );
            break;

        case WM_COMMAND:
        
            switch( LOWORD(wParam) )
            {
                case IDC_DEFAULT:
                    SetDlgItemText( hPage, IDC_IMAGELIB, "" );
                    break;

                case IDC_CHANGE:
                {
                    OPENFILENAME ofn;
                    char lpstrLibrary[MAX_PATH];
                    LPCTSTR lpstrFileMasks = "Icon Libraries\0*.icl;*.exe;*.dll\0All Files (*.*)\0*.*\0\0";

                    //get the current file
                    GetDlgItemText( hPage, IDC_IMAGELIB, lpstrLibrary, MAX_PATH-1 );
                    
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hPage;
                    ofn.hInstance = NULL;
                    ofn.lpstrFilter = lpstrFileMasks;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = lpstrLibrary;
                    ofn.nMaxFile = MAX_PATH-1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = "Select Footprint Image Library";
                    ofn.Flags = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                    ofn.lpstrDefExt = "icl";
                    if( GetOpenFileName( &ofn ) )
                    {
                        //check to make sure it's got enough icons in...
                        if( (UINT)ExtractIcon( g_hInstance, ofn.lpstrFile, (UINT)-1 ) < 8 )
                            MessageBox( hPage, "That file does not have enough icons in it - it must have at least 8", "Change Image Library", MB_ICONEXCLAMATION );
                        else
                        {
                            SetDlgItemText( hPage, IDC_IMAGELIB, ofn.lpstrFile );
			                CheckDlgButton( hPage, IDC_FOOTPRINTS, TRUE );
                        }
                    }
                    break;
                }
            }
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE ); 
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (DWORD)(LPSTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelp( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (DWORD)(LPVOID)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}
