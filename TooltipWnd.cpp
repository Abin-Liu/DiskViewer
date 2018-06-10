// TooltipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DiskViewer.h"
#include "TooltipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTooltipWnd dialog


CTooltipWnd::CTooltipWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CTooltipWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTooltipWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
	m_brush.CreateSolidBrush(::GetSysColor(COLOR_INFOBK));
	m_sizeText = CSize(0, 0);
}


void CTooltipWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTooltipWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTooltipWnd, CDialog)
	//{{AFX_MSG_MAP(CTooltipWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTooltipWnd message handlers

void CTooltipWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(&rect);
	dc.SelectObject(&m_brush);
	dc.Rectangle(&rect);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(GetFont());
	dc.TextOut(4, 2, m_sText);
	// Do not call CDialog::OnPaint() for painting messages
}

void CTooltipWnd::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_brush.DeleteObject();
}

void CTooltipWnd::ShowTooltip(POINT ptLocation, long lZone, long lCylinder, long lSector)
{
	if (lZone < 0 && lCylinder < 0 && lSector < 0)
	{
		ShowWindow(SW_HIDE);
		return;
	}

	m_sText.Format(_T("Zone %04X  Cylinder %04X  Sector %04X"), lZone, lCylinder, lSector);
	CRect rect(ptLocation, GetSize());
	rect.OffsetRect(0, -rect.Height());
	MoveWindow(&rect);
	ShowWindow(SW_SHOW);

	GetClientRect(&rect);
	rect.DeflateRect(1, 1);
	RedrawWindow(&rect);
}

CSize CTooltipWnd::GetSize() const
{
	CSize size = m_sizeText;
	size.cx += 4;
	size.cy += 4;
	return size;
}

BOOL CTooltipWnd::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CTooltipWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CClientDC dc(this);
	dc.SelectObject(GetFont());
	m_sizeText = dc.GetTextExtent(_T("Zone 00DD  Cylinder 0DDD  Sector 0DDD"));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTooltipWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
	CDialog::OnLButtonDown(nFlags, point);
	ShowWindow(SW_HIDE);
}

void CTooltipWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
	CDialog::OnRButtonDown(nFlags, point);
	ShowWindow(SW_HIDE);
}

void CTooltipWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
	CDialog::OnRButtonUp(nFlags, point);
	ShowWindow(SW_HIDE);
}

void CTooltipWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);
	//CDialog::OnMouseMove(nFlags, point);
}
