#if !defined(AFX_TOOLTIPWND_H__7755C3B7_6FAE_4F69_87F9_128955B3764F__INCLUDED_)
#define AFX_TOOLTIPWND_H__7755C3B7_6FAE_4F69_87F9_128955B3764F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TooltipWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTooltipWnd dialog

class CTooltipWnd : public CDialog
{
// Construction
public:
	CSize GetSize() const;
	void ShowTooltip(POINT ptLocation, long lZone, long lCylinder, long lSector);
	CTooltipWnd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTooltipWnd)
	enum { IDD = IDD_TOOLTIPDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTooltipWnd)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTooltipWnd)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CString m_sText;
	CBrush m_brush;
	CSize m_sizeText;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPWND_H__7755C3B7_6FAE_4F69_87F9_128955B3764F__INCLUDED_)
