// Pet.h: interface for the CPet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PET_H__A69EBAA0_385D_11D2_9FF9_00001C192944__INCLUDED_)
#define AFX_PET_H__A69EBAA0_385D_11D2_9FF9_00001C192944__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CPet {

public:
	//constructor/destructor
	CPet();
	virtual ~CPet();

	//public configuration functions
	virtual void SetImages( HICON* hIconTable, int nIcons );
	virtual void SetScale( float fScale );

	//public interface
	virtual void SetImageAndMoveTo( int nImage, int nNewX, int nNewY );
	virtual void MoveTo( int nNewX, int nNewY );
	virtual void SetImage( int nImage );

    virtual void DrawOnTarget( int x, int y, HICON hIcon ) {}; //no default implementation

	//data member access
	inline RECT   GetBoundsRect()  { return m_rcBounds; };
	inline POINT& GetPosition()	   { return m_ptPosition; };
	inline POINT& GetOldPosition() { return m_ptOldPosition; };
    inline SIZE   GetSize()        { return m_sizeImage; };

protected:

	//pure virtual functions
	virtual void Erase() = 0;
	virtual void Draw( int nImage ) = 0;

	//image destruction
	virtual void DestroyImages();

	//icon information
	HICON* m_hIcons;
	int m_nIcons;
	int m_nLastIcon;

	//position & size information
	POINT m_ptPosition;
	POINT m_ptOldPosition;
	SIZE  m_sizeImage;
	float m_fScale;

	//bounding box information
	RECT m_rcBounds;
};

#endif // !defined(AFX_PET_H__A69EBAA0_385D_11D2_9FF9_00001C192944__INCLUDED_)
