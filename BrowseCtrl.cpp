///////////////////////////////////////////////////////////////////////////
//				BrowseCtrl.cpp
//
// CBrowseCtrl is a CButton derived control used to let the user browse for
// files or folders on their systems.
//
// CBrowseCtrl integrates an edit-box and a browse-button together so the
// develops only need to maintain one control in their projects instead of two.

// Moreover, CBrowseCtrl draws its own "build-in" images on the browse-button
// without requiring the developers to include any extra icon, bitmap or other
// image resources.
//
// Written by Abin (abinn32@yahoo.com)
//
// History:
// 
// Jan 08, 2004 - Initial public release.
// Jan 09, 2004 - 1, Improved drawing functions to properly handle the case in
//                   which the control client area is too small to be drawn.
//                   In debug mode it will provide a message box to inform the
//                   developer about that issue.
//                2, Improved the edit box so it acts better upon receiving and
//                   losing the input focus. 
// Jan 10, 2004 - 1, Fixed a problem where the edit bix is not properly redrawn
//                   in some mouse events. Thanks to JOHN11.
//                2, Added method "GetSelectedCount" which returns the number of
//                   items the user has selected in the most recent file/folder
//                   dialog that was terminated by IDOK.
//                3, Improved the mouse/focus events monitoring functions.
//                4, Fixed a drawing glitch when the user clicks on the edges of
//                   the edit box.
//                5, Changed the drawing rects calculating methods for performance
//                   improvement.
//
/////////////////////////////////////////////////////////////////////////// 

#include "stdafx.h"
#include "BrowseCtrl.h"
#include <afxdlgs.h>
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICON_WIDTH	16
#define ICON_HEIGHT	16
#define BTN_EDGE	2
#define WIDTH_SPACE	(BTN_EDGE * 2 + 7)

#define EDITCTRL_ID				0
#define TOOLTIPCTRL_ID			1
#define ID_TMR_MOUSEMONITOR		0
#define ID_TMR_FOCUSMONITOR		1

// color defines
#define WHITE	RGB(255, 255, 255)
#define BLACK	RGB(0, 0, 0)
#define DARK	RGB(128, 128, 128)
#define GRAY	RGB(192, 192, 192)
#define YELLOW	RGB(255, 255, 0)
#define BLUE	RGB(0, 0, 255)
#define OLIVE	RGB(128, 128, 0)
#define CYAN	RGB(0, 255, 255)
#define NAVY	RGB(0, 0, 128)

// "CLineDraw" class is used for helping on drawing lines, the default behavior of
// CDC::LineTo on not drawing the last pixel could be extremely inconvenient in
// certain cases.
class CLineDraw
{
public:

	// Constructor
	CLineDraw(CDC* pDC, CPoint* pPoint, const CRect* pRect)
	{
		ASSERT(pDC && pPoint && pRect);
		m_pDC = pDC;
		m_pPt = pPoint;
		m_pRect = pRect;
	}

	// Set the point the top-lect corner of the rect, offset the point,
	// then move dc to that point
	void InitOffsetMoveTo(long x, long y)
	{
		*m_pPt = m_pRect->TopLeft();
		m_pPt->Offset(x, y);
		m_pDC->MoveTo(*m_pPt);
	}

	// Offset the point and move dc to the point
	void OffsetMoveTo(long x, long y)
	{
		m_pPt->Offset(x, y);
		m_pDC->MoveTo(*m_pPt);
	}

	// Offset the point and draw a line to the point, then set pixel at the point
	// using the color of current pen stored in the dc
	void OffsetLineTo(long x, long y)
	{
		m_pPt->Offset(x, y);
		m_pDC->LineTo(*m_pPt);
		m_pDC->GetCurrentPen()->GetLogPen(&m_lgp);
		m_pDC->SetPixel(*m_pPt, m_lgp.lopnColor);
	}

private:
	CDC* m_pDC;
	CPoint* m_pPt;
	LOGPEN m_lgp;
	const CRect* m_pRect;
};

/////////////////////////////////////////////////////////////////////////////
// CBrowseCtrl Implementations
/////////////////////////////////////////////////////////////////////////////

CBrowseCtrl::CBrowseCtrl()
{
	m_dwStyle = BC_BTN_ICON | BC_ICO_ARROWFOLDER;
	m_nSelCount = 0;
	m_bEditFocused = FALSE;
	m_nPathNamesLen = 0;
	m_bDlgPopped = FALSE;
	m_nNotifyMsg = 0;
	m_bOwnCreate = FALSE;
	m_bBtnPushed = FALSE;
	m_bMouseIn = FALSE;
	m_sTootipText = _T("Browse");
	m_sDlgTitle = _T("Please select a folder:");
	m_bReadOnlyRef = FALSE;
	m_bOpenFileDialog = TRUE;
	m_dwFileFlags = OFN_ALLOWMULTISELECT | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT | OFN_ENABLESIZING;
	m_nFolderFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	m_sFilter = _T("All Files (*.*)|*.*||");
	m_lpszPathNames = NULL;
}

CBrowseCtrl::~CBrowseCtrl()
{
	if (m_lpszPathNames != NULL)
		delete [] m_lpszPathNames;
}

BEGIN_MESSAGE_MAP(CBrowseCtrl, CButton)
	//{{AFX_MSG_MAP(CBrowseCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBrowseCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CButton::PreSubclassWindow();
	if (!m_bOwnCreate)
		CreateCtrl(); // only create the edit/tooltip control once!
	
	if (m_wndEdit.GetSafeHwnd() != NULL)
		m_wndEdit.SetWindowText(m_lpszPathNames);
}

