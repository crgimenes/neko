/****************
  NekoCFG

  Configuration
  program for
  Neko95 v4.0
  
*****************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "NekoCFG.h"
#include "..\NekoCommon.h"
#include "..\NekoSettings.h"
#include "NekoCFG_resource.h"
#include "NekoCFG_resource.hm"

/*************************************************************************************************/

const char* szWindowTitle = "Configure Neko";
const char* szDefaultName = "Neko (default)";
const char* szHelpFile = "Neko98.hlp";

/* help ID lookup table */
static DWORD dwHelpID[] = { 
    IDC_NAME,           HIDC_NAME, 
    IDC_NEW,            HIDC_NEW, 
    IDC_DELETE,         HIDC_DELETE,
    IDC_TASKBAR,        HIDC_TASKBAR, 
    IDC_ABOUT,          HIDC_ABOUT,
    IDOK,               HIDOK,
    IDCANCEL,           HIDCANCEL,
    IDC_APPLY,          HIDC_APPLY,
    IDC_HELP,           HIDC_HELP,
    IDC_TABS,           HIDC_TABS,
    0,0
};

//global variables
HINSTANCE g_hInstance = NULL;
HWND g_hWndNeko = NULL;


//global settings
BOOL g_fShowTaskbar = TRUE;

//list of all cats
LPCATSETTINGS catSettings = NULL;

//function forward declaration
void WINAPI WriteSettings();

/*************************************************************************************************/

/* DeleteCatSettings - removes the given cat from the list */
BOOL WINAPI DeleteCatSettings( LPCATSETTINGS cat )
{
    cat->fDeleted = TRUE;
    return TRUE;
}

/* DlgProc_NewNeko - dialog box procedure to add a neko */
BOOL CALLBACK DlgProc_NewNeko( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            //make sure the user isn't adding *too* many Nekos!
            LPCATSETTINGS cat = catSettings;
            int nCats = 0;
            while( cat )
            {
                if( cat->fDeleted == FALSE ) nCats++;
                cat = cat->next;
            }

            if( nCats > 5 )
            {
                char szBuffer[128];
                sprintf( szBuffer, "You already have %d Nekos!\nAdding more is likely to slow down your computer - do you want to add one anyway?", nCats );
                if( IDNO == MessageBox( hDlg, szBuffer, "Add Neko", MB_YESNO|MB_ICONQUESTION ) )
                {
                    EndDialog( hDlg, FALSE );
                    return TRUE;
                }
            }

            //initialise the text box
            SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_LIMITTEXT, IDC_NEWNEKONAME-2, 0 );

			//add some demo items
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Arthur" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Boris" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Kitty" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Lucy" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Garfield" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Tom" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Bast" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Ginger" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Bob" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Tabs" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Tigger" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"James" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Cooper" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Hey You" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Greebo" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Furball" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Sylvester" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Buffy" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Willow" );
			SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Sarah" );
			//SendDlgItemMessage( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)"Vicious Bastard" );
            break;
        }

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                {
                    char szName[MAX_NEKO_NAME];
                    GetDlgItemText( hDlg, IDC_NEWNEKONAME, szName, MAX_NEKO_NAME );
                    char* lpszPtr = szName;

                    //make sure it's not empty
                    if( strlen( szName ) == 0 )
                    {
                        MessageBox( hDlg, "Invalid Name: You must type a name", "New Neko", MB_ICONINFORMATION );
                        return TRUE;
                    }

                    //make sure it only has letters, numbers and spaces in
                    while( lpszPtr && *lpszPtr != '\0' )
                    {
                        if( !( isalpha(*lpszPtr) || isdigit(*lpszPtr) || *lpszPtr == ' ' ) )
                        {
                            MessageBox( hDlg, "Invalid Name: Sorry - a name can only contain letters, numbers and spaces", "Add Neko", MB_ICONINFORMATION );
                            return TRUE;
                        }
                        lpszPtr++;
                    }

                    //make sure it's unique
                    LPCATSETTINGS cat = catSettings;
                    while( cat )
                    {
                        if( !cat->fDeleted && (stricmp( cat->szName, szName ) == 0 || stricmp( cat->szName, szDefaultName ) == 0 ))
                        {
                            MessageBox( hDlg, "Invalid Name: This neko already exists!", "New Neko", MB_ICONINFORMATION );
                            return TRUE;
                        }
                        cat = cat->next;
                    }


                    //create a new neko
                    cat = new CATSETTINGS;
                    strcpy( cat->szName, szName );

                    //set defaults
                    strcpy( cat->szLibrary, "" );
                    cat->uScale = 100;
                    cat->uSpeed = 16;
                    cat->uMouseSensitivity = 6;
                    strcpy( cat->szSndIdle1, "" );
                    strcpy( cat->szSndIdle2, "" );
                    strcpy( cat->szSndIdle3, "" );
	                strcpy( cat->szSndSleep, "" );
                    strcpy( cat->szSndAwake, "" );
	                cat->uSndFrequency = 0;
                    cat->uAction = CHASE_MOUSE;
					cat->fAlwaysOnTop = FALSE;
                    cat->bFootprints = FALSE;
                    *cat->szFootprintLib = '\0';

                    //link it in
                    cat->fDeleted = FALSE;
                    cat->next = catSettings;
                    catSettings = cat;

                    //add it to the list box & select it
                    int i = SendDlgItemMessage( GetParent(hDlg), IDC_NAME, LB_ADDSTRING, 0, (LPARAM)szName );
                    SendDlgItemMessage( GetParent(hDlg), IDC_NAME, LB_SETCURSEL, i, 0 );
                    PostMessage( GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDC_NAME, CBN_SELCHANGE), 0 );

                    //close the dialog
                    EndDialog( hDlg, TRUE );
                    break;
                }

                case IDCANCEL:
                    EndDialog( hDlg, FALSE );
                    break;
            }
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

