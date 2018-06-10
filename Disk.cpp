// Disk.cpp: implementation of the CDisk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Disk.h"
#include <math.h>
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MIN_ZONE	10
#define BORDER_WIDTH	3
#define DEF_INNER_RESERVED	(ULONG((double)m_lTotalTrks * 0.50)) // Inner reserved cylinders, 25% of total cylinders
#define PI	3.14159265358979	// PI
#define UINT_INVALID 0xffffffff // Indicates an invalid ULONG value

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisk::CDisk()
{
	m_ptOrigin = CPoint(0, 0);
	m_lScrRadius = 0;
	m_lTotalTrks = 0;
	m_bZoomedIn = FALSE;
}

CDisk::~CDisk()
{
}

void CDisk::DrawDisk(CDC *pDC, COLORREF trackColor, COLORREF borderColor, COLORREF bkColor) const
{
	if (pDC == NULL || m_lTotalTrks == 0)
		return;

	CPen trackPen; // Pen to draw disk tracks
	CPen borderPen; // Pen to draw disk border
	CBrush diskBrush; // draw disk background
	trackPen.CreatePen(PS_SOLID, 1, trackColor);
	borderPen.CreatePen(PS_SOLID, 1, borderColor);
	diskBrush.CreateSolidBrush(bkColor);

	CRect rc;
	ULONG lRadius;
	CPen* pOldPen = pDC->SelectObject(&trackPen);
	CBrush* pOldBrush = pDC->SelectObject(&diskBrush);

	// draw circles from OD to ID
	for (int i = 0; i < m_aZones.GetSize(); i++)
	{
		lRadius = _GetTrackScrRadius(m_aZones[i].lOutterTrack);		
		rc.left = m_ptOrigin.x - lRadius;
		rc.right = m_ptOrigin.x + lRadius;
		rc.top = m_ptOrigin.y - lRadius;
		rc.bottom = m_ptOrigin.y + lRadius;
		//pDC->Arc(&rc, rc.TopLeft(), rc.TopLeft());
		pDC->Ellipse(&rc);

		lRadius = _GetTrackScrRadius(m_aZones[i].lOutterTrack + m_aZones[i].lTrackNumber);	
		rc.left = m_ptOrigin.x - lRadius;
		rc.right = m_ptOrigin.x + lRadius;
		rc.top = m_ptOrigin.y - lRadius;
		rc.bottom = m_ptOrigin.y + lRadius;
		//pDC->Arc(&rc, rc.TopLeft(), rc.TopLeft());
		pDC->Ellipse(&rc);
	}

	// draw a horizontal line indicates the zero sector of all tracks
	// on the out-most track
	pDC->MoveTo(_GetPointOnTrack(m_lTotalTrks, 0.0));
	pDC->LineTo(_GetPointOnTrack(0, 0.0));

	pDC->SelectObject(&borderPen);	
	
	// draw outter border
	lRadius = m_lScrRadius;		
	rc.left = m_ptOrigin.x - lRadius;
	rc.right = m_ptOrigin.x + lRadius;
	rc.top = m_ptOrigin.y - lRadius;
	rc.bottom = m_ptOrigin.y + lRadius;
	pDC->Arc(&rc, rc.TopLeft(), rc.TopLeft());
	rc.InflateRect(BORDER_WIDTH, BORDER_WIDTH);
	pDC->Arc(&rc, rc.TopLeft(), rc.TopLeft());

	// draw inner border
	CBrush emptyBrush;
	emptyBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
	pDC->SelectObject(&emptyBrush);

	lRadius = ULONG((double)DEF_INNER_RESERVED * (double)m_lScrRadius / (double)(m_lTotalTrks + DEF_INNER_RESERVED));		
	rc.left = m_ptOrigin.x - lRadius;
	rc.right = m_ptOrigin.x + lRadius;
	rc.top = m_ptOrigin.y - lRadius;
	rc.bottom = m_ptOrigin.y + lRadius;
	pDC->Ellipse(&rc);
	rc.DeflateRect(BORDER_WIDTH, BORDER_WIDTH);
	pDC->Ellipse(&rc);
	
	if (m_bZoomedIn)
	{
		CPen zoomPen; // Pen to draw the zoom boundaries
		zoomPen.CreatePen(PS_DOT, 1, RGB(128, 128, 128));
		pDC->SelectObject(&zoomPen);
		// draw zoom boundaries
		pDC->MoveTo(m_rcZoom.TopLeft());
		pDC->LineTo(m_rcZoom.right, m_rcZoom.top);
		pDC->LineTo(m_rcZoom.BottomRight());
		pDC->LineTo(m_rcZoom.left, m_rcZoom.bottom);
		pDC->LineTo(m_rcZoom.TopLeft());
		zoomPen.DeleteObject();
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	trackPen.DeleteObject();
	borderPen.DeleteObject();
	diskBrush.DeleteObject();
}

void CDisk::DrawDefects(CDC *pDC) const
{
	// draw defections
	if (pDC == NULL || m_lTotalTrks == 0)
		return;

	for (int i = 0; i < m_aHeads.GetSize(); i++)
	{
		if (!m_aHeads[i].IsEnabled())
			continue;

		CPen defectionPen; // Pen to draw defection border
		CBrush defectionBrush; // Brush to fill defection dots
		defectionPen.CreatePen(PS_SOLID, 1, m_aHeads[i].GetColor());
		defectionBrush.CreateSolidBrush(m_aHeads[i].GetColor());

		CRect rc;
		CBrush* pOldBrush = pDC->SelectObject(&defectionBrush);
		pDC->SelectObject(&defectionPen);
		int def = 0;
		CPoint pt;
		while (m_aHeads[i].GetDefect(def++, pt))
		{
			DiskToScreen(pt, pt.x, pt.y);
			rc = CRect(pt, CSize(1, 1));
			rc.InflateRect(1, 1);
			pDC->Ellipse(&rc);
		}

		pDC->SelectObject(pOldBrush);
		defectionPen.DeleteObject();
		defectionBrush.DeleteObject();
	}
}

BOOL CDisk::AddZone(ULONG lStartCylinder, ULONG lEndCylinder, ULONG lSectorPerCylinder)
{
	if (lSectorPerCylinder == 0
		|| lEndCylinder == lStartCylinder)
	{
		return FALSE;
	}

	if (lEndCylinder < lStartCylinder)
	{
		ULONG lTemp = lEndCylinder;
		lEndCylinder = lStartCylinder;
		lStartCylinder = lTemp;
	}

	if (m_lTotalTrks < lEndCylinder + 1)
		m_lTotalTrks = lEndCylinder + 1;

	DISKZONE dz1;
	dz1.lOutterTrack = lStartCylinder;
	dz1.lTrackNumber = lEndCylinder - lStartCylinder + 1;
	dz1.lSectorsPerTrk = lSectorPerCylinder;

	if (_FindZoneIndex(dz1) >= 0)
		return TRUE; // already exists

	for (int i = 0; i < m_aZones.GetSize(); i++)
	{
		if (dz1.lOutterTrack <= m_aZones[i].lOutterTrack)
		{
			m_aZones.InsertAt(i, dz1);
			return TRUE;
		}
	}
	m_aZones.Add(dz1);
	return TRUE;
}

int CDisk::_FindZoneIndex(const DISKZONE& dz) const
{
	for (int i = 0; i < m_aZones.GetSize(); i++)
	{
		if (m_aZones[i].lSectorsPerTrk == dz.lSectorsPerTrk
			&& m_aZones[i].lOutterTrack == dz.lOutterTrack
			&& m_aZones[i].lTrackNumber == dz.lTrackNumber)
		{
			return i;
		}
	}
	return -1;
}

POINT CDisk::_GetPointOnTrack(ULONG lTrack, double fAngle) const
{
	POINT pt;
	ULONG lRadius = _GetTrackScrRadius(lTrack);
	lTrack = _GetAlteredTrack(lTrack);
	pt.x = ULONG(double(m_ptOrigin.x) + cos(fAngle * PI / 180.0) * (double)lRadius);
	pt.y = ULONG(double(m_ptOrigin.y) - sin(fAngle * PI / 180.0) * (double)lRadius);
	return pt;
}

ULONG CDisk::_GetTrackScrRadius(ULONG lTrack) const
{
	lTrack = _GetAlteredTrack(lTrack);
	return ULONG((double)(lTrack) * (double)m_lScrRadius / (double)(m_lTotalTrks + DEF_INNER_RESERVED));
}

BOOL CDisk::AddDefect(ULONG lHead, ULONG lTrack, ULONG lSector)
{
	if (!IsHeadValid(lHead) || !IsSectorValid(lTrack, lSector))
		return FALSE;

	m_aHeads[lHead].AddDefect(lTrack, lSector);
	return TRUE;
}

BOOL CDisk::DiskToScreen(CPoint& point, ULONG lCylinder, ULONG lSector) const
{
	// converts a location from disk track & sector to screen coordinates

	// first determine the zone of this track
	const int IDX = _TrackInZone(lCylinder);
	if (IDX < 0)
		return FALSE;

	// now determine angel according to sector location
	const DISKZONE dz = m_aZones[IDX];
	double fAngle = (double)lSector * 360.0 / (double)(dz.lSectorsPerTrk);
	point = _GetPointOnTrack(lCylinder, 360.0 - fAngle);
	return TRUE;
}

int CDisk::_TrackInZone(ULONG lTrack) const
{
	for (int i = 0; i < m_aZones.GetSize(); i++)
	{
		if (lTrack >= m_aZones[i].lOutterTrack && lTrack < m_aZones[i].lTrackNumber + m_aZones[i].lOutterTrack)
			return i;
	}

	return -1;
}

ULONG CDisk::GetCylinderCount() const
{
	return m_lTotalTrks;
}

BOOL CDisk::RemoveDefects(ULONG lHead)
{
	if (!IsHeadValid(lHead))
		return FALSE;

	m_aHeads[lHead].DeleteAllDefects();
	return TRUE;
}

BOOL CDisk::IsCylinderValid(ULONG lCylinder) const
{
	return _TrackInZone(lCylinder) >= 0;
}

BOOL CDisk::IsSectorValid(ULONG lTrack, ULONG lSector) const
{
	return lSector >= 0 && lSector < GetSectorCount(lTrack);
}

ULONG CDisk::GetSectorCount(ULONG lTrack) const
{
	const int IDX = _TrackInZone(lTrack);
	return IDX < 0 ? 0 : m_aZones[IDX].lSectorsPerTrk;
}

ULONG CDisk::GetZoneCount() const
{
	return (ULONG)m_aZones.GetSize();
}

ULONG CDisk::_GetAlteredTrack(ULONG lTrack) const
{
	return DEF_INNER_RESERVED + m_lTotalTrks - lTrack;
}

BOOL CDisk::InitDisk()
{
	m_sSN.Empty();
	m_lTotalTrks = 0;
	m_aHeads.RemoveAll();
	m_aZones.RemoveAll();
	m_bZoomedIn = FALSE;
	return TRUE;
}

void CDisk::SetSN(LPCTSTR lpSN)
{
	m_sSN = lpSN;
}

BOOL CDisk::IsHeadValid(ULONG lHead) const
{
	return lHead >= 0 && lHead < (ULONG)m_aHeads.GetSize();
}	

void CDisk::SetHeadCount(ULONG lCount)
{
	if (lCount > 0)
		m_aHeads.SetSize(lCount);
	else
		m_aHeads.RemoveAll();

	for (int i = 0; i < m_aHeads.GetSize(); i++)
		m_aHeads[i].DeleteAllDefects();
}

BOOL CDisk::ScreenToDisk(CPoint point, ULONG &lZone, ULONG &lCylinder, ULONG &lSector) const
{
	lZone = UINT_INVALID;
	lCylinder = UINT_INVALID;
	lSector = UINT_INVALID;

	if (point == m_ptOrigin)
		return FALSE;

	// determine the radius
	double fRadius = sqrt(double(point.x - m_ptOrigin.x) * double(point.x - m_ptOrigin.x) + double(point.y - m_ptOrigin.y) * double(point.y - m_ptOrigin.y));
	lCylinder = m_lTotalTrks + DEF_INNER_RESERVED - ULONG((double)(m_lTotalTrks + DEF_INNER_RESERVED) * fRadius / (double)m_lScrRadius);
	if (lCylinder >= m_lTotalTrks)
		return FALSE;

	int nIdx = _TrackInZone(lCylinder);
	if (nIdx < 0)
		return FALSE;

	lZone = (ULONG)nIdx;

	// determine the angle
	double fAngle = 0.0;
	
	if (m_ptOrigin.x == point.x)
	{
		// vertical special case
		fAngle = point.y > m_ptOrigin.y ? 270.0 : 90.0;
	}
	else if (m_ptOrigin.y == point.y)
	{
		// horizontal special case
		fAngle = point.x > m_ptOrigin.x ? 0.0 : 180.0;
	}
	else
	{		
		// common case
		fAngle = atan(((double)point.y - (double)m_ptOrigin.y) / ((double)point.x - (double)m_ptOrigin.x)) * 180.0 / PI;

		// determine the phases (1-4)
		// Phases allocation figure:
		/*
		            y
		            |
		     P2     |    P1
		            |
		 -----------+----------->x
		            |
		     P3     |    P4
		            |
		 */
		int nPhase = 0;
		if (point.y < m_ptOrigin.y)
			nPhase = point.x > m_ptOrigin.x ? 1 : 2;
		else
			nPhase = point.x > m_ptOrigin.x ? 4 : 3;

		// adjust the angle according to phases
		switch (nPhase)
		{
		case 1:
			fAngle = -fAngle;
			break;

		case 2:
			fAngle = 180.0 - fAngle;
			break;

		case 3:
			fAngle = 180.0 - fAngle;
			break;

		case 4:
			fAngle = 360.0 - fAngle;
			break;

		default:
			return FALSE;
			break;
		}
	}

	// map the screen angle to disk angle
	if (fAngle > 0.0)
		fAngle = 360.0 - fAngle;

	// Convert to sector location based on angle
	lSector = ULONG(double(m_aZones[lZone].lSectorsPerTrk) * (fAngle / 360.0));
	return TRUE;
}

void CDisk::GetSN(LPTSTR lpBuffer, int nBufSize) const
{
	ASSERT(lpBuffer != NULL);
	_tcsncpy(lpBuffer, (LPCTSTR)m_sSN, nBufSize);
}

ULONG CDisk::GetHeadCount() const
{
	return m_aHeads.GetSize();
}

ULONG CDisk::GetDefectCount(ULONG lHead) const
{
	if (!IsHeadValid(lHead))
		return 0;
	return m_aHeads[lHead].GetDefectCount();
}

BOOL CDisk::IsZoneValid(ULONG lZone) const
{
	return lZone >= 0 && lZone < (ULONG)m_aZones.GetSize();
}

void CDisk::SetBoundary(POINT ptTopLeft, POINT ptBottomRight)
{
	m_bZoomedIn = FALSE;
	CRect rect = RectFromPts(ptTopLeft, ptBottomRight, TRUE, TRUE);
	m_ptOrigin = rect.CenterPoint();
	m_lScrRadius = rect.Width() / 2;
}

void CDisk::ZoomIn(POINT ptTopLeft, POINT ptBottomRight)
{
	if (m_bZoomedIn)
		return;

	m_bZoomedIn = TRUE;

	// Keep a copy for the original unzoomed boundary
	CRect rcOld;
	rcOld.left = m_ptOrigin.x - m_lScrRadius;
	rcOld.right = rcOld.left + m_lScrRadius * 2;
	rcOld.top = m_ptOrigin.y - m_lScrRadius;
	rcOld.bottom = rcOld.top + m_lScrRadius * 2;

	CRect rect = RectFromPts(ptTopLeft, ptBottomRight, TRUE, FALSE);

	if (rect.Width() < MIN_ZONE)
		rect.InflateRect((MIN_ZONE - rect.Width()) / 2, (MIN_ZONE - rect.Width()) / 2);

	CPoint pt1 = rect.CenterPoint();
	long l1 = rect.Width();	

	double fRadiusRatio = (double)(m_lScrRadius * 2) / (double)l1;	
	long lRadius = long(fRadiusRatio * (double)m_lScrRadius);

	CRect newRect(m_ptOrigin, CSize(1, 1));
	newRect.InflateRect(lRadius / 2, lRadius / 2);
	newRect.left = rcOld.left;
	newRect.right = newRect.left + (long)lRadius;
	newRect.top = rcOld.top;
	newRect.bottom = newRect.top + lRadius;

	CPoint ptOrigin = m_ptOrigin;
	ptOrigin.x -= long(double(pt1.x - ptOrigin.x) * fRadiusRatio);
	ptOrigin.y -= long(double(pt1.y - ptOrigin.y) * fRadiusRatio);

	rect = RectFromPts(ptTopLeft, ptBottomRight, FALSE, FALSE);
	double fSideRatio = double(rect.Width()) / double(rect.Height());
	
	m_rcZoom = rcOld;

	if (rect.Width() > rect.Height()) // need to calc height
	{
		long lHeight = long(double(m_rcZoom.Width()) / fSideRatio);
		m_rcZoom.top = m_rcZoom.CenterPoint().y - lHeight / 2;
		m_rcZoom.bottom = m_rcZoom.top + lHeight;
	}
	else if (rect.Height() > rect.Width()) // need to calc width
	{
		long lWidth = long(double(m_rcZoom.Width()) * fSideRatio);
		m_rcZoom.left = m_rcZoom.CenterPoint().x - lWidth / 2;
		m_rcZoom.right = m_rcZoom.left + lWidth;
	}
	m_ptOrigin = ptOrigin;
	m_lScrRadius = lRadius;
}

CRect CDisk::RectFromPts(POINT pt1, POINT pt2, BOOL bMustSquare, BOOL bUseMin)
{
	CRect rect;
	rect.left = min(pt1.x, pt2.x);
	rect.top = min(pt1.y, pt2.y);
	rect.right = max(pt1.x, pt2.x);
	rect.bottom = max(pt1.y, pt2.y);

	if (bMustSquare)
	{
		if (rect.Width() > rect.Height())
		{
			if (bUseMin)
				rect.DeflateRect((rect.Width() - rect.Height()) / 2, 0);
			else
				rect.InflateRect(0, (rect.Width() - rect.Height()) / 2);
		}
			
	
		if (rect.Height() > rect.Width())
		{
			if (bUseMin)
				rect.DeflateRect(0, (rect.Height() - rect.Width()) / 2);
			else
				rect.InflateRect((rect.Height() - rect.Width()) / 2, 0);
		}			
	}

	return rect;
}

BOOL CDisk::IsZoomedIn() const
{
	return m_bZoomedIn;
}

BOOL CDisk::IsDiskReady() const
{
	return m_bZoomedIn || (m_lScrRadius > 0 && m_lTotalTrks > 0 && m_aZones.GetSize() > 0);
}

COLORREF CDisk::GetHeadColor(ULONG lHead) const
{
	return IsHeadValid(lHead) ? m_aHeads[lHead].GetColor() : RGB(255, 0, 0);
}

BOOL CDisk::SetHeadColor(ULONG lHead, COLORREF color)
{
	if (!IsHeadValid(lHead))
		return FALSE;

	m_aHeads[lHead].SetColor(color);
	return TRUE;
}

void CDisk::EnableHead(ULONG lHead, BOOL bEnable)
{
	if (IsHeadValid(lHead))
		m_aHeads[lHead].Enable(bEnable);
}

BOOL CDisk::IsHeadEnabled(ULONG lHead) const
{
	return IsHeadValid(lHead) ? m_aHeads[lHead].IsEnabled() : FALSE;
}

BOOL CDisk::GetDefect(ULONG lHead, int nIndex, CPoint &defect) const
{
	if (!IsHeadValid(lHead))
		return FALSE;

	return m_aHeads[lHead].GetDefect(nIndex, defect);
}

ULONG CDisk::GetTotaldefectCount() const
{
	ULONG lCount = 0;
	for (int i = 0; i < m_aHeads.GetSize(); i++)
		lCount += m_aHeads[i].GetDefectCount();
	return lCount;
}

void CDisk::RemoveAllDefects()
{
	for (int i = 0; i < m_aHeads.GetSize(); i++)
		m_aHeads[i].DeleteAllDefects();
}