void CBrowseCtrl::SetButtonStyle(DWORD dwStyles)
{
	m_dwStyle = dwStyles;
	if (GetSafeHwnd() != NULL)
	{
		RecalculateRects();
		CreateEdit();
		RedrawWindow();
	}		
}

DWORD CBrowseCtrl::GetButtonStyle() const
{
	return m_dwStyle;
}

BOOL CBrowseCtrl::CreateCtrl()
{
	if (GetSafeHwnd() == NULL)
		return FALSE;

	RecalculateRects();
	if (m_wndEdit.GetSafeHwnd() != NULL)
		return TRUE;

	// Move window text from actual window to the fake button
	if (m_sButtonText.IsEmpty())
		CButton::GetWindowText(m_sButtonText);
	CButton::SetWindowText(_T(""));

	BOOL bRes = CreateEdit();
	if (bRes)
	{
		// Create tooltip control
	    m_wndTooltip.Create(this);
		m_wndTooltip.AddTool(this, m_sTootipText, m_rcButton, TOOLTIPCTRL_ID);
		SetTimer(ID_TMR_FOCUSMONITOR, 1, NULL);
	}
	return bRes;
}

void CBrowseCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawEditFrame(&dc);
	DrawButtonFrame(&dc);

	// Draw text or different images on the fake button
	CRect rect = m_rcContent;
	if (m_bBtnPushed)
		rect.OffsetRect(1, 1);

	if (m_dwStyle & BC_BTN_ICON)
	{
		if (m_dwStyle & BC_ICO_ARROWFOLDER)
		{
			DrawButtonArrowFolder(&dc, rect);
		}
		else if (m_dwStyle & BC_ICO_FOLDER)
		{
			DrawButtonFolder(&dc, rect);
		}
		else if (m_dwStyle & BC_ICO_EXPLORER)
		{
			DrawButtonExplorer(&dc, rect);
		}
		else 
		{
			// Do we have any other stuff to draw?
		}
	}
	else
	{
		DrawButtonText(&dc, rect);
	}
}

void CBrowseCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	KillTimer(ID_TMR_FOCUSMONITOR);
	if (!m_bEditFocused && pOldWnd != &m_wndEdit)
	{
		CButton::OnSetFocus(pOldWnd);
		m_wndEdit.SetFocus();
		m_wndEdit.SetSel(0, -1);
		m_bEditFocused = TRUE;
	}
	SetTimer(ID_TMR_FOCUSMONITOR, 1, NULL);
}

void CBrowseCtrl::OnSetEditFocus()
{
	m_wndEdit.SetSel(0, -1);
}

void CBrowseCtrl::OnKillEditFocus()
{
	m_wndEdit.SetSel(-1, -1);
}

int CBrowseCtrl::DoModal()
{
	// Popup the file/folder dialog
	if (GetSafeHwnd() == NULL)
	{
		ASSERT(FALSE);
		return IDCANCEL;
	}
	
	if (m_bDlgPopped)
		return IDCANCEL;

	m_bDlgPopped = TRUE;
	int nRes = (m_dwStyle & BC_CTL_FOLDERSONLY) ? FolderDoModal() : FileDoModal();
	m_bDlgPopped = FALSE;

	if (m_nNotifyMsg > 0)
		GetParent()->SendMessage(m_nNotifyMsg, (WPARAM)nRes, nRes == IDOK ? (LPARAM)m_nSelCount : (LPARAM)0);
	return nRes;
}

int CBrowseCtrl::GetSelectedCount() const
{
	return m_nSelCount;
}

void CBrowseCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//DoModal();
	CButton::OnLButtonDown(nFlags, point);
	if (m_rcButton.PtInRect(point))
	{
		OnKillEditFocus();
		SetCapture();
		m_bBtnPushed = TRUE;
	}
	RedrawWindow();
}

void CBrowseCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonUp(nFlags, point);
	BOOL bRedrew = FALSE;
	if (GetCapture() == this)	
	{
		if (m_rcButton.PtInRect(point))
		{
			ReleaseCapture();
			m_bBtnPushed = FALSE;
			RedrawWindow();
			bRedrew = TRUE;
			DoModal();
		}
	}

	if (!bRedrew)
		RedrawWindow();
}

void CBrowseCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonDblClk(nFlags, point);
	RedrawWindow();
}

void CBrowseCtrl::DrawButtonFrame(CDC *pDC) const
{
	CRect rect = m_rcButton;
	CBrush brush;
	brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(&rect, &brush);
	brush.DeleteObject();

	if (m_dwStyle & BC_BTN_FLAT)
	{
		if (m_bBtnPushed)
		{
			pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
		}
		else
		{
			if (m_bMouseIn)
				pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DSHADOW));
		}
	}
	else
	{
		if (!m_bBtnPushed)
		{
			pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DDKSHADOW));
			rect.DeflateRect(1, 1);
			pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_3DSHADOW));
		}
		else
		{
			pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DDKSHADOW));
			rect.DeflateRect(1, 1);
			pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
		}
	}	
}

BOOL CBrowseCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_wndTooltip.RelayEvent(pMsg);

	// Popup the file/folder dialog when user hits "Enter" key when the
	// control window is active
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		DoModal();
		return TRUE;
	}
	return CButton::PreTranslateMessage(pMsg);
}

