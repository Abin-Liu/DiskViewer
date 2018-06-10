///////////////////////////////////////////////////////////////////////////
//				BrowseCtrl.h
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
//                3, Improved the mouse/focus events monitoring functions
//                4, Fixed a drawing glitch when the user clicks on the edges of
//                   the edit box.
//                5, Changed the drawing rects calculating methods for performance
//                   improvement.
//
/////////////////////////////////////////////////////////////////////////// 

#ifndef __BROWSECTRL_H__
#define __BROWSECTRL_H__

/////////////////////////////////////////////////////////////////////
//		Browse Button Style Flags
/////////////////////////////////////////////////////////////////////
#define BC_BTN_LEFT			0x0001 // Display the browse button on the left-side of the editbox
#define BC_BTN_ICON			0x0002 // Use icon, no text will be displayed
#define BC_BTN_FLAT			0x0004 // Use flat button style
#define BC_ICO_ARROWFOLDER	0x0008 // The arrow-folder icon, must be combined with BC_BTN_ICON
#define BC_ICO_FOLDER		0x0010 // The folder icon, must be combined with BC_BTN_ICON
#define BC_ICO_EXPLORER		0x0020 // The Windows Explorer icon, must be combined with BC_BTN_ICON
#define BC_CTL_ALLOWEDIT	0x0040 // Allow user to type in the editbox
#define BC_CTL_FOLDERSONLY	0x0080 // Browse for folder

class CBrowseCtrl : public CButton
{
public:

	/////////////////////////////////////////////////////////////////////
	//		Constructor & Destructor
	/////////////////////////////////////////////////////////////////////

	CBrowseCtrl();
	virtual ~CBrowseCtrl();

	/////////////////////////////////////////////////////////////////////
	//		Runtime Creation
	/////////////////////////////////////////////////////////////////////

	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, UINT nNotifyMessageID = 0);

	/////////////////////////////////////////////////////////////////////
	//		Operations
	/////////////////////////////////////////////////////////////////////

	int DoModal(); // Popup the file/folder browse dialog

	/////////////////////////////////////////////////////////////////////
	//		General Control Settings
	/////////////////////////////////////////////////////////////////////
	
	void SetButtonStyle(DWORD dwStyles); // The browse-button styles.
	DWORD GetButtonStyle() const;
	void SetButtonText(LPCTSTR lpText); // The button text
	CString GetButtonText() const;	
	void SetTooltipText(LPCTSTR lpText); // Button tooltip text
	CString GetTooltipText() const;		
	void SetNotifyMessageID(UINT nNotifyMsgID); // Sent to the parent window when dialog closes
	UINT GetNotifyMessageID() const;	

	/////////////////////////////////////////////////////////////////////
	//		Browse Dialog Settings
	/////////////////////////////////////////////////////////////////////

	// The string displays on the edit control, also will be the default selected
	// file/folder when the dialog pops up
	void SetPathName(LPCTSTR lpPathName);
	
	void SetOpenSave(BOOL bOpen); // "Open" or "Save As", CFileDialog only
	BOOL GetOpenSave() const;
	void SetDefExt(LPCTSTR lpDefExt); // Default file extension, CFileDialog only
	CString GetDefExt() const;
	void SetFilter(LPCTSTR lpFilter); // File extension filters, CFileDialog only
	CString GetFilter() const;
	void SetFileFlags(DWORD dwFlags); // File dialog flags, CFileDialog only
	DWORD GetFileFlags() const;
	void SetFolderFlags(UINT nFlags); // Folder dialog flags, folder dialog only
	UINT GetFolderFlags() const;
	BOOL GetReadOnlyPref( ) const; // The "Read-Only" checkbox on dialog, CFileDialog only
	void SetFolderDialogTitle(LPCTSTR lpTitle); // The text appear on the dialog, folder dialog only
	CString GetFolderDialogTitle() const;

	/////////////////////////////////////////////////////////////////////
	//		Browse Result Data Access
	/////////////////////////////////////////////////////////////////////

	int GetSelectedCount() const; // How many files did the user select?
	CString GetPathName() const; // The full path name
	POSITION GetStartPosition() const;	// The path-name start position, for multiple selection 
	CString GetNextPathName( POSITION& pos ) const;	// Next path name
	CString GetFileTitle() const; // File name without extension
	CString GetFileExt() const; // File extension
	CString GetFileName() const; // File name with extension

	/////////////////////////////////////////////////////////////////////
	//		Overloads
	/////////////////////////////////////////////////////////////////////

	void SetWindowText(LPCTSTR lpszString);
	void GetWindowText(CString& rString) const;
	int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;	
	BOOL EnableWindow(BOOL bEnable);

