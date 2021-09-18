// AlwaysOnTopPet.cpp: implementation of the CAlwaysOnTopPet class.
//
//////////////////////////////////////////////////////////////////////

#include "AlwaysOnTopPet.h"

//class name constant
static const char* g_szOnTopClassName = "NekoOnTop_Wnd";

//forward declaration
LRESULT CALLBACK WndProc_OnTop( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//static member
BOOL CAlwaysOnTopPet::m_fRegisteredClass = FALSE;

//external global variable
extern HINSTANCE g_hInstance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlwaysOnTopPet::CAlwaysOnTopPet() : CPet()
{
	//clear regions
	m_hRgns = NULL;

	//initialise members
	m_fBeingDragged = FALSE;
	m_hWndOnTop = NULL;

	//register class
	if( m_fRegisteredClass == FALSE )
	{
		WNDCLASS wc;

		wc.style         = CS_OWNDC|CS_DBLCLKS|CS_SAVEBITS;
		wc.lpfnWndProc   = (WNDPROC)WndProc_OnTop;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(LPVOID);
		wc.hInstance     = g_hInstance;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor( NULL, MAKEINTRESOURCE(IDC_ARROW) );
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = g_szOnTopClassName;

		m_fRegisteredClass = RegisterClass(&wc);
	}

	//set bounding rectangle
	SetRect( &m_rcBounds, 0, 0, GetSystemMetrics(SM_CXSCREEN)-1, GetSystemMetrics(SM_CYSCREEN)-1 );

    //move this pet off-screen to start with
    m_ptPosition.x = m_rcBounds.right;
    m_ptPosition.y = m_rcBounds.bottom;
}

CAlwaysOnTopPet::~CAlwaysOnTopPet()
{
	DestroyWindow( m_hWndOnTop );
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CAlwaysOnTopPet::Draw( int nImage )
{
	//only draw if it's different && not being dragged
	if( nImage != m_nLastIcon && m_fBeingDragged == FALSE )
	{
		//clip the window to the shape of the icon
		HRGN hRgnCopy = CreateRectRgn( 0, 0, m_sizeImage.cx, m_sizeImage.cy );
		CombineRgn( hRgnCopy, m_hRgns[nImage], NULL, RGN_COPY );
		SetWindowRgn( m_hWndOnTop, hRgnCopy, TRUE );

		//draw the current frame on the window
		HDC hDC = GetDC( m_hWndOnTop );
		DrawIconEx( hDC, 0, 0, m_hIcons[nImage], m_sizeImage.cx, m_sizeImage.cy, 0, NULL, DI_NORMAL );
		ReleaseDC( m_hWndOnTop, hDC );
	}
}

void CAlwaysOnTopPet::Erase()
{
	//do nothing
}

void CAlwaysOnTopPet::SetImages(HICON * hIconTable, int nIcons )
{
	//remove current region handles
	DestroyRegions();

	//call base class
	CPet::SetImages( hIconTable, nIcons );

	//prepare region handles
    BuildRegions();

	//create the window if it doesn't exist already
	if( m_hWndOnTop == NULL )
	{
        m_hWndOnTop = CreateWindowEx( WS_EX_TOPMOST|WS_EX_TOOLWINDOW, g_szOnTopClassName, NULL, WS_POPUP, m_ptPosition.x, m_ptPosition.y, m_sizeImage.cx, m_sizeImage.cy, NULL, NULL, g_hInstance, NULL );

        if( m_hWndOnTop )
        {
            SetWindowLong( m_hWndOnTop, 0, (LONG)this );
            ShowWindow( m_hWndOnTop, SW_SHOWNA );
            UpdateWindow( m_hWndOnTop );
        }
	}

	//FIXME: don't change it in the whole class, just this window!!!
	//change it's default icon
	//SetClassLong( m_hWndOnTop, GCL_HICON, m_hIcons[0] );
}

void CAlwaysOnTopPet::DestroyImages()
{
	//call base class
	CPet::DestroyImages();

	//delete regions
	DestroyRegions();
}

void CAlwaysOnTopPet::DrawOnTarget( int x, int y, HICON hIcon )
{
    //grab the device context of the display
    HDC hDC = GetDC( NULL );

    //draw the icon on it
    DrawIconEx( hDC, x, y, hIcon, 0, 0, 0, NULL, DI_NORMAL );

    //release the device context
    ReleaseDC( NULL, hDC );
}





//////////////////////////////////////////////////////////////////////
// On Top Window Procedure
//////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc_OnTop( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_PAINT:
		{
            //draw the most recent icon if the window is being dragged
            CAlwaysOnTopPet* pPet = (CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 );
            if( pPet->m_fBeingDragged )
            {
    			//draw the current icon onto the window (we can't call draw because it checks for icon index and changes the window's region
			    HDC hDC = GetDC( hWnd );
			    DrawIconEx( hDC, 0, 0, pPet->m_hIcons[pPet->m_nLastIcon], pPet->GetSize().cx, pPet->GetSize().cy, 0, NULL, DI_NORMAL );
			    ReleaseDC( hWnd, hDC );
            }
            ValidateRect( hWnd, NULL );
            break;
		}

		case WM_SYSCOMMAND:
			//if the user alt+F4s us or (somehow) minimises or maximises us, ignore it
			if( LOWORD(wParam) != SC_CLOSE && LOWORD(wParam) != SC_MINIMIZE && LOWORD(wParam) != SC_MAXIMIZE ) 
			    return DefWindowProc( hWnd, uMsg, wParam, lParam );
			break;

        case WM_ERASEBKGND:
            return TRUE; //don't erase the background

        //pass mouse messages onto the class
        case WM_LBUTTONDOWN:   ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnLButtonDown(); break;
        case WM_LBUTTONUP:     ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnLButtonUp(); break;
        case WM_LBUTTONDBLCLK: ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnLButtonDblClk(); break;

        case WM_MBUTTONDOWN:   ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnMButtonDown(); break;
        case WM_MBUTTONUP:     ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnMButtonUp(); break;
        case WM_MBUTTONDBLCLK: ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnMButtonDblClk(); break;

        case WM_RBUTTONDOWN:   ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnRButtonDown(); break;
        case WM_RBUTTONUP:     ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnRButtonUp(); break;
        case WM_RBUTTONDBLCLK: ((CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 ))->OnRButtonDblClk(); break;

        //window is being dragged
        case WM_ENTERSIZEMOVE:
        {
            CAlwaysOnTopPet* pPet = (CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 );
            pPet->m_fBeingDragged = TRUE;
            break;
        }

        //window is being dropped
        case WM_EXITSIZEMOVE:
        {
            CAlwaysOnTopPet* pPet = (CAlwaysOnTopPet*)GetWindowLong( hWnd, 0 );
            pPet->m_fBeingDragged = FALSE;
            RECT rc;
            GetWindowRect( hWnd, &rc );
            pPet->MoveTo( rc.left, rc.top );
            break;
        }

        default:
           return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }
    return 0;
}

