// AnalyseText.cpp: implementation of the CAnalyseText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AnalyseText.h"
#include "strsplit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnalyseText::CAnalyseText()
{
	//m_sSN = "";
	m_pDisk = new CDisk;
/*
	m_sBuffer = "";
	m_iHeadNum = 0;
	m_iMaxCyl = 0;
	m_iMinCyl = 0;
	m_iZone = 0;
	m_iHead = 0;
	m_iCylinder = 0;
	m_iSector = 0;
	*/
}

CAnalyseText::~CAnalyseText()
{
//	m_fOpen.Close();
//	m_fOpenZone.Close();
	delete m_pDisk;
}

BOOL CAnalyseText::OnReadFile(CStdioFile& file, LPCTSTR lpTextFind, int nCount, CString& sLine)
{
	if (lpTextFind == NULL)
		return FALSE;

	CString str;
	while( file.ReadString(str))
	{
		if (str.Find(lpTextFind) != -1)
		{
			for (int i = 0; i < nCount - 1; i++)
			{
				if (!file.ReadString(sLine))
				{
					sLine.Empty();
					return FALSE;
				}
			}
			return TRUE;
		}
	}

	return FALSE;	
}

CString CAnalyseText::GetSNInfo(CStdioFile& file)
{
	CString strSN;
	CString strTemp;

	if (!OnReadFile(file, "Log 01 - Health", 2, strTemp))
		return _T("");
	file.SeekToBegin();
	int i = strTemp.Find("(SN: ");	
	return i == -1 ? _T("") : strTemp.Mid(5, 8);
}

BOOL CAnalyseText::GetDefectInfo(CString sAnaFile)
{
	CStdioFile file;
	if (!file.Open(sAnaFile, CFile::modeRead))
		return FALSE;

	m_pDisk->SetSN(GetSNInfo(file));

	CString sLine;
	if (!OnReadFile(file, _T("Log 36 - Health"), 3, sLine))
		return FALSE;

	m_pDisk->SetHeadCount(GetHeadCount(sLine));
	while (file.ReadString(sLine))
	{
		if (!ParseDefect(sLine, TRUE))
			break;
	}

	// read log 37 if there are more than 2 heads
	if (m_pDisk->GetHeadCount() > 2)
	{
		if (OnReadFile(file, _T("Log 37 - Health"), 3, sLine))
		{
			while (file.ReadString(sLine))
			{
				if (!ParseDefect(sLine, FALSE))
					break;
			}
		}
	}

	file.Close();
	return TRUE;
}

BOOL CAnalyseText::GetDriveInfo(LPCTSTR sZoneFile)
{
	CStdioFile file;
	
	if (!file.Open(sZoneFile, CFile::modeRead))
		return FALSE;

	CString sLine;
	UINT nLineNo = 0;
	CStringArray aNums;
	while (file.ReadString(sLine))
	{
		CString sHead;
		sHead.Format(_T("Zone %02X:"), nLineNo);
		if (sLine.Find(sHead) == 0)
		{
			CString sNums = sLine.Mid(13);
			LPCTSTR pStart = sNums;
			LPTSTR pEnd = NULL;

			ULONG lBeginCylinder = _tcstoul(pStart, &pEnd, 16);
			pStart = pEnd;
			ULONG lEndCylinder = _tcstoul(pStart, &pEnd, 16);
			pStart = pEnd;
			ULONG lSectorPer = _tcstoul(pStart, &pEnd, 16);
			m_pDisk->AddZone(lBeginCylinder, lEndCylinder, lSectorPer);
			
			nLineNo++;
		}
	}

	file.Close();
	return TRUE;
}

CDisk* CAnalyseText::GetDisk()
{
	return m_pDisk;
}

const CDisk* CAnalyseText::GetDisk() const
{
	return m_pDisk;
}

BOOL CAnalyseText::ParseDefect(CString &sLine, BOOL bLog36)
{
	const int SUB_LEN = 21;
	CString sTemp(sLine);
	sTemp.TrimLeft();
	sTemp.TrimRight();
	if (sTemp.GetLength() < SUB_LEN - 3)
		return FALSE;

	const int STR_START = bLog36 ? 0 : 2 * SUB_LEN;
	const ULONG HEAD_START = bLog36 ? 0 : 2;

	for (ULONG i = 0; i < 2; i++)
	{
		CString sSeg = sLine.Mid(STR_START + i * SUB_LEN, SUB_LEN);
		sSeg.TrimLeft();
		sSeg.TrimRight();
		if (!sSeg.IsEmpty())
		{		
			ULONG lCylinder = _tcstoul(&((LPCTSTR)sSeg)[3], NULL, 16);
			ULONG lSector = _tcstoul(&((LPCTSTR)sSeg)[13], NULL, 16);
			m_pDisk->AddDefect(HEAD_START + i, lCylinder, lSector);
		}
	}

	return TRUE;
}

int CAnalyseText::LoadFiles(LPCTSTR lpZoneFile, LPCTSTR lpDefectFile)
{
	m_sZoneFile.Empty();
	m_sDefectFile.Empty();
	if (lpZoneFile == NULL 
		|| _tcslen(lpZoneFile) == 0
		|| lpDefectFile == NULL
		|| _tcslen(lpDefectFile) == 0)
	{
		return INVALID_PARAM;
	}

	m_pDisk->InitDisk();
	if (!GetDriveInfo(lpZoneFile))
	{
		m_pDisk->InitDisk();
		return ZONEFILE_FAIL;
	}

	if (!GetDefectInfo(lpDefectFile))
	{
		m_pDisk->InitDisk();
		return DEFECTFILE_FAIL;
	}
	
	m_sZoneFile = lpZoneFile;
	m_sDefectFile = lpDefectFile;
	return LOAD_OK;
}

void CAnalyseText::GetZoneFileName(LPTSTR lpBuffer, int nMaxSize) const
{
	if (lpBuffer != NULL)
		_tcsncpy(lpBuffer, m_sZoneFile, nMaxSize);
}

void CAnalyseText::GetDefectFileName(LPTSTR lpBuffer, int nMaxSize) const
{
	if (lpBuffer != NULL)
		_tcsncpy(lpBuffer, m_sDefectFile, nMaxSize);
}

ULONG CAnalyseText::GetHeadCount(CString &sLine)
{
	sLine.TrimLeft();
	sLine.TrimRight();
	ULONG lHead = 0;
	CString sFind;
	sFind.Format(_T("_____Head___%X______"), lHead);
	while (sLine.Find(sFind) != -1)
		sFind.Format(_T("_____Head___%X______"), ++lHead);

	return lHead;
}
