// DesktopHack.h: interface for the CDesktopHack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESKTOPHACK_H__A69EBAA1_385D_11D2_9FF9_00001C192944__INCLUDED_)
#define AFX_DESKTOPHACK_H__A69EBAA1_385D_11D2_9FF9_00001C192944__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CDesktopHack  
{
public:
	void GetDesktopRect( RECT& rcDesktop );
	void FindDesktopHandle();
	CDesktopHack();
	virtual ~CDesktopHack();

protected:
	HWND m_hWndDesktop;

	BOOL m_fActiveDesktop;    //the desktop is an active (web) desktop
	BOOL m_fNoChicagoDesktop; //the desktop is not a windows 95/nt4 desktop

};

#endif // !defined(AFX_DESKTOPHACK_H__A69EBAA1_385D_11D2_9FF9_00001C192944__INCLUDED_)