/* DlgProc_About - dialog box procedure for about dialog */
BOOL CALLBACK DlgProc_About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( ( uMsg == WM_CLOSE ) || ( uMsg == WM_COMMAND && LOWORD(wParam)==IDOK ) ) EndDialog( hDlg, TRUE ); else return FALSE;
    return TRUE;
}

/* DlgProc_Config - dialog box procedure for configuration dialog */
BOOL CALLBACK DlgProc_Config( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            SendDlgItemMessage( hDlg, IDC_TASKBAR, BM_SETCHECK, g_fShowTaskbar, 0 );

            LPCATSETTINGS cat = catSettings;
            while( cat )
            {
                if( !cat->fDeleted ) SendDlgItemMessage( hDlg, IDC_NAME, LB_ADDSTRING, 0, ( strcmp( cat->szName, "" ) == 0 ) ? (LPARAM)szDefaultName : (LPARAM)cat->szName );
                cat = cat->next;
            }
            SendDlgItemMessage( hDlg, IDC_NAME, LB_SELECTSTRING, 0, (LPARAM)szDefaultName );
            EnableWindow( GetDlgItem( hDlg, IDC_DELETE ), FALSE );
            EnableWindow( GetDlgItem( hDlg, IDC_APPLY ), (g_hWndNeko != NULL ) ? TRUE : FALSE );

            InitialisePropertyDialog( GetDlgItem( hDlg, IDC_TABS ) );
            SendMessage( hDlg, WM_COMMAND, MAKEWPARAM(IDC_NAME, LBN_SELCHANGE), 0 );
            return TRUE;
        }

        case WM_DESTROY:
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                    SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                    g_fShowTaskbar = ( IsDlgButtonChecked( hDlg, IDC_TASKBAR ) == BST_CHECKED );
                    WriteSettings();
                    if( IsWindow( g_hWndNeko ) == FALSE ) g_hWndNeko = FindWindow( szNekoClassName, szNekoWindowTitle );
                    SendMessage( g_hWndNeko, MY_UPDATENEKO, 0, 0 );
                    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
                    EndDialog( hDlg, TRUE ); 
                    break;

                case IDCANCEL:
                    EndDialog( hDlg, FALSE ); break;

                case IDC_APPLY:
                    SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                    g_fShowTaskbar = ( IsDlgButtonChecked( hDlg, IDC_TASKBAR ) == BST_CHECKED );
                    WriteSettings();
                    if( IsWindow( g_hWndNeko ) == FALSE ) g_hWndNeko = FindWindow( szNekoClassName, szNekoWindowTitle );
                    SendMessage( g_hWndNeko, MY_UPDATENEKO, 0, 0 );
                    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
                    break;

                case IDC_ABOUT:
                    DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, (DLGPROC)DlgProc_About );
                    break;

                case IDC_NEW:
                    DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_NEWNEKO), hDlg, (DLGPROC)DlgProc_NewNeko );
                    break;

                case IDC_DELETE:
                {
                    int iItem = SendDlgItemMessage( hDlg, IDC_NAME, LB_GETCURSEL, 0, 0 );
                    char szDoomed[MAX_NEKO_NAME] = "";

                    //get string
                    SendDlgItemMessage( hDlg, IDC_NAME, LB_GETTEXT, iItem, (LPARAM)szDoomed );
                    if( strcmp( szDoomed, szDefaultName ) == 0 ) strcpy( szDoomed, "" );

                    //find it in the settings list and continue
                    LPCATSETTINGS cat = catSettings;
                    while( cat )
                    {
                        if( !cat->fDeleted )
                            if( strcmp( cat->szName, szDoomed ) == 0 ) break;

                        cat = cat->next;
                    }

                    if( cat )
                    {
                        //remove it from the list box
                        SendDlgItemMessage( hDlg, IDC_NAME, LB_DELETESTRING, iItem, 0 );

                        //select another item
                        SendDlgItemMessage( hDlg, IDC_NAME, LB_SETCURSEL, (iItem ? iItem-1 : 0), 0 );
                        SendMessage( hDlg, WM_COMMAND, MAKEWPARAM(IDC_NAME,LBN_SELCHANGE), 0 );

                        //remove it from memory
                        if( DeleteCatSettings( cat ) == FALSE ) 
                            MessageBox( hDlg, "Internal Error: Could not delete cat!", szWindowTitle, MB_ICONERROR );
                    }
                    else
                        MessageBox( hDlg, "Internal Error: Dropped off the end of the cat list!", szWindowTitle, MB_ICONERROR );

                    break;
                }

                case IDC_HELP:
                    WinHelp( hDlg, szHelpFile, HELP_FINDER, 0 );
                    break;

                case IDC_NAME:
                    if( HIWORD(wParam) == LBN_SELCHANGE )
                    {
                        int iNew = SendDlgItemMessage( hDlg, IDC_NAME, LB_GETCURSEL, 0, 0 );
                        char szNew[MAX_NEKO_NAME] = "";

                        //get string and enable delete if it's not the default item (first)
                        SendDlgItemMessage( hDlg, IDC_NAME, LB_GETTEXT, iNew, (LPARAM)szNew );
                        if( strcmp( szNew, szDefaultName ) == 0 )
                        {
                            strcpy( szNew, "" );
                            EnableWindow( GetDlgItem( hDlg, IDC_DELETE ), FALSE );
                        }
                        else EnableWindow( GetDlgItem( hDlg, IDC_DELETE ), TRUE );

                        //the user has selected a different name - find it in the settings list and continue
                        LPCATSETTINGS cat = catSettings;
                        while( cat )
                        {
                            if( !cat->fDeleted )
                                if( strcmp( cat->szName, szNew ) == 0 ) break;

                            cat = cat->next;
                        }

                        if( cat )
                            SetActiveCat( cat );
                        else
                            MessageBox( hDlg, "Internal Error: Dropped off the end of the cat list!", szWindowTitle, MB_ICONERROR );
                    }
                    break;
            }
            break;

        case WM_NOTIFY:
        {
            LPNMHDR nmhdr = (LPNMHDR)lParam;
            switch( nmhdr->idFrom )
            {
                case IDC_TABS:
                    if( nmhdr->code == TCN_SELCHANGE ) OnSelChanged( GetDlgItem( hDlg, IDC_TABS ) );
                    break;
            }
            break;
        }

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

