// DefectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiskViewer.h"
#include "DefectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFECT_INVALID	0xffffffff

/////////////////////////////////////////////////////////////////////////////
// CDefectDlg dialog


CDefectDlg::CDefectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pView = NULL;
	m_pDisk = NULL;
	m_bLoading = FALSE;
	m_defect.lHead = DEFECT_INVALID;
	m_defect.lCylinder = DEFECT_INVALID;
	m_defect.lSector = DEFECT_INVALID;
	m_nCurSel = -1;
}

void CDefectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefectDlg)
	DDX_Control(pDX, IDC_LIST1, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefectDlg, CDialog)
	//{{AFX_MSG_MAP(CDefectDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ZOOMTO, OnZoomto)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefectDlg message handlers

void CDefectDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (m_pView != NULL)
		m_pView->SendMessage(WM_DEF_HIDE, 0, 0);

	CDialog::OnCancel();
}

BOOL CDefectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_wndList.SetColumnHeader(_T("Head, 45; Cylinder, 70; Sector, 60"));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDefectDlg::Create(CView* pView, const CDisk* pDisk) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pDisk = pDisk;
	m_pView = pView;
	return CDialog::Create(IDD, pView);
}

void CDefectDlg::Reset()
{
	GetDlgItem(IDC_ZOOMTO)->EnableWindow(FALSE);
	if (m_pDisk == NULL)
		return;

	m_wndList.ShowWindow(SW_HIDE);
	m_bLoading = TRUE;

	m_wndList.DeleteAllItems();
	m_defect.lHead = DEFECT_INVALID;
	m_defect.lCylinder = DEFECT_INVALID;
	m_defect.lSector = DEFECT_INVALID;

	for (ULONG i = 0; i < m_pDisk->GetHeadCount(); i++)
	{
		if (!m_pDisk->IsHeadEnabled(i))
			continue;

		int n = 0;
		CPoint defect;
		CString sHead, sCylinder, sSector;
		while (m_pDisk->GetDefect(i, n++, defect))
		{
			sHead.Format(_T("%02X"), i);
			sCylinder.Format(_T("%06X"), defect.x);
			sSector.Format(_T("%04X"), defect.y);
			const int IDX = m_wndList.InsertItemEx(INT_MAX, sHead, sCylinder, sSector);
			if (IDX >= 0)
				m_wndList.SetItemBkColor(IDX, -1, m_pDisk->GetHeadColor(i));
		}
	}

	m_wndList.SetItemTextColor(-1, -1, RGB(255, 255, 255));
	m_bLoading = FALSE;
	m_wndList.ShowWindow(SW_SHOW);
}

void CDefectDlg::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_ZOOMTO)->EnableWindow(FALSE);

	if (m_pView != NULL
		&& (pNMListView->uNewState & LVIS_SELECTED)
		&& !m_bLoading
		&& GetFocus() == &m_wndList)
	{
		m_nCurSel = pNMListView->iItem;
		if (m_nCurSel >= 0)
		{
			m_defect.lHead = _tcstoul(m_wndList.GetItemText(m_nCurSel, 0), NULL, 16);
			m_defect.lCylinder = _tcstoul(m_wndList.GetItemText(m_nCurSel, 1), NULL, 16);
			m_defect.lSector = _tcstoul(m_wndList.GetItemText(m_nCurSel, 2), NULL, 16);
			m_pView->SendMessage(WM_DEF_HIGHLIGHT, 0, 0);
		}
		GetDlgItem(IDC_ZOOMTO)->EnableWindow(m_nCurSel >= 0);
	}
	*pResult = 0;
}

const DEFECT* CDefectDlg::GetDefect() const
{
	if (m_defect.lHead == DEFECT_INVALID
		|| m_defect.lCylinder == DEFECT_INVALID
		|| m_defect.lSector == DEFECT_INVALID)
	{
		return NULL;
	}
	else
	{
		return &m_defect;
	}
}

void CDefectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

BOOL CDefectDlg::GetHighlightRect(CRect* pRect) const
{
	if (m_nCurSel < 0 || pRect == NULL)
		return FALSE;

	CPoint pt;
	if (!m_pDisk->DiskToScreen(pt, m_defect.lCylinder, m_defect.lSector))
		return FALSE;

	*pRect = CRect(pt, CSize(1, 1));
	pRect->InflateRect(5, 5);
	return TRUE;
}

void CDefectDlg::OnZoomto() 
{
	// TODO: Add your control notification handler code here
	if (m_pView != NULL)
		m_pView->SendMessage(WM_DEF_HIGHLIGHT, 1, 0);
}

void CDefectDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_pView != NULL && m_nCurSel >= 0)
		m_pView->SendMessage(WM_DEF_HIGHLIGHT, 1, 0);
	*pResult = 0;
}
