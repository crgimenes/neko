// Neko.cpp: implementation of the CNeko class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "NekoCommon.h"
#include "NekoSettings.h"
#include "Neko.h"
#include "resource.h"
#include <math.h>

//maths calculations
#define g_dSinPiPer8        0.3826834323651  // sin [pi/8]
#define g_dSinPiPer8Times3  0.9238795325113  // sin ( [pi/8] x 3 )

//misc. constants
#define MAX_TICK        9999 //Odd Only

//animation control constants
#define STOP_TIME      4
#define WASH_TIME      10
#define SCRATCH_TIME   4
#define YAWN_TIME      3
#define AWAKE_TIME     3
#define CLAW_TIME      10

//external system variable
extern HINSTANCE g_hInstance;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeko::CNeko( char* lpszName )
{
	//store pet
	m_pPet = NULL;

    //plug icons into animation table
	m_nAnimation[STOP][0]	 = 28;      m_nAnimation[STOP][1] =	    28;
	m_nAnimation[WASH][0]	 = 25;      m_nAnimation[WASH][1] =	    28;
	m_nAnimation[SCRATCH][0] = 26;      m_nAnimation[SCRATCH][1] =  27;
	m_nAnimation[YAWN][0]	 = 29;      m_nAnimation[YAWN][1] =	    29;
	m_nAnimation[SLEEP][0]	 = 30;      m_nAnimation[SLEEP][1] =	31;
	m_nAnimation[AWAKE][0]	 = 0;	    m_nAnimation[AWAKE][1] =	0;
	m_nAnimation[U_MOVE][0]  = 1;	    m_nAnimation[U_MOVE][1] =	2;
	m_nAnimation[D_MOVE][0]  = 9;	    m_nAnimation[D_MOVE][1] =	10;
	m_nAnimation[L_MOVE][0]  = 13;      m_nAnimation[L_MOVE][1] =	14;
	m_nAnimation[R_MOVE][0]  = 5;	    m_nAnimation[R_MOVE][1] =	6;
	m_nAnimation[UL_MOVE][0] = 15;      m_nAnimation[UL_MOVE][1] =  16;
	m_nAnimation[UR_MOVE][0] = 3;	    m_nAnimation[UR_MOVE][1] =  4;
	m_nAnimation[DL_MOVE][0] = 11;      m_nAnimation[DL_MOVE][1] =  12;
	m_nAnimation[DR_MOVE][0] = 7;	    m_nAnimation[DR_MOVE][1] =  8;
	m_nAnimation[U_CLAW][0]  = 17;      m_nAnimation[U_CLAW][1] =	18;
	m_nAnimation[D_CLAW][0]  = 23;      m_nAnimation[D_CLAW][1] =	24;
	m_nAnimation[L_CLAW][0]  = 21;      m_nAnimation[L_CLAW][1] =	22;
	m_nAnimation[R_CLAW][0]  = 19;      m_nAnimation[R_CLAW][1] =	20;

    //set variables
	m_nDX = m_nDY = 0;
	strcpy( m_szName, lpszName );
    m_dwSpeed = 16;
    m_dwIdleSpace = 6;
    m_Action = CHASE_MOUSE;
    m_nActionCount = 0;
    *m_szFootprintLibname = '\0';
    *m_szLibname = '\0';
    m_bFootprints = FALSE;

	strcpy( m_szSndIdle1, "" );
	strcpy( m_szSndIdle2, "" );
	strcpy( m_szSndIdle3, "" );
	strcpy( m_szSndSleep, "" );
	strcpy( m_szSndAwake, "" );
	m_dwSndFrequency = 0;
    m_dwScale = 100;

	//build configuration registry key
	char szKey[1024];
	strcpy( szKey, szNekoRegKey );
	if( strlen( m_szName ) > 0 )
	{
		strcat( szKey, "\\" );
		strcat( szKey, m_szName );
	}

	//load configuration
    CNekoSettings NekoSettings( szKey, (strlen(m_szName) == 0) );
	if( NekoSettings.IsOpen() )
	{
		//load in all of the settings
		NekoSettings.GetInt( szNekoScaleKey,		&m_dwScale );
		NekoSettings.GetInt( szNekoSpeedKey,		&m_dwSpeed );
		NekoSettings.GetInt( szNekoSenseKey,		&m_dwIdleSpace );
		NekoSettings.GetString( szNekoLibraryKey,	m_szLibname, MAX_PATH-1 );
		NekoSettings.GetString( szNekoSndIdle1Key,	m_szSndIdle1, MAX_PATH-1 );
		NekoSettings.GetString( szNekoSndIdle2Key,	m_szSndIdle2, MAX_PATH-1 );
		NekoSettings.GetString( szNekoSndIdle3Key,	m_szSndIdle3, MAX_PATH-1 );
		NekoSettings.GetString( szNekoSndSleepKey,	m_szSndSleep, MAX_PATH-1 );
		NekoSettings.GetString( szNekoSndAwakeKey,	m_szSndAwake, MAX_PATH-1 );
		NekoSettings.GetInt( szNekoSndFreqKey,		&m_dwSndFrequency );
        NekoSettings.GetBool( szNekoFootprintKey,   &m_bFootprints );
        NekoSettings.GetString( szNekoFootprintLibKey, m_szFootprintLibname, MAX_PATH-1 );

        DWORD dwAction = m_Action;
        NekoSettings.GetInt( szNekoActionKey, &dwAction );
        m_Action = dwAction;


        DWORD bAlwaysOnTop = FALSE;
		NekoSettings.GetInt( szNekoOnTopKey, &bAlwaysOnTop );

        //create the correct pet
        if( bAlwaysOnTop )
            m_pPet = new CAlwaysOnTopPet();
        else
            m_pPet = new CDesktopPet();
       
	}
    else
    {
        //configuration didn't open... create a desktop pet only
        m_pPet = new CDesktopPet();
    }

    //initialse footprint icons
    for( int i = 0; i < 8; i++ ) m_hIconFootprints[i] = NULL;

    //apply scaling
    m_pPet->SetScale( ((float)m_dwScale / 100.0f ) );

	//load the images
	BOOL fLoadProblems = FALSE;
	if( m_szLibname == NULL || *m_szLibname == '\0' || ((int)ExtractIcon( g_hInstance, m_szLibname, -1 ) < 32 ))
    {
        //use default images if there is no file or not enough icons
		GetModuleFileName( NULL, m_szLibname, MAX_PATH );
		fLoadProblems = !LoadImages();
    }
	else
	{
		//load all the icons in the file
		fLoadProblems = !LoadImages();
		if( fLoadProblems ) 
		{
			//use default images if it fails with the user's choice
			GetModuleFileName( NULL, m_szLibname, MAX_PATH );
			fLoadProblems = !LoadImages();
		}
	}

	/* FIXME:

		It appears that Windows offers no support for ExtractIcon
		on icons that are not 32x32 - this means that all icon
		libraries selected by the user to use will have their icons
		scaled down to 32x32. I have tried the following:

        1) Used LoadLibrary() to load the icon library chosen. This
		   worked on some, but not all. I then used LoadLibraryEx() 
		   and passed it the 'don't call DllMain' flags. This caused
		   the libraries that weren't working to work, and vice-versa.

		2) After LoadLibrary(), attempting to load all resource IDs
		   until 32 valid icons were loaded. This nearly worked, but
		   took ages and was therefor unacceptable. It also failed
		   with LoadLibrary() as in 1.

		3) Tried EnumResourceNames() for all icons. It only loaded some
		   of them and then gave up. At this point, so did I.

		Result: It is only possible to use 32x32 icons in Neko, although
		these can be scaled up or down as required, resulting in blockyness
		etc.

	*/

    //set initial state
	SetState( STOP );

    //set initial action
    m_nActionX = m_pPet->GetBoundsRect().left + ( rand() % (m_pPet->GetBoundsRect().right-(m_dwSpeed * 8)) );
    m_nActionY = m_pPet->GetBoundsRect().top + ( rand() % (m_pPet->GetBoundsRect().bottom-(m_dwSpeed * 8)) );
    m_nActionDX = ((( rand() % 2 ) ? 1 : -1) * (m_dwSpeed/2)) + 1;
	m_nActionDY = ((( rand() % 2 ) ? 1 : -1) * (m_dwSpeed/2)) + 1;

    //set initial position (random)
    m_nToX = m_pPet->GetBoundsRect().left + ( rand() % ( (m_pPet->GetBoundsRect().right- m_pPet->GetSize().cx) - m_pPet->GetBoundsRect().left ) ) ;
    m_nToY = m_pPet->GetBoundsRect().top + ( rand() % ( (m_pPet->GetBoundsRect().bottom - m_pPet->GetSize().cy) - m_pPet->GetBoundsRect().top ) );
	
	m_pPet->MoveTo( m_nToX, m_nToY );

	//deal with error (fixme?)
	/*
	if( fLoadProblems )
		throw ICON_EXCEPTION;
	*/
}

