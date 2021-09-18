// Pet.cpp: implementation of the CPet class.
//
//////////////////////////////////////////////////////////////////////

#include "Pet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPet::CPet()
{
	//prepare empty icon table
	m_nIcons = 0;
	m_nLastIcon = -1;
	m_hIcons = NULL;

	//set initial and old position
	m_ptPosition.x = 0;
	m_ptPosition.y = 0;
	m_ptOldPosition.x = -1;
	m_ptOldPosition.y = -1;

	//set initial size and scale
	m_sizeImage.cx = 0;
	m_sizeImage.cy = 0;
    m_fScale = 1.0f;

	//set initial bounding box (none)
	SetRect( &m_rcBounds, -1, -1, -1, -1 );
}



CPet::~CPet()
{
	//delete the images
	DestroyImages();
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


void CPet::SetImages( HICON* hIconTable, int nIcons )
{
	//delete current images
	DestroyImages();

	//we can't have 0 images
	if( nIcons <= 0 ) 
	{
		nIcons = 1;
#ifdef DEBUG
		OutputDebugString( "WARNING: CPet::SetImages() passed 0 as nIcons\n" );
#endif
	}

    //FIXME: it might be possible to use CopyImage() passing it the *scaled* width and height
    //in combination with the LR_COPYFROMRESOURCE. This might fix the non-32x32 icon problem

	//allocate icon table and clear all entries
	m_hIcons = new HICON[nIcons];
	for( int i = 0; i < nIcons; i++ ) m_hIcons[i] = CopyIcon(hIconTable[i]);

	//store icon table size and set last drawn icon index
	m_nIcons = nIcons;
	m_nLastIcon = -1;

	//get size of first icon
    ICONINFO ii;
    GetIconInfo( m_hIcons[0], &ii );

	BITMAP bm;
	GetObject( ii.hbmMask, sizeof(BITMAP), &bm );
    DeleteObject( ii.hbmMask );
    DeleteObject( ii.hbmColor );

    //calculate scaled size
	m_sizeImage.cx = int( bm.bmWidth * m_fScale );
	m_sizeImage.cy = int( bm.bmHeight * m_fScale );
}



void CPet::DestroyImages()
{
	//delete all images, if any
	if( m_nIcons > 0 )
	{
		//delete all icons in the table
		for( int i = 0; i < m_nIcons; i++ ) DestroyIcon( m_hIcons[i] );

		//delete the table
		delete[] m_hIcons;
		m_hIcons = NULL;
	}
}



void CPet::SetImage( int nImage )
{
	//only draw if the image has changed
	if( nImage != m_nLastIcon )
	{
		Erase();
		Draw( nImage );
		m_nLastIcon = nImage;
	}
}



void CPet::MoveTo(int nNewX, int nNewY)
{
	if( m_ptOldPosition.x == -1 )
	{
		//set old position to the current position as we've not been moved before
		m_ptOldPosition.x = nNewX;
		m_ptOldPosition.y = nNewY;
	}
	else
	{
		//store current position
		m_ptOldPosition.x = m_ptPosition.x;
		m_ptOldPosition.y = m_ptPosition.y;
	}

	//remove current
	Erase();

	//change current position
	m_ptPosition.x = nNewX;
	m_ptPosition.y = nNewY;

	//redraw
	Draw( m_nLastIcon );
}

void CPet::SetImageAndMoveTo(int nImage, int nNewX, int nNewY)
{
	//change image
	m_nLastIcon = nImage;

	//move
	MoveTo( nNewX, nNewY );
}


void CPet::SetScale(float fScale)
{
    if( fScale != 0.0f )
        m_fScale = fScale;
    else
        m_fScale = 1.0f;
}