void CBrowseCtrl::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == ID_TMR_MOUSEMONITOR)
	{
		if (m_bMouseIn)
		{
			POINT pt;
			::GetCursorPos(&pt);
			CRect rect = m_rcButton;
			ClientToScreen(&rect);
			if (!rect.PtInRect(pt))
			{
				KillTimer(ID_TMR_MOUSEMONITOR);
				m_bMouseIn = FALSE;
				OnMouseLeave();
			}
		}
	}
	else if (nIDEvent == ID_TMR_FOCUSMONITOR)
	{
		if (GetFocus() != &m_wndEdit)
		{
			if (m_bEditFocused)
			{
				m_bEditFocused = FALSE;
				OnKillEditFocus();
			}
		}
		else
		{
			if (!m_bEditFocused)
			{
				m_bEditFocused = TRUE;
				OnSetEditFocus();
			}
		}
	}
			
	CButton::OnTimer(nIDEvent);
}

void CBrowseCtrl::OnDestroy() 
{
	KillTimer(ID_TMR_MOUSEMONITOR);
	KillTimer(ID_TMR_FOCUSMONITOR);
	m_wndEdit.DestroyWindow();
	m_wndTooltip.DestroyWindow();
	CButton::OnDestroy();
}

void CBrowseCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
	CButton::OnMouseMove(nFlags, point);
	if (!m_bMouseIn)
	{
		if (m_rcButton.PtInRect(point))
		{
			m_bMouseIn = TRUE;
			SetTimer(ID_TMR_MOUSEMONITOR, 1, NULL);
			OnMouseEnter();
		}
	}
}

void CBrowseCtrl::OnMouseEnter()
{
	if (m_dwStyle & BC_BTN_FLAT)
		RedrawWindow(&m_rcButton);
}

void CBrowseCtrl::OnMouseLeave()
{
	if (GetCapture() == this)
		ReleaseCapture();

	BOOL bNeedRedraw = (m_dwStyle & BC_BTN_FLAT) != 0;

	if (m_bBtnPushed)
	{
		m_bBtnPushed = FALSE;
		bNeedRedraw = TRUE;		
	}

	if (bNeedRedraw)
		RedrawWindow(&m_rcButton);
}

void CBrowseCtrl::SetTooltipText(LPCTSTR lpText)
{
	m_sTootipText = lpText;
	if (GetSafeHwnd() != NULL)
		m_wndTooltip.UpdateTipText(lpText, this, TOOLTIPCTRL_ID);
}

void CBrowseCtrl::SetWindowText(LPCTSTR lpszString)
{
	SetButtonText(lpszString);
}

int CBrowseCtrl::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	if (lpszStringBuf == NULL)
		return 0;
	_tcsncpy(lpszStringBuf, m_sButtonText, nMaxCount);
	return _tcslen(lpszStringBuf);
}

void CBrowseCtrl::GetWindowText(CString &rString) const
{
	rString = GetButtonText();
}

void CBrowseCtrl::DrawEditFrame(CDC *pDC) const
{
	// Draw a 3D frame around the edit box
	CRect rect = m_rcEdit;
	pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
	rect.DeflateRect(1, 1);
	pDC->Draw3dRect(rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_BTNFACE));
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, ::GetSysColor((m_dwStyle & BC_CTL_ALLOWEDIT) && (GetStyle() & WS_DISABLED) == 0 ? COLOR_WINDOW : COLOR_BTNFACE));
	CPen* pPen = pDC->SelectObject(&pen);
	rect.DeflateRect(1, 1);
	pDC->MoveTo(rect.TopLeft());
	pDC->LineTo(rect.right, rect.top);
	pDC->SelectObject(pPen);
	pen.DeleteObject();
}