/*************************************************************************************************/

/* WriteCatSetting - write a cat from the registry with the given cat settings block */
void WINAPI WriteCatSettings( LPCATSETTINGS cat, LPCSTR szName )
{
    //see if this cat is flagged as deleted
    if( cat->fDeleted == TRUE )
    {
        //remove this item from the registry
        if( strlen( szName ) > 0 )
        {
            HKEY hKey;
            char szKeyName[MAX_PATH];
            strcpy( szKeyName, szNekoRegKey );
            if( RegOpenKeyEx( HKEY_CURRENT_USER, szKeyName, 0, KEY_WRITE, &hKey ) == ERROR_SUCCESS )
            {
                LONG l = RegDeleteKey( hKey, cat->szName );
                if( l != ERROR_SUCCESS ) 
                {
/*
                    LPVOID lpMsgBuf;

                    FormatMessage( 
                        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        l,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL 
                    );
                    // Display the string.
                    MessageBox( NULL, (char*)lpMsgBuf, "Can't Delete Key", MB_OK|MB_ICONINFORMATION );

                    // Free the buffer.
                    LocalFree( lpMsgBuf );
*/
 
                }
                RegCloseKey( hKey );
            }
            else
                MessageBox( NULL, "Internal Error: Couldn't open registry key!", 0, MB_TASKMODAL );
        }
        else
            MessageBox( NULL, "Internal Error: Tried to delete default Neko!", 0, MB_TASKMODAL );
    }
    else
    {
        //build & open key
	    char szKey[1024];
	    strcpy( szKey, szNekoRegKey );
	    if( strlen( szName ) > 0 )
	    {
		    strcat( szKey, "\\" );
		    strcat( szKey, szName );
	    }

        //write cat to the registry
        CNekoSettings NekoSettings( szKey );

        //display
        NekoSettings.SetString( szNekoLibraryKey, cat->szLibrary );
        NekoSettings.SetInt( szNekoScaleKey, cat->uScale );
		NekoSettings.SetInt( szNekoOnTopKey, cat->fAlwaysOnTop );

        //movement
        NekoSettings.SetInt( szNekoSpeedKey, cat->uSpeed );
        NekoSettings.SetInt( szNekoSenseKey, cat->uMouseSensitivity );

        //sounds
        NekoSettings.SetString( szNekoSndIdle1Key, cat->szSndIdle1 );
        NekoSettings.SetString( szNekoSndIdle2Key, cat->szSndIdle2 );
        NekoSettings.SetString( szNekoSndIdle3Key, cat->szSndIdle3 );
	    NekoSettings.SetString( szNekoSndSleepKey, cat->szSndSleep );
        NekoSettings.SetString( szNekoSndAwakeKey, cat->szSndAwake );
	    NekoSettings.SetInt( szNekoSndFreqKey, cat->uSndFrequency );

        //independence
        NekoSettings.SetInt( szNekoActionKey, cat->uAction );

        //effects
        NekoSettings.SetBool( szNekoFootprintKey, cat->bFootprints );
        NekoSettings.SetString( szNekoFootprintLibKey, cat->szFootprintLib );
    }
}


