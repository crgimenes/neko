// DesktopPet.cpp: implementation of the CDesktopPet class.
//
//////////////////////////////////////////////////////////////////////

#include "DesktopPet.h"

//constants
#define UPDATE_COUNTER_MAX 50 //number of redraws until desktop handle is recalculated

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDesktopPet::CDesktopPet() : CDesktopHack(), CPet()
{
	//clear the update counter
	m_nUpdateHWndCounter = 0;

	//set bounds
	GetDesktopRect( m_rcBounds );
}

CDesktopPet::~CDesktopPet()
{
	RedrawWindow( m_hWndDesktop, NULL, NULL, RDW_INVALIDATE|RDW_ERASE );
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CDesktopPet::Draw( int nImage )
{
	//grab the device context of the desktop
	HDC hDC = GetDC( m_hWndDesktop );
	
	//draw the icon on it
    DrawIconEx( hDC, m_ptPosition.x, m_ptPosition.y, m_hIcons[nImage], m_sizeImage.cx, m_sizeImage.cy, 0, NULL, DI_NORMAL );

	//release the device context
    ReleaseDC( m_hWndDesktop, hDC );

	//increment the update counter and update if required
	if( ++m_nUpdateHWndCounter > UPDATE_COUNTER_MAX )
	{
		FindDesktopHandle();
		GetDesktopRect( m_rcBounds );
	}
}

void CDesktopPet::Erase()
{
	//set up rect to be erased
    RECT rcErase = { m_ptOldPosition.x, m_ptOldPosition.y, 
                     m_ptOldPosition.x + m_sizeImage.cx, m_ptOldPosition.y + m_sizeImage.cy };

	//invalidate it and tell it to update
    InvalidateRect( m_hWndDesktop, &rcErase, TRUE ); 
	UpdateWindow( m_hWndDesktop );
}

void CDesktopPet::DrawOnTarget( int x, int y, HICON hIcon )
{
    //grab the device context of the desktop
    HDC hDC = GetDC( m_hWndDesktop );

    //draw the icon on it
    DrawIconEx( hDC, x, y, hIcon, 0, 0, 0, NULL, DI_NORMAL );

    //release the device context
    ReleaseDC( m_hWndDesktop, hDC );
}

