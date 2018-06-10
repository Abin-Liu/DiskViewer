#if !defined(AFX_OPENFILEDLG_H__C07A84CA_5756_4089_BDC6_88850C194575__INCLUDED_)
#define AFX_OPENFILEDLG_H__C07A84CA_5756_4089_BDC6_88850C194575__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenFileDlg.h : header file
//
#include "BrowseCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// COpenFileDlg dialog

class COpenFileDlg : public CDialog
{
// Construction
public:
	CString GetDefectFile() const;
	CString GetZoneFile() const;
	COpenFileDlg(LPCTSTR lpZoneFile = NULL, LPCTSTR lpDefectFile = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenFileDlg)
	enum { IDD = IDD_OPENFILEDLG };
	CBrowseCtrl	m_wndZoneFile;
	CBrowseCtrl	m_wndDefectFile;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenFileDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnFileNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENFILEDLG_H__C07A84CA_5756_4089_BDC6_88850C194575__INCLUDED_)
