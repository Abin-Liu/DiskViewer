// DiskViewerView.cpp : implementation of the CDiskViewerView class
//

#include "stdafx.h"
#include "DiskViewer.h"
#include "DiskViewerDoc.h"
#include "DiskViewerView.h"
#include "openfiledlg.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerView

IMPLEMENT_DYNCREATE(CDiskViewerView, CView)

BEGIN_MESSAGE_MAP(CDiskViewerView, CView)
	//{{AFX_MSG_MAP(CDiskViewerView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_HEAD_0, OnHead0)
	ON_UPDATE_COMMAND_UI(ID_HEAD_0, OnUpdateHead0)
	ON_COMMAND(ID_HEAD_1, OnHead1)
	ON_UPDATE_COMMAND_UI(ID_HEAD_1, OnUpdateHead1)
	ON_COMMAND(ID_HEAD_2, OnHead2)
	ON_UPDATE_COMMAND_UI(ID_HEAD_2, OnUpdateHead2)
	ON_COMMAND(ID_HEAD_3, OnHead3)
	ON_UPDATE_COMMAND_UI(ID_HEAD_3, OnUpdateHead3)
	ON_COMMAND(ID_COPYIMG, OnCopyimg)
	ON_UPDATE_COMMAND_UI(ID_COPYIMG, OnUpdateCopyimg)
	ON_WM_ERASEBKGND()
	ON_WM_CHAR()
	ON_COMMAND(ID_VIEW_DEFLIST, OnViewDeflist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEFLIST, OnUpdateViewDeflist)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_MESSAGE(WM_DEF_HIGHLIGHT, OnDefectHighlight)
	ON_MESSAGE(WM_DEF_HIDE, OnDefectHide)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerView construction/destruction

CDiskViewerView::CDiskViewerView()
{
	// TODO: add construction code here	
	m_ptStart = CPoint(-1, -1);
	m_ptEnd = CPoint(-1, -1);
	m_bBtnDown = FALSE;
	m_bShowDefList = AfxGetApp()->GetProfileInt(_T(""), _T("Show List"), TRUE);
	m_bHighlight = FALSE;
}

CDiskViewerView::~CDiskViewerView()
{
}

LRESULT CDiskViewerView::OnDefectHighlight(WPARAM wParam, LPARAM lParam)
{
	CRect rectNew;
	if (m_wndDefList.GetHighlightRect(&rectNew))
	{
		m_bHighlight = TRUE;
		
		if (wParam == 1)
		{
			// zoom immediately
			m_bBtnDown = FALSE;
			m_ptStart = m_rectHighlight.TopLeft();
			m_ptEnd = m_rectHighlight.BottomRight();
			GetDocument()->GetDisk()->ZoomIn(m_ptStart, m_ptEnd);

			// recalculate the highlight box
			m_wndDefList.GetHighlightRect(&m_rectHighlight);
			Invalidate();
		}
		else
		{
			// just display a highlight rect
			CRect rect(m_rectHighlight);
			rect.InflateRect(1, 1);
			InvalidateRect(&rect);
			m_rectHighlight = rectNew;
			rectNew.InflateRect(1, 1);
			InvalidateRect(&rectNew);
		}		
	}
	else
	{
		m_bHighlight = FALSE;
	}
	return (LRESULT)0;
}

BOOL CDiskViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerView drawing