BOOL CBrowseCtrl::CreateEdit()
{
	if (GetSafeHwnd() == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString str;
	if (m_wndEdit.GetSafeHwnd() != NULL)
	{
		m_wndEdit.GetWindowText(str);
		m_wndEdit.DestroyWindow();
	}

	CRect rect = m_rcEdit;
	rect.DeflateRect(2, 3, 2, 2);
	DWORD dwStyle = ES_AUTOHSCROLL | ES_NOHIDESEL | ES_NOHIDESEL | WS_CHILD | WS_VISIBLE;
	if ((m_dwStyle & BC_CTL_ALLOWEDIT) == 0)
		dwStyle |= ES_READONLY;

	if (GetStyle() & WS_DISABLED)
		dwStyle |= WS_DISABLED;

	BOOL bRes = m_wndEdit.Create(dwStyle, rect, this, EDITCTRL_ID);
	ASSERT(bRes);

	if (bRes)
	{
		m_wndEdit.SetMargins(1, 1);
		m_wndEdit.SetFont(GetFont());
		m_wndEdit.SetWindowText(str);
	}
	return bRes;
}

void CBrowseCtrl::SetNotifyMessageID(UINT nNotifyMsgID)
{
	m_nNotifyMsg = nNotifyMsgID;
}

UINT CBrowseCtrl::GetNotifyMessageID() const
{
	return m_nNotifyMsg;
}

BOOL CBrowseCtrl::EnableWindow(BOOL bEnable)
{
	if (GetSafeHwnd() == NULL)
		return FALSE;

	m_wndEdit.EnableWindow(bEnable);
	BOOL bRes = CButton::EnableWindow(bEnable);
	RedrawWindow();
	return bRes;
}

void CBrowseCtrl::SetOpenSave(BOOL bOpen)
{
	m_bOpenFileDialog = bOpen;
}

BOOL CBrowseCtrl::GetOpenSave() const
{
	return m_bOpenFileDialog;
}

void CBrowseCtrl::SetDefExt(LPCTSTR lpDefExt)
{
	m_sDefExt = lpDefExt;
}

CString CBrowseCtrl::GetDefExt() const
{
 	return m_sDefExt;
}

void CBrowseCtrl::SetFileFlags(DWORD dwFlags)
{
	m_dwFileFlags = dwFlags;
}

DWORD CBrowseCtrl::GetFileFlags() const
{
	return m_dwFileFlags;
}

void CBrowseCtrl::SetFilter(LPCTSTR lpFilter)
{
	m_sFilter = lpFilter;
}

CString CBrowseCtrl::GetFilter() const
{
	return m_sFilter;
}

CString CBrowseCtrl::GetTooltipText() const
{	
	return m_sTootipText;
}

CString CBrowseCtrl::GetPathName() const
{
	return CString(m_lpszPathNames);
//	if (m_wndEdit.GetSafeHwnd() == NULL)
//		return _T("");

//	CString str;
//	m_wndEdit.GetWindowText(str);
//	return str;
}

CString CBrowseCtrl::GetFileName() const
{
	if (m_dwStyle & BC_CTL_FOLDERSONLY)
		return _T("");

	CString str = GetPathName();
	if (str.IsEmpty())
		return str;

	// find the first "\\" and extract the right part
	return str.Right(str.GetLength() - str.ReverseFind(_T('\\')) - 1);
}

CString CBrowseCtrl::GetFileExt() const
{
	CString str = GetFileName();
	if (str.IsEmpty())
		return str;

	// find the first "." and extract the right part
	return str.Right(str.GetLength() - str.ReverseFind(_T('.')) - 1);
}

CString CBrowseCtrl::GetFileTitle() const
{
	CString str = GetFileName();
	if (str.IsEmpty())
		return str;

	// find the first "." and extract the left part
	return str.Left(str.Find(_T('.')));
}

POSITION CBrowseCtrl::GetStartPosition() const
{
	return (POSITION)(LPCTSTR)m_lpszPathNames;
}

CString CBrowseCtrl::GetNextPathName(POSITION &pos) const
{
	// Make sure pos is a valid position
	if (m_lpszPathNames == NULL
		|| m_nPathNamesLen == 0
		|| m_nSelCount == 0
		|| (DWORD)pos < (DWORD)m_lpszPathNames
		|| (DWORD)pos >= (DWORD)m_lpszPathNames + (DWORD)m_nPathNamesLen)
	{
		pos = NULL;
		return _T("");
	}

	LPCTSTR p = (LPCTSTR)pos;
	const int LEN = _tcslen(p);
	if (LEN == 0)
	{
		pos = NULL;
		return _T("");
	}
	else
	{
		pos = (POSITION)(&p[LEN + 1]); // Advance the pointer to next file name
		return p;
	}
}

BOOL CBrowseCtrl::GetReadOnlyPref() const
{
	return m_bReadOnlyRef;
}

BOOL CBrowseCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, UINT nNotifyMessageID) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pParentWnd == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_bOwnCreate = TRUE;
	m_nNotifyMsg = nNotifyMessageID;
	return CButton::Create(_T(""), WS_CHILD|WS_VISIBLE|WS_TABSTOP| BS_PUSHBUTTON, rect, pParentWnd, nID)
		&& CreateCtrl();
}

void CBrowseCtrl::SetPathName(LPCTSTR lpPathName)
{
	if (m_lpszPathNames != NULL)
	{
		delete [] m_lpszPathNames;
		m_lpszPathNames = NULL;
	}

	if (lpPathName != NULL)
		m_lpszPathNames = _tcsdup(lpPathName);

	if (m_wndEdit.GetSafeHwnd() != NULL)
		m_wndEdit.SetWindowText(lpPathName);
}

int CBrowseCtrl::BrowseCallbackProc(HWND hwnd, UINT nMsg, LPARAM lParam, LPARAM lpData)
{	
	lParam = 0; // Appeases VC6 warning level 4.

	if (nMsg == BFFM_INITIALIZED)
	{
		CBrowseCtrl* pCtrl = (CBrowseCtrl*)(lpData);
		if (pCtrl != NULL)
		{
			CString sPathName =  pCtrl->GetPathName();
			if (!sPathName.IsEmpty())
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)sPathName);
		}
	}
	return 0;	
}

void CBrowseCtrl::SetFolderFlags(UINT nFlags)
{
	m_nFolderFlags = nFlags;
}

UINT CBrowseCtrl::GetFolderFlags() const
{
	return m_nFolderFlags;
}

int CBrowseCtrl::FolderDoModal()
{
	// Popup the folder dialog
	if (GetSafeHwnd() == NULL)
	{
		ASSERT(FALSE);
		return IDCANCEL;
	}

	TCHAR szPathName[MAX_PATH + 1] = _T("");
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(BROWSEINFO)); 
	bi.hwndOwner = GetParent()->GetSafeHwnd();
	bi.pszDisplayName = szPathName;
	bi.lpszTitle = m_sDlgTitle;
	bi.ulFlags = m_nFolderFlags;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)this;
	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&bi);
	if (pItemIDList == NULL)
		return IDCANCEL;
	
	m_nSelCount = 1;
	if (m_lpszPathNames != NULL)
	{
		delete [] m_lpszPathNames;
		m_lpszPathNames = NULL;
	}
	m_lpszPathNames = new TCHAR[MAX_PATH + 2];
	memset(m_lpszPathNames, 0, (MAX_PATH + 2) * sizeof(TCHAR));
	::SHGetPathFromIDList(pItemIDList, m_lpszPathNames);
		
	IMalloc* pMalloc = NULL;
	if(SUCCEEDED(::SHGetMalloc(&pMalloc)))
	{
		pMalloc->Free(pItemIDList);
		pMalloc->Release();
	}
		
	m_nPathNamesLen = _tcslen(m_lpszPathNames);
	m_bReadOnlyRef = FALSE;
	m_wndEdit.SetWindowText(m_lpszPathNames);
	m_wndEdit.SetSel(0, -1);
	return IDOK;
}	

