#if !defined(AFX_DEFECTDLG_H__EF0BDD45_CCE3_4F82_8737_474D36FBE90B__INCLUDED_)
#define AFX_DEFECTDLG_H__EF0BDD45_CCE3_4F82_8737_474D36FBE90B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefectDlg.h : header file
//

#include "Disk.h"
#include "ReportCtrl.h"

#define WM_DEF_HIGHLIGHT	WM_APP + 153
#define WM_DEF_HIDE			WM_APP + 154

struct DEFECT
{
	ULONG lHead;
	ULONG lCylinder;
	ULONG lSector;
};

/////////////////////////////////////////////////////////////////////////////
// CDefectDlg dialog

class CDefectDlg : public CDialog
{
// Construction
public:
	BOOL GetHighlightRect(CRect* pRect) const;
	const DEFECT* GetDefect() const;
	void Reset();
	CDefectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefectDlg)
	enum { IDD = IDD_DEFECTDLG };
	CReportCtrl	m_wndList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefectDlg)
	public:
	virtual BOOL Create(CView* pView, const CDisk* pDisk);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefectDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnZoomto();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	const CDisk* m_pDisk;
	int m_nCurSel;
	BOOL m_bLoading;
	DEFECT m_defect;
	CView* m_pView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFECTDLG_H__EF0BDD45_CCE3_4F82_8737_474D36FBE90B__INCLUDED_)