void CDiskViewerView::OnDraw(CDC* dc)
{
	CMemDC pDC(dc);
	CDiskViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	const int OFFSETX = 20;
	const int OFFSETY = 30;
	const int GAP = 3;
	int i = 0;
	CDisk* pDisk = GetDocument()->GetDisk();
	ASSERT(pDisk != NULL);

	// set text background color to "transparent"
	int nBkMode = pDC->SetBkMode(TRANSPARENT);

	if (!pDisk->IsDiskReady())
	{
		pDC->TextOut(OFFSETX, OFFSETY, _T("Please select log files to proceed."));
		pDC->SetBkMode(nBkMode);
		return;
	}

	// draw disk surface & defects
	pDisk->DrawDisk(pDC, RGB(128, 128, 128), RGB(0, 0, 0), RGB(255, 255, 212));
	pDisk->DrawDefects(pDC);

	CPen selPen, blackPen, redPen;
	selPen.CreatePen(PS_DOT, 1, RGB(128, 128, 128));
	blackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	redPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	CPen* pOld = pDC->SelectObject(&selPen);

	// draw the selection boundaries
	// a dot lined rectangle to represent the selection area
	if (m_bBtnDown)
	{
		CRect rect = CDisk::RectFromPts(m_ptStart, m_ptEnd, FALSE, FALSE);
		pDC->MoveTo(rect.TopLeft());
		pDC->LineTo(rect.right, rect.top);
		pDC->LineTo(rect.BottomRight());
		pDC->LineTo(rect.left, rect.bottom);
		pDC->LineTo(rect.TopLeft());
	}

	// draw text
	// draw the drive serial No
	TCHAR szSN[65], szDraw[80];
	pDisk->GetSN(szSN, 64);
	if (*szSN == _T('\0'))
		_tcscpy(szSN, _T("--------"));
	_stprintf(szDraw, _T("SN: %s"), szSN);
	pDC->TextOut(OFFSETX, OFFSETY, szDraw);

	// we need the text height
	CSize size = pDC->GetTextExtent(_T("A"));
	size.cx = size.cy; 
	CRect rect(CPoint(OFFSETX, OFFSETY + GAP), size);
	CBrush brushes[4]; // for 4 heads
	pDC->SelectObject(&blackPen);
	CString str;	

	// draw head count
	rect.top += size.cy + GAP;
	rect.bottom = rect.top + size.cy;	
	str.Format(_T("Heads: %X"), pDisk->GetHeadCount());
	pDC->TextOut(OFFSETX, rect.top, str);

	// draw zone count
	rect.top += size.cy + GAP;
	rect.bottom = rect.top + size.cy;	
	str.Format(_T("Zones: %X"), pDisk->GetZoneCount());
	pDC->TextOut(OFFSETX, rect.top, str);

	// draw cylinder count
	rect.top += size.cy + GAP;
	rect.bottom = rect.top + size.cy;	
	str.Format(_T("Cyldrs: %X"), pDisk->GetCylinderCount());
	pDC->TextOut(OFFSETX, rect.top, str);
	
	const BOOL RDY = pDisk->IsDiskReady();
	rect.top += size.cy;
	rect.bottom = rect.top + size.cy;

	// draw legends
	for (i = 0; i < (int)pDisk->GetHeadCount(); i++)
	{
		brushes[i].CreateSolidBrush(pDisk->GetHeadColor(i));
		CBrush* pOldBrush = pDC->SelectObject(&brushes[i]);

		rect.top += size.cy + GAP;
		rect.bottom = rect.top + size.cy;		
		pDC->Rectangle(&rect);

		pDC->SelectObject(pOldBrush);
		brushes[i].DeleteObject();

		CString sHead;
		if (!RDY)
			sHead.Format(_T("Head %02X"), i);
		else
			sHead.Format(_T("Head %02X (%X)"), i, pDisk->GetDefectCount((ULONG)i));
		pDC->TextOut(rect.right + GAP, rect.top, sHead);
	}

	// zoomed mark
	CRect rcTL, rcTR, rcBR, rcBL;
	if (pDisk->IsZoomedIn() && GetZoomedMarkRects(rcTL, rcTR, rcBR, rcBL))
	{
		COLORREF color = pDC->SetTextColor(RGB(0, 0, 255));
		pDC->TextOut(rcTL.left, rcTL.top, _T("Zoomed"));
		pDC->TextOut(rcTR.left, rcTR.top, _T("Zoomed"));
		pDC->TextOut(rcBR.left, rcBR.top, _T("Zoomed"));
		pDC->TextOut(rcBL.left, rcBL.top, _T("Zoomed"));
		pDC->SetTextColor(color);
	}

	// highlight box
	if (m_bHighlight)
	{	
		pDC->SelectObject(&redPen);
		pDC->MoveTo(m_rectHighlight.TopLeft());
		pDC->LineTo(m_rectHighlight.right, m_rectHighlight.top);
		pDC->LineTo(m_rectHighlight.BottomRight());
		pDC->LineTo(m_rectHighlight.left, m_rectHighlight.bottom);
		pDC->LineTo(m_rectHighlight.TopLeft());		
	}
	
	pDC->SetBkMode(nBkMode);
	pDC->SelectObject(pOld);
	selPen.DeleteObject();
	blackPen.DeleteObject();
	redPen.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerView printing

BOOL CDiskViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDiskViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDiskViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerView diagnostics

#ifdef _DEBUG
void CDiskViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CDiskViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDiskViewerDoc* CDiskViewerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDiskViewerDoc)));
	return (CDiskViewerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiskViewerView message handlers
void CDiskViewerView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
	CView::OnLButtonDown(nFlags, point);
	if (!GetDocument()->GetDisk()->IsDiskReady())
		return;

	if (GetDocument()->GetDisk()->IsZoomedIn())
	{
		Reset(TRUE);
		Invalidate();
	}
	else
	{
		if (m_bHighlight && m_rectHighlight.PtInRect(point))
		{
			m_bBtnDown = FALSE;
			m_ptStart = m_rectHighlight.TopLeft();
			m_ptEnd = m_rectHighlight.BottomRight();
			GetDocument()->GetDisk()->ZoomIn(m_ptStart, m_ptEnd);

			// recalculate the highlight box
			m_wndDefList.GetHighlightRect(&m_rectHighlight);
			Invalidate();
		}
		else
		{
			m_bHighlight = FALSE;
			m_ptStart = point;
			m_ptEnd = CPoint(-1, -1);
			m_bBtnDown = TRUE;
		}		
	}
}