void CAlwaysOnTopPet::OnLButtonDown()
{
    //default left button handler - begin window dragging
    SendMessage( m_hWndOnTop, WM_SYSCOMMAND, SC_MOVE+2, 0 );
}

void CAlwaysOnTopPet::DestroyRegions()
{
    if( m_hRgns )
    {
    	//delete all regions and free the array
    	for( int i = 0; i < m_nIcons; i++ ) if( m_hRgns[i] ) DeleteObject( m_hRgns[i] );
    	delete[] m_hRgns;
        m_hRgns = NULL;
    }
}

void CAlwaysOnTopPet::MoveTo(int nNewX, int nNewY)
{
	if( m_fBeingDragged == FALSE )
    {
	    //store current position
	    m_ptOldPosition.x = m_ptPosition.x;
	    m_ptOldPosition.y = m_ptPosition.y;

	    //change current position
	    m_ptPosition.x = nNewX;
	    m_ptPosition.y = nNewY;

	    //move the window if it's not being moved by something else
		MoveWindow( m_hWndOnTop, nNewX, nNewY, m_sizeImage.cx, m_sizeImage.cy, TRUE );
    }
}


void CAlwaysOnTopPet::SetImageAndMoveTo(int nImage, int nNewX, int nNewY)
{
    if( m_fBeingDragged == FALSE )
    {
	    //move
	    MoveTo( nNewX, nNewY );

	    //change image
	    Draw( nImage );
	    m_nLastIcon = nImage;
    }
}

void CAlwaysOnTopPet::SetImage( int nImage )
{
    if( m_fBeingDragged == FALSE ) CPet::SetImage( nImage );
}