CNeko::~CNeko()
{
	delete m_pPet;
    for( int i = 0; i < 8; i++ ) if( m_hIconFootprints[i] ) DestroyIcon( m_hIconFootprints[i] );
}

BOOL CNeko::MoveStart()
{
    return( !(( m_nOldToX >= m_nToX-(int)m_dwIdleSpace ) && 
              ( m_nOldToX <= m_nToX+(int)m_dwIdleSpace ) &&
              ( m_nOldToY >= m_nToY-(int)m_dwIdleSpace ) && 
              ( m_nOldToY <= m_nToY+(int)m_dwIdleSpace )));
}

void CNeko::CalcDirection()
{
    State NewState;
    double LargeX, LargeY, Length, SinTheta;

    if( (m_nDX == 0) && (m_nDY == 0) )
        NewState = STOP;
    else 
    {
        LargeX = (double)m_nDX;
        LargeY = (double)(-m_nDY);
        Length = sqrt(LargeX * LargeX + LargeY * LargeY);
        SinTheta = LargeY / Length;

        if( m_nDX > 0 )
        {
            if( SinTheta > g_dSinPiPer8Times3 )
                NewState = U_MOVE;
            else 
                if( (SinTheta <= g_dSinPiPer8Times3 ) && ( SinTheta > g_dSinPiPer8 ) ) 
                    NewState = UR_MOVE;
                else 
                    if( (SinTheta <= g_dSinPiPer8) && (SinTheta > -(g_dSinPiPer8) ) ) 
                        NewState = R_MOVE;
                    else 
                        if( (SinTheta <= -(g_dSinPiPer8) ) && (SinTheta > -(g_dSinPiPer8Times3) ) ) 
                            NewState = DR_MOVE;
                        else 
                            NewState = D_MOVE;
        }
        else 
        {
            if( SinTheta > g_dSinPiPer8Times3 )
                NewState = U_MOVE;
            else 
                if( (SinTheta <= g_dSinPiPer8Times3) && (SinTheta > g_dSinPiPer8) ) 
                    NewState = UL_MOVE;
                else 
                    if( (SinTheta <= g_dSinPiPer8) && (SinTheta > -(g_dSinPiPer8) ) )
                        NewState = L_MOVE;
                    else 
                        if( (SinTheta <= -(g_dSinPiPer8)) && (SinTheta > -(g_dSinPiPer8Times3) ) ) 
                            NewState = DL_MOVE;
                        else 
                            NewState = D_MOVE;
        }
    }

    if( m_State != NewState ) SetState( NewState );

}