protected:

	/////////////////////////////////////////////////////////////////////
	//		Protected Methods
	/////////////////////////////////////////////////////////////////////
	BOOL CreateCtrl(); // Initialize the control
	
	// Button drawing
	void DrawButtonExplorer(CDC* pDC, const CRect& rect) const; // Draw the explorer icon on the button
	void DrawButtonFolder(CDC* pDC, const CRect& rect) const; // Draw the folder icon on the button
	void DrawButtonArrowFolder(CDC* pDC, const CRect& rect) const; // Draw the arrow-folder icon on the button
	void DrawButtonText(CDC* pDC, const CRect& rect) const; // Draw text on the button
	void DrawButtonFrame(CDC* pDC) const; // Draw the button frame and background

	// File/Folder dialog popup
	int FileDoModal();
	int FolderDoModal();

	// Folder dialog call back function
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT nMsg, LPARAM lParam, LPARAM lpData);
	
	// Edit control
	BOOL CreateEdit();
	void DrawEditFrame(CDC* pDC) const;
	
	// Mouse/input events
	void OnMouseEnter(); // The mouse cursor has entered the browse button area
	void OnMouseLeave(); // The mouse cursor has left the browse button area
	void OnSetEditFocus(); // The input focus has entered the edit control
	void OnKillEditFocus(); // The input focus has left the edit control

	BOOL CheckDrawingArea(const CRect& rect, const CSize* pSize = NULL) const;
	void RecalculateRects(BOOL bMoveEdit = FALSE); // Recalculate drawing rects

protected:	

	/////////////////////////////////////////////////////////////////////
	//		Member Data
	/////////////////////////////////////////////////////////////////////

	// If greater than 0, this message is sent to the parent window when the
	// browse dialog is closed, wParam is either IDOK or IDCANCEL, lParam 
	// is number of files the user selected. In case of browing for folders, 
	// lParam is always 1 because SHBrowseForFolder does not allow multiple
	// selection.
	UINT m_nNotifyMsg; 

	CRect m_rcButton;
	CRect m_rcContent;
	CRect m_rcEdit;
	CEdit m_wndEdit; // The edit control which displays browse results
	BOOL m_bEditFocused; // The edit control is currently focused
	BOOL m_bOwnCreate; // Set to TRUE when the "Create" method has been called
	DWORD m_dwStyle; // The control styles
	BOOL m_bBtnPushed; // Set to TRUE if the button has been pused down, FALSE when released.
	BOOL m_bMouseIn; // Set to TRUE if the mouse cursor has entered the button area
	CToolTipCtrl m_wndTooltip; // The tooltip control for the button
	CString m_sTootipText; // Tooltip text, can be changed at fly
	
	// The button text, only displayed BC_BTN_ICON flag is not set
	CString m_sButtonText;

	// stuff for the file/folder dialog
	BOOL m_bDlgPopped; // The file/folder dialog has been popped up
	CString m_sDlgTitle; // only valid when m_bBrowseForFolder is true
	int m_nPathNamesLen; // Used when multiple files are selected, CFileDialog only
	BOOL m_bOpenFileDialog; // "Open" or "Save as", CFileDialog only
	DWORD m_dwFileFlags; // Dialog flags, CFileDialog only
	UINT m_nFolderFlags; // Dialog flags, folder dialog only
	BOOL m_bReadOnlyRef; // The "Open as Read Only" mark, CFileDialog only
	CString m_sDefExt; // Default file extension, CFileDialog only
	CString m_sFilter; // Default filters, CFileDialog only
	LPTSTR m_lpszPathNames; // Default path name, CFileDialog only
	int m_nSelCount;	// How many files did the user select?

	/////////////////////////////////////////////////////////////////////
	//		Wizard Generated Stuff
	/////////////////////////////////////////////////////////////////////

	//{{AFX_VIRTUAL(CBrowseCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CBrowseCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // #ifndef __BROWSECTRL_H__