int CBrowseCtrl::FileDoModal()
{
	if (GetSafeHwnd() == NULL)
	{
		ASSERT(FALSE);
		return IDCANCEL;
	}

	CString sDefPathName = GetPathName();
	CFileDialog dlg(m_bOpenFileDialog, m_sDefExt, sDefPathName, m_dwFileFlags, m_sFilter, GetParent());
	if (dlg.DoModal() != IDOK)
		return IDCANCEL;	

	if (m_lpszPathNames != NULL)
	{
		delete [] m_lpszPathNames;
		m_lpszPathNames = NULL;
	}

	POSITION pos = dlg.GetStartPosition();

	// determine the string length
	m_nSelCount = 0;
	m_nPathNamesLen = 0;
	while (pos != NULL)
	{
		m_nSelCount++;
		m_nPathNamesLen += dlg.GetNextPathName(pos).GetLength() + 1;
	}
	
	m_nPathNamesLen++;
	m_lpszPathNames = new TCHAR[m_nPathNamesLen];

	// copy the string
	LPTSTR lpDes = m_lpszPathNames;
	pos = dlg.GetStartPosition();
	while (pos != NULL)
	{
		CString str = dlg.GetNextPathName(pos);
		_tcscpy(lpDes, str);			
		lpDes = &lpDes[str.GetLength() + 1];
	}
	m_lpszPathNames[m_nPathNamesLen - 1] = _T('\0');			

	m_bReadOnlyRef = dlg.GetReadOnlyPref();
	m_wndEdit.SetWindowText(m_lpszPathNames);
	m_wndEdit.SetSel(0, -1);
	return IDOK;
}

void CBrowseCtrl::SetFolderDialogTitle(LPCTSTR lpTitle)
{
	m_sDlgTitle = lpTitle;
}

CString CBrowseCtrl::GetFolderDialogTitle() const
{
	return m_sDlgTitle;
}

void CBrowseCtrl::SetButtonText(LPCTSTR lpText)
{
	m_sButtonText = lpText;
	if ((m_dwStyle & BC_BTN_ICON) == 0 && GetSafeHwnd() != NULL)
	{
		RecalculateRects();
		CreateEdit();
		RedrawWindow();
	}
}

CString CBrowseCtrl::GetButtonText() const
{
	return m_sButtonText;
}

void CBrowseCtrl::DrawButtonText(CDC *pDC, const CRect& rect) const
{	
	CFont* pFont = pDC->SelectObject(GetFont());

	// Adjust the text to fit the rect
	CString sText = m_sButtonText;
	CSize size = pDC->GetTextExtent(sText);	
	CheckDrawingArea(rect, &size);

	if (size.cy > m_rcContent.Height())
	{
		// not enough height, don't draw anything
		pDC->SelectObject(pFont);
		return;
	}

	// not enough width, but that could be OK
	while (!sText.IsEmpty() && size.cx > rect.Width())
	{
		// Shrink the string to fit in the rect 
		sText.Delete(sText.GetLength() - 1, 1);
		size = pDC->GetTextExtent(sText);
	}

	if (sText.IsEmpty())
	{
		// not enough space for even one character, why borther drawing?
		pDC->SelectObject(pFont);
		return;
	}

	int nBkMode = pDC->SetBkMode(TRANSPARENT);
	if (GetStyle() & WS_DISABLED)
	{
		COLORREF color = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		pDC->TextOut(m_rcContent.left, m_rcContent.top, sText);
		pDC->SetTextColor(color);
	}
	else
	{
		pDC->TextOut(m_rcContent.left, m_rcContent.top, sText);
	}
	
	pDC->SelectObject(pFont);
	pDC->SetBkMode(nBkMode);
}