void CNeko::RunTowards(int nX, int nY)
{
	//store old and new target
    m_nOldToX = m_nToX; m_nOldToY = m_nToY;
    m_nToX = nX; m_nToY = nY;

	//calculate distance to target and set delta positions
    double dLargeX, dLargeY, dDoubleLength, dLength;
    dLargeX = (double)(m_nToX - m_pPet->GetPosition().x - (int)m_pPet->GetSize().cx / 2); //stop in middle of cursor
    dLargeY = (double)(m_nToY - m_pPet->GetPosition().y - (int)m_pPet->GetSize().cy + 1); //...and just above
    dDoubleLength = dLargeX * dLargeX + dLargeY * dLargeY;

    if( dDoubleLength != 0.0 ) 
    {
        dLength = sqrt( dDoubleLength );
        if( dLength <= (int)m_dwSpeed ) 
        {
			//less than top speed - jump the gap!
            m_nDX = (int)dLargeX;
            m_nDY = (int)dLargeY;
        } 
        else 
        {
			//more than top speed - run at top speed towards target
            m_nDX = (int)(((int)m_dwSpeed * dLargeX) / dLength );
            m_nDY = (int)(((int)m_dwSpeed * dLargeY) / dLength );
        }
    } 
    else //we're at the target - stop
		m_nDX = m_nDY = 0;

    //increment animation counter
    if ( ++m_uTickCount >= MAX_TICK ) m_uTickCount = 0;
    
    if ( m_uTickCount%2 == 0 )
        if (m_uStateCount < MAX_TICK) m_uStateCount++;

    //change state
    switch( m_State ) 
    {
        case STOP:
            if( MoveStart() ) 
                SetState( AWAKE ); 
            else
                if( m_uStateCount >= STOP_TIME ) 
                    if( m_nDX < 0 && m_pPet->GetPosition().x <= 0 ) SetState( L_CLAW ); 
                    else 
                        if( m_nDX > 0 && m_pPet->GetPosition().x >= ( m_pPet->GetBoundsRect().right - m_pPet->GetBoundsRect().left ) - m_pPet->GetSize().cx ) SetState( R_CLAW ); 
                        else 
                            if( m_nDY < 0 && m_pPet->GetPosition().y <= 0 ) SetState( U_CLAW ); 
                            else
                                if( m_nDY > 0 && m_pPet->GetPosition().y >= ( m_pPet->GetBoundsRect().bottom - m_pPet->GetBoundsRect().top ) - m_pPet->GetSize().cy ) SetState( D_CLAW );
                                else SetState( WASH );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;

        case WASH:
            if( MoveStart() ) SetState( AWAKE ); 
                else if( m_uStateCount >= WASH_TIME ) SetState( SCRATCH );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;

        case SCRATCH:
            if( MoveStart() ) SetState( AWAKE ); 
                else if (m_uStateCount >= SCRATCH_TIME ) SetState( YAWN );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;

        case YAWN:
            if( MoveStart() )  SetState( AWAKE );
                else if (m_uStateCount >= YAWN_TIME) SetState( SLEEP );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;
        
        case SLEEP:
            if( MoveStart() ) SetState( AWAKE );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;
        
        case AWAKE:
            if( m_uStateCount >= (UINT)(AWAKE_TIME + (rand()%20)) ) CalcDirection();
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;
       
        case U_MOVE:
        case D_MOVE:
        case L_MOVE:
        case R_MOVE:
        case UL_MOVE:
        case UR_MOVE:
        case DL_MOVE:
        case DR_MOVE:
		{
			//make sure Neko does not go outside boundary area
            int nX = m_pPet->GetPosition().x, nY = m_pPet->GetPosition().y;
			int nNewX = nX + m_nDX, nNewY = nY + m_nDY;
            int nWidth = ( m_pPet->GetBoundsRect().right - m_pPet->GetBoundsRect().left ) - m_pPet->GetSize().cx;
            int nHeight = ( m_pPet->GetBoundsRect().bottom - m_pPet->GetBoundsRect().top ) - m_pPet->GetSize().cy;
			BOOL fOutside = ( nNewX <= 0 || nNewX >= nWidth || nNewY <= 0 || nNewY >= nHeight );

			//change the image and move Neko
	        CalcDirection();

            //clip new x and y positions and see if we've moved anywhere
            if( nNewX < 0 ) nNewX = 0; else if( nNewX > nWidth ) nNewX = nWidth;
            if( nNewY < 0 ) nNewY = 0; else if( nNewY > nHeight ) nNewY = nHeight;
            BOOL fNotMoved = ( nNewX == nX ) && ( nNewY == nY );

            //stop if we can't go any further
            if( fOutside && fNotMoved )
                SetState(STOP);
			else
            {
				m_pPet->SetImageAndMoveTo( GetStateAnimationFrameIndex(), nNewX, nNewY );
                if( m_bFootprints )
                {
                    int iFpAnim = -1;
                    switch( m_State )
                    {
                        case U_MOVE:  iFpAnim = 0; break;
                        case D_MOVE:  iFpAnim = 4; break;
                        case L_MOVE:  iFpAnim = 6; break;
                        case R_MOVE:  iFpAnim = 2; break;
                        case UL_MOVE: iFpAnim = 7; break;
                        case UR_MOVE: iFpAnim = 1; break;
                        case DL_MOVE: iFpAnim = 5; break;
                        case DR_MOVE: iFpAnim = 3; break;
                    }
                    if( iFpAnim != -1 )
                    {
                        if( m_uTickCount & 1 )
                            m_pPet->DrawOnTarget( nX-(m_nDY/2), nY, m_hIconFootprints[iFpAnim] );
                        else
                            m_pPet->DrawOnTarget( nX, nY-(m_nDX/2), m_hIconFootprints[iFpAnim] );
                    }
                }
            }
            break;
		}
        
        case U_CLAW:
        case D_CLAW:
        case L_CLAW:
        case R_CLAW:
            if( MoveStart() ) SetState( AWAKE );
                else if( m_uStateCount >= CLAW_TIME ) SetState( SCRATCH );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;

        default:
            //something bad has happened!
            MessageBeep( 0xFFFFFFFF );
            SetState( STOP );
			m_pPet->SetImage( GetStateAnimationFrameIndex() );
            break;
    }
}