void CDiskViewerView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CView::OnMouseMove(nFlags, point);
	if (!GetDocument()->GetDisk()->IsDiskReady())
		return;

	if (nFlags & MK_RBUTTON)
	{
		ShowTooltip(point);
	}

	if (!m_bBtnDown
		|| (nFlags & MK_LBUTTON) != MK_LBUTTON
		|| m_ptStart.x < 0 || m_ptStart.y < 0)
	{
		m_ptEnd = CPoint(-1, -1);
		m_bBtnDown = FALSE;
	}
	else
	{
		CRect rectOld, rectNew, rectMax;
		rectOld = CDisk::RectFromPts(m_ptStart, m_ptEnd, FALSE, FALSE);
		m_ptEnd = point;
		rectNew = CDisk::RectFromPts(m_ptStart, m_ptEnd, FALSE, FALSE);
		rectMax.left = min(rectOld.left, rectNew.left);
		rectMax.top = min(rectOld.top, rectNew.top);
		rectMax.right = max(rectOld.right, rectNew.right);
		rectMax.bottom = max(rectOld.bottom, rectNew.bottom);
		rectMax.InflateRect(3, 3);
		InvalidateRect(&rectMax);
	}	
}

void CDiskViewerView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
	CView::OnLButtonUp(nFlags, point);
	//m_bHighlight = FALSE;
	if (!GetDocument()->GetDisk()->IsDiskReady())
		return;
	
	if (m_bBtnDown && m_ptStart.x >= 0 && m_ptStart.y >= 0
		&& m_ptEnd.x >= 0 && m_ptEnd.y >= 0)
	{
		m_ptEnd = point;
		GetDocument()->GetDisk()->ZoomIn(m_ptStart, m_ptEnd);
		Invalidate();
	}
	else
	{		
		m_ptStart = CPoint(-1, -1);
		m_ptEnd = CPoint(-1, -1);
	}
	m_bBtnDown = FALSE;
}

void CDiskViewerView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	AfxGetMainWnd()->SetWindowText(_T("Disk Viewer v1.0"));
	m_wndTooltip.Create(this);
	m_wndDefList.Create(this, GetDocument()->GetDisk());
	CRect rectThis, rectDlg;
	this->GetWindowRect(&rectThis);
	m_wndDefList.GetWindowRect(&rectDlg);

	int nWidth = rectDlg.Width();
	int nHeight = rectDlg.Height();
	rectDlg.right = rectThis.right + nWidth / 2;
	rectDlg.left = rectDlg.right - nWidth;
	rectDlg.top = rectThis.top + 60;
	rectDlg.bottom = rectDlg.top + nHeight;
	m_wndDefList.MoveWindow(&rectDlg);
	Reset(TRUE);
}

void CDiskViewerView::Reset(BOOL bQuitZoom)
{
	m_bHighlight = FALSE;
	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(0, 10, 0, 10);
	GetDocument()->GetDisk()->SetBoundary(rect.TopLeft(), rect.BottomRight());
	if (bQuitZoom)
	{
		m_ptStart = CPoint(-1, -1);
		m_ptEnd = CPoint(-1, -1);
		m_bBtnDown = FALSE;
	}
}

void CDiskViewerView::OnSize(UINT nType, int cx, int cy) 
{
	// TODO: Add your message handler code here
	CView::OnSize(nType, cx, cy);

	if (!GetDocument()->GetDisk()->IsZoomedIn())
		Reset(FALSE);
}

void CDiskViewerView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CView::OnRButtonDown(nFlags, point);
	//m_bHighlight = FALSE;

	if (!GetDocument()->GetDisk()->IsDiskReady())
		return;

	ShowTooltip(point);
}

