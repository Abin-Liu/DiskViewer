// OpenFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiskViewer.h"
#include "OpenFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_FILE_NOTIFY	WM_APP + 1000

/////////////////////////////////////////////////////////////////////////////
// COpenFileDlg dialog


COpenFileDlg::COpenFileDlg(LPCTSTR lpZoneFile, LPCTSTR lpDefectFile, CWnd* pParent /*=NULL*/)
	: CDialog(COpenFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenFileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_wndZoneFile.SetPathName(lpZoneFile);
	m_wndDefectFile.SetPathName(lpDefectFile);
}


void COpenFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenFileDlg)
	DDX_Control(pDX, IDC_ZONEFILE, m_wndZoneFile);
	DDX_Control(pDX, IDC_DEFECTFILE, m_wndDefectFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenFileDlg, CDialog)
	//{{AFX_MSG_MAP(COpenFileDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FILE_NOTIFY, OnFileNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenFileDlg message handlers

LRESULT COpenFileDlg::OnFileNotify(WPARAM wParam, LPARAM lParam)
{
	wParam = 0;
	lParam = 0;
	GetDlgItem(IDOK)->EnableWindow(!m_wndZoneFile.GetPathName().IsEmpty() && !m_wndDefectFile.GetPathName().IsEmpty());
	return 0;
}

BOOL COpenFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_wndZoneFile.SetNotifyMessageID(WM_FILE_NOTIFY);
	m_wndDefectFile.SetNotifyMessageID(WM_FILE_NOTIFY);
	m_wndZoneFile.SetFileFlags(m_wndZoneFile.GetFileFlags() & ~OFN_ALLOWMULTISELECT);
	m_wndDefectFile.SetFileFlags(m_wndDefectFile.GetFileFlags() & ~OFN_ALLOWMULTISELECT);
	GetDlgItem(IDOK)->EnableWindow(!m_wndZoneFile.GetPathName().IsEmpty() && !m_wndDefectFile.GetPathName().IsEmpty());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString COpenFileDlg::GetZoneFile() const
{
	return m_wndZoneFile.GetPathName();
}

CString COpenFileDlg::GetDefectFile() const
{
	return m_wndDefectFile.GetPathName();
}