//This function was based on the BitmapToRegion function found in www.codeguru.com's "bitmaps and palettes" section
//	Author : Jean-Edouard Lachand-Robert (http://www.geocities.com/Paris/LeftBank/1160/resume.htm), June 1998.
void CAlwaysOnTopPet::BuildRegions()
{
	//create a memory DC inside which we will scan the bitmap content
	HDC hMemDC = CreateCompatibleDC(NULL);

	//create a 32 bits depth bitmap and select it into the memory DC 
    BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), int(m_sizeImage.cx/m_fScale), int(m_sizeImage.cy/m_fScale), 1, 16, BI_RGB, 0, 0, 0, 0, 0 };
	VOID* pBitsDib; 
	HBITMAP hBmDib = CreateDIBSection( hMemDC, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pBitsDib, NULL, 0 );
	HBITMAP hOldMemBmp = (HBITMAP)SelectObject( hMemDC, hBmDib );

	//create a DC just to copy the bitmap into the memory DC
	HDC hDC = CreateCompatibleDC( hMemDC );

	//get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
	BITMAP bmDib;
	GetObject( hBmDib, sizeof(bmDib), &bmDib );
	while( bmDib.bmWidthBytes % 4 ) bmDib.bmWidthBytes++;

    //calculate scaling matrix
    XFORM xForm = { m_fScale, 0.0, 0.0, m_fScale, 0.0, 0.0 };

    //allocate the region array
	m_hRgns = new HRGN[m_nIcons];

    //build all regions
	for( int i = 0; i < m_nIcons; i++ )
    {
        HRGN hRgn = NULL;

        //extract icon mask image
        ICONINFO ii;
        GetIconInfo( m_hIcons[i], &ii );
        DeleteObject( ii.hbmColor );

		//get bitmap size
		BITMAP bm;
		GetObject( ii.hbmMask, sizeof(bm), &bm );

		//copy the bitmap into the memory DC
		HBITMAP hOldBmp = (HBITMAP)SelectObject( hDC, ii.hbmMask );
		BitBlt( hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY );

        //For better performances, we will use the ExtCreateRegion() function to create the
		//region. This function take a RGNDATA structure on entry. We will add rectangles by
		//amount of ALLOC_UNIT number in this structure.
		#define ALLOC_UNIT 100
		DWORD dwMaxRects = ALLOC_UNIT;
		HANDLE hData = GlobalAlloc( GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * dwMaxRects) );
		RGNDATA* pData = (RGNDATA *)GlobalLock( hData );
		pData->rdh.dwSize = sizeof(RGNDATAHEADER);
		pData->rdh.iType = RDH_RECTANGLES;
		pData->rdh.nCount = pData->rdh.nRgnSize = 0;
		SetRect( &pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0 );

		//scan each bitmap row from bottom to top (the bitmap is inverted vertically)
		BYTE* pDib = (BYTE*)bmDib.bmBits + (bmDib.bmHeight - 1) * bmDib.bmWidthBytes;
		for( int y = 0; y < bm.bmHeight; y++ )
		{
			//scan each bitmap pixel from left to right
			for( int x = 0; x < bm.bmWidth; x++ )
			{
				//search for a continuous range of "non transparent pixels"
				int x0 = x;
                WORD* p = (WORD*)pDib + x;
				while( x < bm.bmWidth)
				{
                    if( *p != 0 )
						//This pixel is "transparent"
						break;

					p++;
					x++;
				}

				if( x > x0 )
				{
					//Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
					if( pData->rdh.nCount >= dwMaxRects )
					{
						GlobalUnlock(hData);
						dwMaxRects += ALLOC_UNIT;
						hData = GlobalReAlloc( hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * dwMaxRects), GMEM_MOVEABLE );
						pData = (RGNDATA*)GlobalLock(hData);
					}
					RECT* pr = (RECT*)&pData->Buffer;
					SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
					if (x0 < pData->rdh.rcBound.left) pData->rdh.rcBound.left = x0;
					if (y < pData->rdh.rcBound.top)   pData->rdh.rcBound.top = y;
					if (x > pData->rdh.rcBound.right) pData->rdh.rcBound.right = x;
					if (y+1 > pData->rdh.rcBound.bottom) pData->rdh.rcBound.bottom = y+1;
					pData->rdh.nCount++;

					//On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
					//large (ie: > 4000). Therefore, we have to create the region by multiple steps.
					if( pData->rdh.nCount == 2000 )
					{
						HRGN h = ExtCreateRegion( &xForm, sizeof(RGNDATAHEADER) + (sizeof(RECT) * dwMaxRects), pData);
						if( hRgn )
						{
							CombineRgn( hRgn, hRgn, h, RGN_OR );
							DeleteObject(h);
						}
						else
							hRgn = h;

						pData->rdh.nCount = 0;
						SetRect( &pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0 );
					}
				}
			}

			//go to next row (remember, the bitmap is inverted vertically)
			pDib -= bmDib.bmWidthBytes;
		}

		//create or extend the region with the remaining rectangles
		HRGN h = ExtCreateRegion( &xForm, sizeof(RGNDATAHEADER) + (sizeof(RECT) * dwMaxRects), pData );
		if( hRgn )
		{
			CombineRgn( hRgn, hRgn, h, RGN_OR );
			DeleteObject(h);
		}
		else
			hRgn = h;

		//clean up
        DeleteObject( SelectObject( hDC, hOldBmp ) );
        GlobalFree(hData);
    
        //store the region
        m_hRgns[i] = hRgn;
    }

    //clean up
	DeleteDC(hDC);
	DeleteObject( SelectObject( hMemDC, hOldMemBmp ) );
	DeleteDC( hMemDC );
}
