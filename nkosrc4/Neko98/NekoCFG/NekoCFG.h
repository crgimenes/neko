#ifndef _NEKOCFG_H
#define _NEKOCFG_H

#include "..\NekoCommon.h"

//structures
typedef struct tagTABINITDATA {
    char* szTitle;
    UINT uIDDialog;
    DLGPROC lpfnDlgProc;
    UINT uIDIcon;
    HWND hDlg;
} TABINITDATA, *LPTABINITDATA;

typedef struct tagCATSETTINGS {
    char szName[MAX_NEKO_NAME];
    
    //display
    char szLibrary[MAX_PATH];
    DWORD uScale;
	BOOL fAlwaysOnTop;

    //movement
    DWORD uSpeed;
    DWORD uMouseSensitivity;

    //sounds
    char szSndIdle1[MAX_PATH];
    char szSndIdle2[MAX_PATH];
    char szSndIdle3[MAX_PATH];
	char szSndSleep[MAX_PATH];
    char szSndAwake[MAX_PATH];
	DWORD uSndFrequency;

    //independence
    DWORD uAction;

    //effects
    BOOL bFootprints;
    char szFootprintLib[MAX_PATH];

    //list items
    BOOL fDeleted;
    struct tagCATSETTINGS* next;

} CATSETTINGS, *LPCATSETTINGS;

//ranges for slider bars
#define MIN_SPEED 2
#define MAX_SPEED 48
#define MIN_SENSE 1
#define MAX_SENSE 64
#define MIN_SNDFREQ 0
#define MAX_SNDFREQ 100
#define MIN_SCALE 10
#define MAX_SCALE 400

//external functions
extern void WINAPI InitialisePropertyDialog( HWND hWnd );
extern void WINAPI ShutdownPropertyDialog();
extern void WINAPI OnSelChanged( HWND hWnd );
extern void WINAPI SetActiveCat( LPCATSETTINGS lpCat );
extern void WINAPI GetDialogSettings();

//external variables
extern HINSTANCE g_hInstance;
extern const char* szHelpFile;

#endif //_NEKOCFG_H
