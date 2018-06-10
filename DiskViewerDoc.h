// DiskViewerDoc.h : interface of the CDiskViewerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKVIEWERDOC_H__79993E48_6423_4837_87E0_8C5E46CC71F6__INCLUDED_)
#define AFX_DISKVIEWERDOC_H__79993E48_6423_4837_87E0_8C5E46CC71F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalyseText.h"

class CDiskViewerDoc : public CDocument
{
protected: // create from serialization only
	CDiskViewerDoc();
	DECLARE_DYNCREATE(CDiskViewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskViewerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	CAnalyseText* GetAna();
	CDisk* GetDisk();
	virtual ~CDiskViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDiskViewerDoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CAnalyseText m_ana;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKVIEWERDOC_H__79993E48_6423_4837_87E0_8C5E46CC71F6__INCLUDED_)
