// DiskViewerView.h : interface of the CDiskViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKVIEWERVIEW_H__C4F6F7F5_C80B_4ECC_BF1C_C1018D7DFEB8__INCLUDED_)
#define AFX_DISKVIEWERVIEW_H__C4F6F7F5_C80B_4ECC_BF1C_C1018D7DFEB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DiskViewerDoc.h"
#include "tooltipwnd.h"
#include "DefectDlg.h"

class CDiskViewerView : public CView
{
protected: // create from serialization only
	CDiskViewerView();
	DECLARE_DYNCREATE(CDiskViewerView)

// Attributes
public:
	CDiskViewerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskViewerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDiskViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDiskViewerView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnHead0();
	afx_msg void OnUpdateHead0(CCmdUI* pCmdUI);
	afx_msg void OnHead1();
	afx_msg void OnUpdateHead1(CCmdUI* pCmdUI);
	afx_msg void OnHead2();
	afx_msg void OnUpdateHead2(CCmdUI* pCmdUI);
	afx_msg void OnHead3();
	afx_msg void OnUpdateHead3(CCmdUI* pCmdUI);
	afx_msg void OnCopyimg();
	afx_msg void OnUpdateCopyimg(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnViewDeflist();
	afx_msg void OnUpdateViewDeflist(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnDefectHighlight(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDefectHide(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	
	BOOL GetZoomedMarkRects(CRect& rcTL, CRect& rcTR, CRect& rcBR, CRect& rcBL);
	void CopyImage(LPRECT lpRect = NULL);
	BOOL ShowTooltip(POINT point);
	void Reset(BOOL bQuitZoom);
	CPoint m_ptStart;
	CPoint m_ptEnd;
	BOOL m_bBtnDown;
	BOOL m_bShowDefList;
	CTooltipWnd m_wndTooltip;
	CDefectDlg m_wndDefList;
	CRect m_rectHighlight;
	BOOL m_bHighlight;
};

#ifndef _DEBUG  // debug version in DiskViewerView.cpp
inline CDiskViewerDoc* CDiskViewerView::GetDocument()
   { return (CDiskViewerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKVIEWERVIEW_H__C4F6F7F5_C80B_4ECC_BF1C_C1018D7DFEB8__INCLUDED_)