/* WriteSettings - writes all of the settings to the registry */
void WINAPI WriteSettings()
{
    //tell all dialogs to write what they've got so far into the data structure
    GetDialogSettings();

    //write global settings
    CNekoSettings NekoSettings( szNekoRegKey );
    NekoSettings.SetBool( szNekoTaskbarKey, g_fShowTaskbar );
    
    int nNumCats = 0;
    LPCATSETTINGS cat = catSettings;
    while( cat )
    {
        //write this cat's settings
        WriteCatSettings( cat, cat->szName );

        //unlink it if it's been deleted, or write out the name index key if not
        if( cat->fDeleted == FALSE )
        {
            //build the key index
            char szSubKey[MAX_PATH];
            sprintf( szSubKey, "%d", nNumCats );

            //write this cat's name to the registry
		    NekoSettings.SetString( szSubKey, cat->szName );

            //advance the list
            nNumCats++;
            cat = cat->next;
        }
        else
        {
            cat = cat->next;
        }
    }

    NekoSettings.SetInt( szNekoNumCatsKey, nNumCats );
}

/* ReadCatSetting - loads a cat from the registry into the given cat settings block */
void WINAPI ReadCatSettings( LPCATSETTINGS cat, LPCSTR szName )
{
    strcpy( cat->szName, szName );

    //set defaults
    strcpy( cat->szLibrary, "" );
    cat->uScale = 100;
    cat->uSpeed = 16;
    cat->uMouseSensitivity = 6;
    strcpy( cat->szSndIdle1, "" );
    strcpy( cat->szSndIdle2, "" );
    strcpy( cat->szSndIdle3, "" );
	strcpy( cat->szSndSleep, "" );
    strcpy( cat->szSndAwake, "" );
	cat->uSndFrequency = 0;
    cat->uAction = CHASE_MOUSE;
    cat->fDeleted = FALSE;
    cat->fAlwaysOnTop = FALSE;
    cat->bFootprints = FALSE;
    *cat->szFootprintLib = '\0';

    //build & open key
	char szKey[1024];
	strcpy( szKey, szNekoRegKey );
	if( strlen( szName ) > 0 )
	{
		strcat( szKey, "\\" );
		strcat( szKey, szName );
	}

    CNekoSettings NekoSettings( szKey );

    //display
    NekoSettings.GetString( szNekoLibraryKey, cat->szLibrary, MAX_PATH-1 );
    NekoSettings.GetInt( szNekoScaleKey, &cat->uScale );
    DWORD fAlwaysOnTop = cat->fAlwaysOnTop;
	NekoSettings.GetInt( szNekoOnTopKey, &fAlwaysOnTop );
    cat->fAlwaysOnTop = fAlwaysOnTop;

    //movement
    NekoSettings.GetInt( szNekoSpeedKey, &cat->uSpeed );
    NekoSettings.GetInt( szNekoSenseKey, &cat->uMouseSensitivity );

    //sounds
    NekoSettings.GetString( szNekoSndIdle1Key, cat->szSndIdle1, MAX_PATH-1 );
    NekoSettings.GetString( szNekoSndIdle2Key, cat->szSndIdle2, MAX_PATH-1 );
    NekoSettings.GetString( szNekoSndIdle3Key, cat->szSndIdle3, MAX_PATH-1 );
	NekoSettings.GetString( szNekoSndSleepKey, cat->szSndSleep, MAX_PATH-1 );
    NekoSettings.GetString( szNekoSndAwakeKey, cat->szSndAwake, MAX_PATH-1 );
	NekoSettings.GetInt( szNekoSndFreqKey, &cat->uSndFrequency );

    //independence
    NekoSettings.GetInt( szNekoActionKey, &cat->uAction );

    NekoSettings.GetBool( szNekoFootprintKey, &cat->bFootprints );
    NekoSettings.GetString( szNekoFootprintLibKey, cat->szFootprintLib, MAX_PATH-1 );
}