void CBrowseCtrl::DrawButtonArrowFolder(CDC *pDC, const CRect& rect) const
{
	if (!CheckDrawingArea(rect))
		return;

	CPoint pt;
	CLineDraw ld(pDC, &pt, &rect);
	const BOOL DISABLED = (GetStyle() & WS_DISABLED) != 0;
	int i = 0;
	
	CPen blackPen, whitePen, yellowPen, olivePen;
	blackPen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : BLACK);
	whitePen.CreatePen(PS_SOLID, 1, DISABLED ? WHITE : WHITE);
	yellowPen.CreatePen(PS_SOLID, 1, DISABLED ? GRAY : YELLOW);
	olivePen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : OLIVE);

	// black frame
	CPen* pPen = pDC->SelectObject(&blackPen);
	ld.InitOffsetMoveTo(1, 13);
	ld.OffsetLineTo(10, 0);
	ld.OffsetLineTo(4, -4);
	ld.OffsetLineTo(0, -1);
	ld.OffsetLineTo(-9, 0);
	ld.OffsetLineTo(-5, 5);
	ld.OffsetLineTo(0, -8);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(2, 0);
	ld.OffsetLineTo(1, 1);
	ld.OffsetLineTo(6, 0);
	ld.OffsetLineTo(0, 2);

	// top bending arrow
	ld.InitOffsetMoveTo(9, 2);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(2, 0);
	ld.OffsetLineTo(2, 2);
	ld.OffsetMoveTo(1, -1);
	ld.OffsetLineTo(0, 2);
	ld.OffsetLineTo(-2, 0);

	// Olive patterns
	pPen = pDC->SelectObject(&olivePen);
	ld.InitOffsetMoveTo(6, 9);

	for (i = 0; i < 4; i++)
	{
		ld.OffsetLineTo(8, 0);
		ld.OffsetMoveTo(-9, 1);
	}

	// yellow patterns
	pPen = pDC->SelectObject(&yellowPen);
	ld.InitOffsetMoveTo(2, 6);
	ld.OffsetLineTo(1, -1);
	ld.InitOffsetMoveTo(2, 8);
	ld.OffsetLineTo(2, -2);
	ld.InitOffsetMoveTo(2, 10);
	ld.OffsetLineTo(4, -4);
	ld.OffsetLineTo(1, 1);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(1, 1);
	ld.OffsetLineTo(1, -1);

	// white patterns
	pPen = pDC->SelectObject(&whitePen);
	ld.InitOffsetMoveTo(2, 5);
	ld.OffsetLineTo(0, 0);
	ld.OffsetMoveTo(0, 2);
	ld.OffsetLineTo(2, -2);
	ld.InitOffsetMoveTo(2, 9);
	ld.OffsetLineTo(3, -3);
	ld.InitOffsetMoveTo(2, 11);
	ld.OffsetLineTo(5, -5);
	ld.OffsetLineTo(1, 1);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(1, 1);

	pDC->SelectObject(pPen);
	blackPen.DeleteObject();
	whitePen.DeleteObject();
	yellowPen.DeleteObject();
	olivePen.DeleteObject();
}

void CBrowseCtrl::DrawButtonFolder(CDC *pDC, const CRect& rect) const
{
	if (!CheckDrawingArea(rect))
		return;

	CPoint pt;
	CLineDraw ld(pDC, &pt, &rect);
	const BOOL DISABLED = (GetStyle() & WS_DISABLED) != 0;
	int i = 0;

	CPen blackPen, whitePen, yellowPen, darkPen, grayPen;
	blackPen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : BLACK);
	whitePen.CreatePen(PS_SOLID, 1, DISABLED ? WHITE : WHITE);
	yellowPen.CreatePen(PS_SOLID, 1, DISABLED ? GRAY : YELLOW);
	darkPen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : DARK);
	grayPen.CreatePen(PS_SOLID, 1, DISABLED ? GRAY : GRAY);
	
	// right side black part
	CPen* pPen = pDC->SelectObject(&blackPen);
	ld.InitOffsetMoveTo(15, 4);
	ld.OffsetLineTo(0, 9);
	ld.OffsetLineTo(-12, 0);
	ld.OffsetMoveTo(11, -1);
	ld.OffsetLineTo(-2, -5);

	// front white part
	pDC->SelectObject(&whitePen);
	ld.InitOffsetMoveTo(3, 12);
	ld.OffsetLineTo(-2, -5);
	ld.OffsetLineTo(9, 0);

	// front dark frame
	pDC->SelectObject(&darkPen);
	ld.InitOffsetMoveTo(13, 11);
	ld.OffsetLineTo(0, 1);
	ld.OffsetLineTo(-11, 0);
	ld.OffsetLineTo(-2, -5);
	ld.OffsetLineTo(0, -1);
	ld.OffsetLineTo(12, 0);
	ld.OffsetLineTo(-1, 1);
	ld.OffsetMoveTo(2, 1);
	ld.OffsetLineTo(-1, 1);

	// front gray patterns
	pDC->SelectObject(&grayPen);
	pt = rect.TopLeft();
	pt.Offset(2, 8);

	for (i = 0; i < 5; i++)
	{
		pDC->MoveTo(pt);
		if (i == 4)
			pt.Offset(2, 2);
		else
			pt.Offset(3, 3);
		ld.OffsetLineTo(0, 0);
		pt.Offset(-1, -3);
	}

	// front yellow patterns
	pDC->SelectObject(&yellowPen);
	pt = rect.TopLeft();
	pt.Offset(3, 10);

	for (i = 0; i < 6; i++)
	{
		pDC->MoveTo(pt);
		if (i == 0)
			pt.Offset(1, 1);
		else if (i == 5)
			pt.Offset(0, 0);
		else
			pt.Offset(3, 3);
		ld.OffsetLineTo(0, 0);
		pt.Offset(-1, -3);
	}

	// rear dark frame
	pDC->SelectObject(&darkPen);
	ld.InitOffsetMoveTo(1, 5);
	ld.OffsetLineTo(0, -2);
	ld.OffsetLineTo(2, -2);
	ld.OffsetLineTo(4, 0);
	ld.OffsetLineTo(2, 2);
	ld.OffsetLineTo(5, 0);
	ld.OffsetLineTo(0, 7);

	// rear white part
	pDC->SelectObject(&whitePen);
	ld.InitOffsetMoveTo(2, 5);
	ld.OffsetLineTo(0, -2);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(4, 0);
	ld.OffsetLineTo(2, 2);
	ld.OffsetLineTo(4, 0);

	// rear gray patterns
	pDC->SelectObject(&grayPen);
	pt = rect.TopLeft();
	pt.Offset(3, 5);

	for (i = 0; i < 4; i++)
	{
		pDC->MoveTo(pt);
		if (i > 0)
			pt.Offset(2, 2);
		ld.OffsetLineTo(0, 0);
		pt.Offset(0, -2);
	}
	ld.OffsetLineTo(4, 2);
	ld.OffsetLineTo(0, 2);

	// rear yellow patterns
	pDC->SelectObject(&yellowPen);
	pt = rect.TopLeft();
	pt.Offset(3, 4);

	for (i = 0; i < 3; i++)
	{
		pDC->MoveTo(pt);
		if (i == 0)
			pt.Offset(1, 1);
		else
			pt.Offset(2, 2);
		ld.OffsetLineTo(0, 0);
		pt.Offset(0, -2);
	}

	ld.OffsetMoveTo(2, 2);
	ld.OffsetLineTo(0, 0);
	ld.OffsetMoveTo(2, 0);
	ld.OffsetLineTo(1, 1);

	// clean up
	pDC->SelectObject(pPen);
	blackPen.DeleteObject();
	whitePen.DeleteObject();
	yellowPen.DeleteObject();
	darkPen.DeleteObject();
	grayPen.DeleteObject();
}

