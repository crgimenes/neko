// Neko.h: interface for the CNeko class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEKO_H__A69EBAA5_385D_11D2_9FF9_00001C192944__INCLUDED_)
#define AFX_NEKO_H__A69EBAA5_385D_11D2_9FF9_00001C192944__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DesktopPet.h"
#include "AlwaysOnTopPet.h"
#include "NekoCommon.h"

class CNeko {

public:
	CNeko( char* pszName = "" );
	virtual ~CNeko();

	//public interface
	virtual void Update();
	virtual void RunTowards( int nX, int nY );

protected:

	//states. (note: I know that the numbering is unnecessary, but you never know... - these MUST go from 0-17)
    enum State { STOP = 0, WASH = 1, SCRATCH = 2, YAWN = 3, SLEEP = 4, AWAKE = 5, U_MOVE = 6, 
				  D_MOVE = 7, L_MOVE = 8, R_MOVE = 9, UL_MOVE = 10, UR_MOVE = 11, DL_MOVE = 12,
				  DR_MOVE = 13, U_CLAW = 14, D_CLAW = 15, L_CLAW = 16, R_CLAW = 17 };

    //position info
	int m_nDX, m_nDY;
    int m_nOldToX, m_nOldToY;
    int m_nToX, m_nToY;

    //size info
	DWORD m_dwScale;
    
    //state info
    UINT m_uTickCount, m_uStateCount;
    State m_State;

    //action info
    int m_Action;
    int m_nActionCount;
    int m_nActionX, m_nActionY, m_nActionDX, m_nActionDY;

    //attributes
    DWORD m_dwSpeed;
    DWORD m_dwIdleSpace;
    char m_szLibname[MAX_PATH];
    char m_szFootprintLibname[MAX_PATH];
    BOOL m_bFootprints;
	
	//sound stuff
	char m_szSndIdle1[MAX_PATH], m_szSndIdle2[MAX_PATH], m_szSndIdle3[MAX_PATH];
	char m_szSndSleep[MAX_PATH], m_szSndAwake[MAX_PATH];
	DWORD m_dwSndFrequency;

	//named Neko stuff
	char m_szName[MAX_NEKO_NAME];

	//animation lookup table
    int m_nAnimation[18][2];

	State GetState() { return m_State; }

    HICON m_hIconFootprints[8];

private:
	CPet* m_pPet;
	BOOL LoadImages();
	int GetStateAnimationFrameIndex();
	void CalcDirection();
	BOOL MoveStart();
	void SetState( State state );
};

#endif // !defined(AFX_NEKO_H__A69EBAA5_385D_11D2_9FF9_00001C192944__INCLUDED_)
