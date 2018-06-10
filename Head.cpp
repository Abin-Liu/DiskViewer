#include "stdafx.h"
#include "Head.h"

CHead::CHead()
{
	m_color = RGB(255, 0, 0);
	m_bEnabled = TRUE;
}

CHead::CHead(const CHead& rhs)
{
	m_aDefects.Copy(rhs.m_aDefects);
	m_color = rhs.m_color;
	m_bEnabled = rhs.m_bEnabled;
}

const CHead& CHead::operator=(const CHead& rhs)
{
	m_aDefects.Copy(rhs.m_aDefects);
	m_color = rhs.m_color;
	m_bEnabled = rhs.m_bEnabled;
	return *this;
}

void CHead::SetColor(COLORREF color)
{
	m_color = color;
}

COLORREF CHead::GetColor() const
{
	return m_color;
}

void CHead::Enable(BOOL bEnable)
{
	m_bEnabled = bEnable;
}

BOOL CHead::IsEnabled() const
{
	return m_bEnabled;
}

void CHead::AddDefect(ULONG lCylinder, ULONG lSector)
{
	CPoint pt(lCylinder, lSector);
	m_aDefects.Add(pt);
}

BOOL CHead::GetDefect(int nIndex, CPoint& defect) const
{
	if (nIndex < 0 || nIndex >= m_aDefects.GetSize())
		return FALSE;
	
	defect = m_aDefects[nIndex];
	return TRUE;
}

ULONG CHead::GetDefectCount() const
{
	return m_aDefects.GetSize();
}

void CHead::DeleteAllDefects()
{
	m_aDefects.RemoveAll();
}