void CBrowseCtrl::DrawButtonExplorer(CDC *pDC, const CRect& rect) const
{
	if (!CheckDrawingArea(rect))
		return;

	CPoint pt;
	CLineDraw ld(pDC, &pt, &rect);
	const BOOL DISABLED = (GetStyle() & WS_DISABLED) != 0;
	int i = 0;

	CPen blackPen, whitePen, yellowPen, darkPen, grayPen, olivePen, cyanPen, bluePen, navyPen;
	blackPen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : BLACK);
	whitePen.CreatePen(PS_SOLID, 1, DISABLED ? WHITE : WHITE);
	yellowPen.CreatePen(PS_SOLID, 1, DISABLED ? GRAY : YELLOW);
	darkPen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : DARK);
	grayPen.CreatePen(PS_SOLID, 1, DISABLED ? GRAY : GRAY);
	olivePen.CreatePen(PS_SOLID, 1, DISABLED ? GRAY : OLIVE);
	cyanPen.CreatePen(PS_SOLID, 1, DISABLED ? WHITE : CYAN);
	bluePen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : BLUE);
	navyPen.CreatePen(PS_SOLID, 1, DISABLED ? DARK : NAVY);

	// top-left dark frame
	CPen* pPen = pDC->SelectObject(&darkPen);
	ld.InitOffsetMoveTo(0, 11);
	ld.OffsetLineTo(0, -9);
	ld.OffsetLineTo(2, -2);
	ld.OffsetLineTo(4, 0);
	ld.OffsetLineTo(2, 2);
	ld.OffsetLineTo(5, 0);

	// scope dark border
	ld.InitOffsetMoveTo(8, 9);
	ld.OffsetLineTo(-1, 1);
	ld.OffsetLineTo(-3, 0);
	ld.OffsetLineTo(-2, -2);
	ld.OffsetLineTo(0, -2);
	ld.OffsetLineTo(2, -2);
	ld.OffsetLineTo(3, 0);
	ld.OffsetLineTo(2, 2);

	// black lines
	pPen = pDC->SelectObject(&blackPen);
	ld.InitOffsetMoveTo(14, 3);
	ld.OffsetLineTo(0, 8);
	ld.OffsetLineTo(-1, 1);
	ld.OffsetLineTo(1, 1);
	ld.OffsetLineTo(0, 1);
	ld.OffsetLineTo(-1, 1);
	ld.OffsetLineTo(-1, 0);
	ld.OffsetMoveTo(-4, -3);
	ld.OffsetLineTo(-7, 0);
	ld.OffsetMoveTo(4, -2);
	ld.OffsetLineTo(1, 0);
	ld.OffsetLineTo(2, -2);
	ld.OffsetLineTo(0, -2);
	ld.OffsetLineTo(-2, -2);

	// yellow lines
	pPen = pDC->SelectObject(&yellowPen);
	ld.InitOffsetMoveTo(2, 10);
	ld.OffsetLineTo(0, 0);
	ld.InitOffsetMoveTo(2, 4);
	ld.OffsetLineTo(0, -2);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(1, 1);
	ld.OffsetLineTo(1, -1);
	ld.OffsetLineTo(7, 7);
	ld.OffsetMoveTo(-2, 0);
	ld.OffsetLineTo(2, 2);
	ld.OffsetMoveTo(0, -4);
	ld.OffsetLineTo(-2, -2);
	ld.OffsetMoveTo(2, 0);
	ld.OffsetLineTo(0, 0);

	// gray parts
	pPen = pDC->SelectObject(&grayPen);
	ld.InitOffsetMoveTo(1, 2);

	int q = -1;
	for (i = 0; i < 5; i++, q = -q)
		ld.OffsetLineTo(1, q);

	ld.OffsetLineTo(7, 7);
	ld.OffsetMoveTo(0, -2);
	ld.OffsetLineTo(-1, -1);
	ld.OffsetMoveTo(-1, 3);
	ld.OffsetLineTo(2, 2);
	ld.OffsetMoveTo(-4, 1);
	ld.OffsetLineTo(0, 0);
	ld.OffsetMoveTo(-2, -1);
	ld.OffsetLineTo(1, -1);
	ld.OffsetMoveTo(0, -2);
	ld.OffsetLineTo(-4, 4);
	ld.OffsetLineTo(-1, -1);
	ld.OffsetMoveTo(0, -4);
	ld.OffsetLineTo(1, -1);

	// white parts
	pPen = pDC->SelectObject(&whitePen);
	ld.InitOffsetMoveTo(1, 10);
	ld.OffsetLineTo(0, -7);
	ld.OffsetLineTo(11, 0);
	ld.InitOffsetMoveTo(3, 6);
	ld.OffsetLineTo(1, -1);
	ld.OffsetMoveTo(2, 0);
	ld.OffsetLineTo(-3, 3);

	// Olive parts
	pPen = pDC->SelectObject(&olivePen);
	ld.InitOffsetMoveTo(13, 3);
	ld.OffsetLineTo(0, 8);
	ld.OffsetLineTo(-1, 0);
	ld.OffsetLineTo(-2, -2);
	ld.OffsetLineTo(-1, 0);
	ld.OffsetLineTo(0, -2);
	ld.OffsetMoveTo(-1, 4);
	ld.OffsetLineTo(-6, 0);

	// Cyan parts
	pPen = pDC->SelectObject(&cyanPen);
	ld.InitOffsetMoveTo(5, 5);
	ld.OffsetLineTo(-2, 2);
	ld.OffsetLineTo(2, 2);
	ld.OffsetLineTo(2, -2);
	ld.OffsetLineTo(-1, -1);
	ld.OffsetLineTo(-1, 1);
	ld.InitOffsetMoveTo(9, 11);
	ld.OffsetLineTo(3, 3);

	// Navy parts
	pPen = pDC->SelectObject(&navyPen);
	ld.InitOffsetMoveTo(8, 11);
	ld.OffsetLineTo(3, 3);

	// Blue parts
	pPen = pDC->SelectObject(&bluePen);
	ld.InitOffsetMoveTo(9, 10);
	ld.OffsetLineTo(4, 4);
	ld.OffsetLineTo(0, -1);
	ld.OffsetLineTo(-3, -3);
	
	// clean up
	pDC->SelectObject(pPen);
	blackPen.DeleteObject();
	whitePen.DeleteObject();
	yellowPen.DeleteObject();
	darkPen.DeleteObject();
	grayPen.DeleteObject();
	olivePen.DeleteObject();
	cyanPen.DeleteObject();
	bluePen.DeleteObject();
	navyPen.DeleteObject();
}

