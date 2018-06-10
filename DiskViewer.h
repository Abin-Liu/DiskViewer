// DiskViewer.h : main header file for the DISKVIEWER application
//

#if !defined(AFX_DISKVIEWER_H__0684297D_C2A4_40DD_9789_3A4A58F7FB23__INCLUDED_)
#define AFX_DISKVIEWER_H__0684297D_C2A4_40DD_9789_3A4A58F7FB23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerApp:
// See DiskViewer.cpp for the implementation of this class
//

class CDiskViewerApp : public CWinApp
{
public:
	CDiskViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDiskViewerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKVIEWER_H__0684297D_C2A4_40DD_9789_3A4A58F7FB23__INCLUDED_)