void CDiskViewerView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	m_bShowDefList = AfxGetApp()->WriteProfileInt(_T(""), _T("Show List"), m_bShowDefList);
	m_wndTooltip.DestroyWindow();
	m_wndDefList.DestroyWindow();
}

void CDiskViewerView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CView::OnRButtonUp(nFlags, point);
	//m_bHighlight = FALSE;
	if (!GetDocument()->GetDisk()->IsDiskReady())
		return;
	m_wndTooltip.ShowWindow(SW_HIDE);
}

BOOL CDiskViewerView::ShowTooltip(POINT point)
{
	if (m_wndTooltip.GetSafeHwnd() == NULL)
		return FALSE;

	ULONG lZone, lCylinder, lSector;
	if (!GetDocument()->GetDisk()->ScreenToDisk(point, lZone, lCylinder, lSector))
	{
		m_wndTooltip.ShowWindow(SW_HIDE);
		return FALSE;
	}

	m_wndTooltip.ShowTooltip(point, lZone, lCylinder, lSector);
	return TRUE;
}

void CDiskViewerView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	TCHAR szZoneFile[MAX_PATH + 1] = _T("");
	TCHAR szDefectFile[MAX_PATH + 1] = _T("");
	GetDocument()->GetAna()->GetZoneFileName(szZoneFile, MAX_PATH);
	GetDocument()->GetAna()->GetDefectFileName(szDefectFile, MAX_PATH);

	if (*szZoneFile == _T('\0'))
		_tcsncpy(szZoneFile, AfxGetApp()->GetProfileString(_T(""), _T("Zone Table")), MAX_PATH);

	if (*szDefectFile == _T('\0'))
		_tcsncpy(szDefectFile, AfxGetApp()->GetProfileString(_T(""), _T("Defect Log")), MAX_PATH);

	COpenFileDlg dlg(szZoneFile, szDefectFile);
	if (dlg.DoModal() == IDOK)
	{
		// Update the system registry
		AfxGetApp()->WriteProfileString(_T(""), _T("Zone Table"), dlg.GetZoneFile());
		AfxGetApp()->WriteProfileString(_T(""), _T("Defect Log"), dlg.GetDefectFile());
			
		const int RES = GetDocument()->GetAna()->LoadFiles(dlg.GetZoneFile(), dlg.GetDefectFile());
		if (RES == LOAD_OK)
		{
			// files loaded successfully
			m_ptStart = m_ptEnd = CPoint(-1, -1);
			m_bBtnDown = FALSE;

			// initialize some color
			COLORREF colors[4] = { RGB(255, 0, 0), RGB(0, 0, 255), RGB(0, 128, 0), RGB(255, 0, 255) };
			CDisk* pDisk = GetDocument()->GetDisk();
			for (UINT i = 0; i < pDisk->GetHeadCount(); i++)
				pDisk->SetHeadColor(i, colors[i % 4]);

			m_wndDefList.Reset();
			if (m_bShowDefList)
				m_wndDefList.ShowWindow(SW_SHOW);

			Invalidate();
		}
		else if (RES == ZONEFILE_FAIL)
		{
			// failed to parse the zone-table files
			m_wndDefList.ShowWindow(SW_HIDE);
			CString sErr;
			sErr.Format(_T("Failed to parse file:\n\n\"%s\".\n\nUnrecognized file format or contents."), dlg.GetZoneFile());
			MessageBox(sErr, _T("File Failure"), MB_ICONEXCLAMATION);
		}
		else if (RES == DEFECTFILE_FAIL)
		{
			// failed to parse the defect-log file
			m_wndDefList.ShowWindow(SW_HIDE);
			CString sErr;
			sErr.Format(_T("Failed to parse file:\n\n\"%s\".\n\nUnrecognized file format or contents."), dlg.GetDefectFile());
			MessageBox(sErr, _T("File Failure"), MB_ICONEXCLAMATION);
		}
		else
		{
			// should never happen
			MessageBox(_T("Unknown file read failure."), _T("File Failure"), MB_ICONEXCLAMATION);
		}		
	}
}

void CDiskViewerView::OnHead0() 
{
	// TODO: Add your command handler code here
	GetDocument()->GetDisk()->EnableHead(0, !GetDocument()->GetDisk()->IsHeadEnabled(0));
	Invalidate();
	m_wndDefList.Reset();
}

void CDiskViewerView::OnUpdateHead0(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->GetDisk()->GetHeadCount() >= 1);
	pCmdUI->SetCheck(GetDocument()->GetDisk()->IsHeadEnabled(0));
}