void CBrowseCtrl::OnMove(int x, int y) 
{
	CButton::OnMove(x, y);
	RecalculateRects(TRUE);
}

void CBrowseCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	RecalculateRects(TRUE);
}

void CBrowseCtrl::RecalculateRects(BOOL bMoveEdit)
{
	ASSERT(GetSafeHwnd() != NULL);
	CRect rcClient;
	GetClientRect(&rcClient);	

	// get the button rect
	m_rcButton = rcClient;
	CSize netSize(ICON_WIDTH, ICON_HEIGHT);
	int nReqWidth = ICON_WIDTH;

	if ((m_dwStyle & BC_BTN_ICON) == 0)
	{
		// Unlike icons, text size could vary
		CClientDC dc(this);
		dc.SelectObject(GetFont());
		netSize = dc.GetTextExtent(m_sButtonText);
		nReqWidth = netSize.cx;
	}

	if (nReqWidth < ICON_WIDTH)
		nReqWidth = ICON_WIDTH;
	nReqWidth += WIDTH_SPACE;

	if (m_rcButton.Width() > nReqWidth)
	{
		if (m_dwStyle & BC_BTN_LEFT)
			m_rcButton.right = m_rcButton.left + nReqWidth; // Left side
		else
			m_rcButton.left = m_rcButton.right - nReqWidth; // Right side
	}

	// get the content rect(areat for drawing icons and text on the browse button)
	CRect rcBtn = m_rcButton;
	rcBtn.DeflateRect(BTN_EDGE, BTN_EDGE);
	
	m_rcContent = CRect(rcBtn.TopLeft(), netSize);

	if (rcBtn.Height() > netSize.cy)
		m_rcContent.OffsetRect(0, (rcBtn.Height() - netSize.cy) / 2);
	else
		m_rcContent.bottom = rcBtn.bottom;

	if (rcBtn.Width() > netSize.cx)
		m_rcContent.OffsetRect((rcBtn.Width() - netSize.cx) / 2, 0);
	else
		m_rcContent.right = rcBtn.right;
	
	if (m_bBtnPushed)
		m_rcContent.OffsetRect(1, 1);

	// Get the edit box area
	m_rcEdit = rcClient;
	if (m_dwStyle & BC_BTN_LEFT)
		m_rcEdit.left = m_rcButton.right;
	else
		m_rcEdit.right = m_rcButton.left;

	if (bMoveEdit && m_wndEdit.GetSafeHwnd() != NULL)
		m_wndEdit.MoveWindow(&m_rcEdit, TRUE);
}

BOOL CBrowseCtrl::CheckDrawingArea(const CRect& rect, const CSize* pSize) const
{
	CSize size = (pSize == NULL) ? CSize(ICON_WIDTH, ICON_HEIGHT) : *pSize;
	BOOL bOK = rect.Width() >= size.cx && rect.Height() >= size.cy;
	if (!bOK)
	{
#ifdef _DEBUG
		static BOOL bWarned = FALSE;
		if (!bWarned)
		{
			bWarned = TRUE;
			AfxMessageBox(_T("Debug Warning - Control Client Area Too Small:\n\nThe client rectangle of this CBrowseCtrl control window is too small to draw the selected icon or text!\n\nThis message will not appear in the release build."), MB_ICONEXCLAMATION);
		}
#endif
	}

	return bOK;
}