int CNeko::GetStateAnimationFrameIndex()
{
    if ( m_State != SLEEP )
		return m_nAnimation[m_State][m_uTickCount & 0x1];
    else 
        return m_nAnimation[m_State][(m_uTickCount>>2) & 0x1];

}

void CNeko::SetState( State state )
{
    //reset the animation counters
    m_uTickCount = 0;
    m_uStateCount = 0;

    //update the state
    m_State = state;
}


BOOL CNeko::LoadImages()
{
	/* Note: The icons should be in the following order in the file:

	    Awake
		Up 1
		Up 2
		Up Right 1
		Up Right 2
		Right 1
		Right 2
		Down Right 1
		Down Right 2
		Down 1
		Down 2
		Down Left 1
		Down Left 2
		Left 1
		Left 2
		Up Left 1
		Up Left 2
		Up Claw 1
		Up Claw 2
		Right Claw 1
		Right Claw 2
		Left Claw 1
		Left Claw 2
		Down Claw 1
		Down Claw 2
		Wash 2
		Scratch 1
		Scratch 2
		Yawn 1
		Yawn 2
		Sleep 1
		Sleep 2
	*/

	//load the icons
	int n;
	HICON hIcons[32];
	for( n = 0; n < 32; n++ )
		hIcons[n] =  ExtractIcon( g_hInstance, m_szLibname, n );

	//check last icon
	if( (UINT)hIcons[31] <= 1 )
	{
		//error - delete all icons
		for( n = 0; n < 32; n++ ) DestroyIcon( hIcons[n] );

        char szBuffer[1024];
        wsprintf( szBuffer, "There are not enough icons in this icon library\n%s\nIt must contain at least 32 icons", m_szLibname );
        MessageBox( NULL, szBuffer, "Error", MB_ICONERROR|MB_TASKMODAL );
		return FALSE;
	}

	//apply icons
	m_pPet->SetImages( hIcons, 32 );

    //destroy icon table
    for( n = 0; n < 32; n++ ) DestroyIcon( hIcons[n] );

    //load footprints
    if( m_bFootprints )
    {
        if( *m_szFootprintLibname )
            for( n = 0; n < 8; n++ ) m_hIconFootprints[n] = ExtractIcon( g_hInstance, m_szFootprintLibname, n );
        else
        {
            UINT uID[] = { IDI_FP_UP, IDI_FP_UPRIGHT, IDI_FP_RIGHT, IDI_FP_DOWNRIGHT, IDI_FP_DOWN, IDI_FP_DOWNLEFT, IDI_FP_LEFT, IDI_FP_UPLEFT };
            for( n = 0; n < 8; n++ ) m_hIconFootprints[n] = LoadIcon( g_hInstance, MAKEINTRESOURCE(uID[n]) );
        }
    }

	return TRUE;
}

