#ifndef __HEAD_H__
#define __HEAD_H__

#include "ArrayEx.h"

class CHead
{
public:
	CHead();
	CHead(const CHead& rhs);

	void DeleteAllDefects();
	ULONG GetDefectCount() const;
	BOOL GetDefect(int nIndex, CPoint& defect) const;
	void AddDefect(ULONG lCylinder, ULONG lSector);
	BOOL IsEnabled() const;
	void Enable(BOOL bEnable);
	COLORREF GetColor() const;
	void SetColor(COLORREF color);	
	const CHead& operator=(const CHead& rhs);

private:
	CArrayEx<POINT, POINT> m_aDefects;
	COLORREF m_color;
	BOOL m_bEnabled;
};

#endif