void CDiskViewerView::OnHead1() 
{
	// TODO: Add your command handler code here
	GetDocument()->GetDisk()->EnableHead(1, !GetDocument()->GetDisk()->IsHeadEnabled(1));
	Invalidate();
	m_wndDefList.Reset();
}

void CDiskViewerView::OnUpdateHead1(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->GetDisk()->GetHeadCount() >= 2);
	pCmdUI->SetCheck(GetDocument()->GetDisk()->IsHeadEnabled(1));
}

void CDiskViewerView::OnHead2() 
{
	// TODO: Add your command handler code here
	GetDocument()->GetDisk()->EnableHead(2, !GetDocument()->GetDisk()->IsHeadEnabled(2));
	Invalidate();
	m_wndDefList.Reset();
}

void CDiskViewerView::OnUpdateHead2(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->GetDisk()->GetHeadCount() >= 3);
	pCmdUI->SetCheck(GetDocument()->GetDisk()->IsHeadEnabled(2));
}

void CDiskViewerView::OnHead3() 
{
	// TODO: Add your command handler code here
	GetDocument()->GetDisk()->EnableHead(3, !GetDocument()->GetDisk()->IsHeadEnabled(3));
	Invalidate();
	m_wndDefList.Reset();
}

void CDiskViewerView::OnUpdateHead3(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->GetDisk()->GetHeadCount() >= 4);
	pCmdUI->SetCheck(GetDocument()->GetDisk()->IsHeadEnabled(3));
}

void CDiskViewerView::CopyImage(LPRECT lpRect)
{
	// determine the area to capture
	CRect rect;
	if (lpRect != NULL)
		rect = *lpRect;
	else
		GetClientRect(&rect);

	CClientDC dc(this);
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CBitmap bm;
    CSize size(rect.Width(), rect.Height());
    bm.CreateCompatibleBitmap(&dc, size.cx, size.cy);
    CBitmap * oldbm = memDC.SelectObject(&bm);

    memDC.BitBlt(0, 0, size.cx, size.cy, &dc, 0, 0, SRCCOPY);
    GetParent()->OpenClipboard();
    ::EmptyClipboard();
    ::SetClipboardData(CF_BITMAP, bm.m_hObject);
    CloseClipboard();

    memDC.SelectObject(oldbm);
    bm.Detach();  // make sure bitmap not deleted with CBitmap object
}

void CDiskViewerView::OnCopyimg() 
{
	// TODO: Add your command handler code here
	CopyImage(NULL);
}

void CDiskViewerView::OnUpdateCopyimg(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->GetDisk()->IsDiskReady());
}

BOOL CDiskViewerView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	pDC = NULL;
	return FALSE;
	//return CView::OnEraseBkgnd(pDC);
}

BOOL CDiskViewerView::GetZoomedMarkRects(CRect &rcTL, CRect &rcTR, CRect &rcBR, CRect &rcBL)
{
	if (!GetDocument()->GetDisk()->IsZoomedIn())
		return FALSE;

	CClientDC dc(this);
	const CSize SZ = dc.GetTextExtent(_T("Zoomed"));
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.DeflateRect(4, 2);

	rcTL = CRect(rcClient.TopLeft(), SZ);
	rcTR = CRect(CPoint(rcClient.right - SZ.cx, rcClient.top), SZ);
	rcBR = CRect(CPoint(rcClient.right - SZ.cx, rcClient.bottom - SZ.cy), SZ);
	rcBL = CRect(CPoint(rcClient.left, rcClient.bottom - SZ.cy), SZ);
	return TRUE;
}

void CDiskViewerView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CView::OnChar(nChar, nRepCnt, nFlags);
	if (nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_SPACE)
	{
		if (GetDocument()->GetDisk()->IsZoomedIn())
		{
			Reset(TRUE);
			Invalidate();
		}
	}
}

void CDiskViewerView::OnViewDeflist() 
{
	// TODO: Add your command handler code here
	m_bShowDefList = !m_bShowDefList;
	if (m_bShowDefList)
		m_wndDefList.ShowWindow(SW_SHOW);
	else
		m_wndDefList.ShowWindow(SW_HIDE);		
}

void CDiskViewerView::OnUpdateViewDeflist(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->GetDisk()->IsDiskReady());
	pCmdUI->SetCheck(m_bShowDefList);
}

LRESULT CDiskViewerView::OnDefectHide(WPARAM wParam, LPARAM lParam)
{
	m_bShowDefList = FALSE;
	return (LRESULT)0;
}