/********************************************************************************************************/

void CNeko::Update()
{
    //apply VVPAI (very, very poor artificial intelligence!!!)
    switch( m_Action )
    {
        default:
        case CHASE_MOUSE:
        {
	        POINT pt;
	        GetCursorPos( &pt );
	        RunTowards( pt.x, pt.y );
            break;
        }

        case RUN_AWAY_FROM_MOUSE:
        {
            POINT pt;
            int xdiff, ydiff;
            GetCursorPos( &pt );
			DWORD dwLimit = m_dwIdleSpace*16;

			xdiff = ( m_pPet->GetPosition().x + (m_pPet->GetSize().cx/2) ) - pt.x;
			ydiff = ( m_pPet->GetPosition().y + (m_pPet->GetSize().cy/2) ) - pt.y;

			if( abs(xdiff) < (int)dwLimit && abs(ydiff) < (int)dwLimit )
			{
				//mouse cursor is too close
				int x, y;
				double dLength = sqrt(xdiff*xdiff + ydiff*ydiff);
				if( dLength != 0.0 )
				{
					x = m_pPet->GetPosition().x + (int)((xdiff / dLength) * dwLimit);
					y = m_pPet->GetPosition().y + (int)((ydiff / dLength) * dwLimit);
				}
				else
					x = y = 32;

				//make Neko run away from the mouse
                RunTowards( x, y );
                if( m_State == AWAKE ) CalcDirection(); //don't show awake animation
			}
			else
				RunTowards( m_nToX, m_nToY ); //keep running...
			break;
		}

        case RUN_AROUND_RANDOMLY:
        {
            if( m_State == SLEEP) m_nActionCount++;
            if( m_nActionCount > (int)m_dwIdleSpace*10 )
            {
                m_nActionCount = 0;
                RunTowards( m_pPet->GetBoundsRect().left + (rand() % (m_pPet->GetBoundsRect().right-m_pPet->GetBoundsRect().left)), m_pPet->GetBoundsRect().top + (rand() % (m_pPet->GetBoundsRect().bottom-m_pPet->GetBoundsRect().top)) );
            }
            else
                RunTowards( m_nToX, m_nToY );
            break;
        }

        case PACE_AROUND_SCREEN:
        {
            if( (m_nDX == 0) && (m_nDY == 0) ) m_nActionCount = ( m_nActionCount + 1 ) % 4;
            switch( m_nActionCount )
            {
                case 0: RunTowards( m_pPet->GetBoundsRect().left + m_pPet->GetSize().cx, m_pPet->GetBoundsRect().top + m_pPet->GetSize().cy ); break;
                case 1: RunTowards( m_pPet->GetBoundsRect().left + m_pPet->GetSize().cx, m_pPet->GetBoundsRect().bottom - m_pPet->GetSize().cy ); break;
                case 2: RunTowards( m_pPet->GetBoundsRect().right - m_pPet->GetSize().cx, m_pPet->GetBoundsRect().bottom - m_pPet->GetSize().cy ); break;
                case 3: RunTowards( m_pPet->GetBoundsRect().right - m_pPet->GetSize().cx, m_pPet->GetBoundsRect().top + m_pPet->GetSize().cy ); break;
            }
            break;
        }

        case RUN_AROUND:
        {
			//bounding box repel border
			DWORD dwBoundingBox = m_dwSpeed * 8;

			//move invisible ball
            m_nActionX += m_nActionDX;
            m_nActionY += m_nActionDY;
			
			//repel invisible ball from the edges of the screen.
			if( m_nActionX < (int)(m_pPet->GetBoundsRect().left + dwBoundingBox) )
				if( m_nActionX > m_pPet->GetBoundsRect().left ) m_nActionDX++; else m_nActionDX = -m_nActionDX;
			else
				if( m_nActionX > (int)(m_pPet->GetBoundsRect().right - dwBoundingBox) )
					if( m_nActionX < m_pPet->GetBoundsRect().right ) m_nActionDX--; else m_nActionDX = -m_nActionDX;

			if( m_nActionY < (int)(m_pPet->GetBoundsRect().top + dwBoundingBox) )
				if( m_nActionY > m_pPet->GetBoundsRect().top ) m_nActionDY++; else m_nActionDY = -m_nActionDY;
			else
				if( m_nActionY > (int)(m_pPet->GetBoundsRect().bottom - dwBoundingBox) )
					if( m_nActionY < m_pPet->GetBoundsRect().bottom ) m_nActionDY--; else m_nActionDY = -m_nActionDY;

			//tell Neko to run towards the new point
            RunTowards( m_nActionX, m_nActionY );
            break;
        }
    }

    //play idle sounds
    if( m_dwSndFrequency )
    {
        if( (DWORD)(rand()%100) <= m_dwSndFrequency )
        {
            switch( GetState() )
            {
                case AWAKE:
                    PlaySound( m_szSndAwake, NULL, SND_NOSTOP|SND_NOWAIT|SND_FILENAME|SND_NODEFAULT|SND_ASYNC );
                    break;

                case SLEEP:
                    PlaySound( m_szSndSleep, NULL, SND_NOSTOP|SND_NOWAIT|SND_FILENAME|SND_NODEFAULT|SND_ASYNC );
                    break;

				default:
                    switch( rand()%3 )
                    {
                        case 0:  PlaySound( m_szSndIdle1, NULL, SND_NOSTOP|SND_NOWAIT|SND_FILENAME|SND_NODEFAULT|SND_ASYNC ); break;
                        case 1:  PlaySound( m_szSndIdle2, NULL, SND_NOSTOP|SND_NOWAIT|SND_FILENAME|SND_NODEFAULT|SND_ASYNC ); break;
                        default: PlaySound( m_szSndIdle3, NULL, SND_NOSTOP|SND_NOWAIT|SND_FILENAME|SND_NODEFAULT|SND_ASYNC ); break;
                    }
                break;
            }
        }
    }
}

