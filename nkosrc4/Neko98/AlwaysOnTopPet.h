// AlwaysOnTopPet.h: interface for the CAlwaysOnTopPet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALWAYSONTOPPET_H__A69EBAA4_385D_11D2_9FF9_00001C192944__INCLUDED_)
#define AFX_ALWAYSONTOPPET_H__A69EBAA4_385D_11D2_9FF9_00001C192944__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Pet.h"

class CAlwaysOnTopPet : virtual public CPet  
{
public:
	virtual void MoveTo( int nNewX, int nNewY );
	virtual void SetImage( int nImage );
	virtual void SetImageAndMoveTo( int nImage, int nNewX, int nNewY );
	virtual void SetImages( HICON* hIconTable, int nIcons );
	CAlwaysOnTopPet();
	virtual ~CAlwaysOnTopPet();

    virtual void DrawOnTarget( int x, int y, HICON hIcon );

    //virtual callback functions - these can all be overridden to perform different actions
    //depending on the various mouse clicks. Note: CAlwaysOnTopPet::OnLButtonDown() handles
    //the dragging - if this is overridden, OnLButtonUp is not called.

    virtual void OnLButtonDown();   //implementation is in .cpp file
    virtual void OnLButtonUp()      {};
    virtual void OnLButtonDblClk()  {};
    virtual void OnMButtonDown()    {};
    virtual void OnMButtonUp()      {};
    virtual void OnMButtonDblClk()  {};
    virtual void OnRButtonDown()    {};
    virtual void OnRButtonUp()      {};
    virtual void OnRButtonDblClk()  {};

    //class information functions
    inline BOOL IsDragging() { return m_fBeingDragged; };

protected:
	HWND m_hWndOnTop;
	BOOL m_fBeingDragged;
	HRGN IconMaskToRegion( HICON hIcon );
	virtual void DestroyImages();
	HRGN* m_hRgns;
	virtual void Erase();
	virtual void Draw( int nImage );

private:
	void BuildRegions();
	void DestroyRegions();
	static BOOL m_fRegisteredClass;

friend 
	LRESULT CALLBACK WndProc_OnTop( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	
};

#endif // !defined(AFX_ALWAYSONTOPPET_H__A69EBAA4_385D_11D2_9FF9_00001C192944__INCLUDED_)