/* ReadSettings - reads all of the settings from the registry and creates the global settings array */
void WINAPI ReadSettings()
{
    LPCATSETTINGS cat;

    CNekoSettings * NekoSettings = new CNekoSettings( szNekoRegKey );
    NekoSettings->GetBool( szNekoTaskbarKey, &g_fShowTaskbar );

    //load in new cats list
    DWORD nNumCats = 0;
    NekoSettings->GetInt( szNekoNumCatsKey, &nNumCats );

    if( nNumCats == 0 )
    {
        //the user hasn't run the config program, or there are no Nekos - use default
        cat = new CATSETTINGS;
        cat->next = NULL;
        ReadCatSettings( cat, "" );

        catSettings = cat;
    }
    else
    {
        for( DWORD i = 0; i < nNumCats; i++ )
        {
            //build the key index
            char szSubKey[MAX_PATH], szName[MAX_NEKO_NAME];
            sprintf( szSubKey, "%d", i );

            //load this cat's name from the registry
		    NekoSettings->GetString( szSubKey, szName, MAX_NEKO_NAME-1 );

            //create a new setting for it and put the object into the list
            cat = new CATSETTINGS;
            cat->next = catSettings;
            ReadCatSettings( cat, szName );
            catSettings = cat;
        }
    }

    delete NekoSettings;
}

/* DeleteConfigList - deletes the list of settings from memory */
void WINAPI DeleteConfigList()
{
    LPCATSETTINGS cat = catSettings;
    while( catSettings )
    {
        cat = catSettings;
        catSettings = catSettings->next;
        delete cat;
    }
}


/* FindAndActivateOldInstance - activates the old instance of the configuration program
   if it has been loaded twice */
BOOL FindAndActivateOldInstance()
{
    HWND hWnd = FindWindow( NULL, szWindowTitle );
    if( hWnd )
    {
        OpenIcon(hWnd);
        SetForegroundWindow(hWnd);
        return TRUE;
    }

    return FALSE;
}

/* WinMain - main program start point */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    //see if the configuration program is loaded already
    if( FindAndActivateOldInstance() ) return 0;

    //ensure the common control library is loaded
    InitCommonControls();

    //store the instance handle and find the Neko window
    g_hInstance = hInstance;
    g_hWndNeko = FindWindow( szNekoClassName, szNekoWindowTitle );

    //initialise program and display dialog
    ReadSettings();
    DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_CONFIG), NULL, (DLGPROC)DlgProc_Config );
    DeleteConfigList();
    ShutdownPropertyDialog();
    return 0;
}
