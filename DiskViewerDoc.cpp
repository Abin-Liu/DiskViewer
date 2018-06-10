// DiskViewerDoc.cpp : implementation of the CDiskViewerDoc class
//

#include "stdafx.h"
#include "DiskViewer.h"

#include "DiskViewerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerDoc

IMPLEMENT_DYNCREATE(CDiskViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CDiskViewerDoc, CDocument)
	//{{AFX_MSG_MAP(CDiskViewerDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerDoc construction/destruction

CDiskViewerDoc::CDiskViewerDoc()
{
	// TODO: add one-time construction code here
	
}

CDiskViewerDoc::~CDiskViewerDoc()
{
}

BOOL CDiskViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDiskViewerDoc serialization

void CDiskViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerDoc diagnostics

#ifdef _DEBUG
void CDiskViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDiskViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerDoc commands

CDisk* CDiskViewerDoc::GetDisk()
{
	return m_ana.GetDisk();
}

CAnalyseText* CDiskViewerDoc::GetAna()
{
	return &m_ana;
}

void CDiskViewerDoc::OnFileSave() 
{
	// TODO: Add your command handler code here
	
}

void CDiskViewerDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDisk()->IsDiskReady());
}

void CDiskViewerDoc::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDisk()->IsDiskReady());
}

void CDiskViewerDoc::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDisk()->IsDiskReady());
}
