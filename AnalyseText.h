// AnalyseText.h: interface for the CAnalyseText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSETEXT_H__8A0505F4_A18F_4317_A2BB_E29557326631__INCLUDED_)
#define AFX_ANALYSETEXT_H__8A0505F4_A18F_4317_A2BB_E29557326631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Disk.h"

enum { LOAD_OK = 0, INVALID_PARAM, ZONEFILE_FAIL, DEFECTFILE_FAIL };

class __declspec(dllexport) CAnalyseText  
{
public:
	
	CAnalyseText();
	virtual ~CAnalyseText();

	int LoadFiles(LPCTSTR lpZoneFile, LPCTSTR lpDefectFile);
	const CDisk* GetDisk() const;
	CDisk* GetDisk();
	void GetDefectFileName(LPTSTR lpBuffer, int nMaxSize) const;
	void GetZoneFileName(LPTSTR lpBuffer, int nMaxSize) const;
	
protected:
	static ULONG GetHeadCount(CString& sLine);
	BOOL GetDriveInfo(LPCTSTR sZoneFile);         //Get drive information including SN, HeadNum, and zone info
	BOOL GetDefectInfo(CString sAnaFile); //Get drive defect list
	BOOL ParseDefect(CString& sLine, BOOL bLog36);	
	BOOL OnReadFile(CStdioFile& file, LPCTSTR lpTextFind, int nCount, CString& sLine);
	CString GetSNInfo(CStdioFile& file);

	CDisk * m_pDisk;
	CString m_sBuffer;
	CString m_sZoneFile;
	CString m_sDefectFile;
};


#endif // !defined(AFX_ANALYSETEXT_H__8A0505F4_A18F_4317_A2BB_E29557326631__INCLUDED_)
