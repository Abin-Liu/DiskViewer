////////////////////////////////////////////////////////////////////////////////
//		Disk.h
//
// The CDisk class is used to simulate a disk surface, it accepts disk
// configurations and display the surface in a visual representation.
//
// Jan. 24, 2004, Abin
////////////////////////////////////////////////////////////////////////////////

#ifndef __DISK_H__
#define __DISK_H__

#include "ArrayEx.h"
#include "Head.h"

class CDisk
{
public:		
	
	////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////
	CDisk();
	virtual ~CDisk();

	////////////////////////////////////////////////////////////////////////////
	// Disk Initialization
	////////////////////////////////////////////////////////////////////////////
	virtual BOOL InitDisk(); // Reset the disk surface and other info

	////////////////////////////////////////////////////////////////////////////
	// Drawing Functions
	////////////////////////////////////////////////////////////////////////////
	void DrawDisk(CDC* pDC, COLORREF trackColor = RGB(192, 192, 192), COLORREF borderColor = RGB(0, 0, 0), COLORREF bkColor = RGB(255, 255, 255)) const;
	void DrawDefects(CDC* pDC) const;

	////////////////////////////////////////////////////////////////////////////
	// Screen-Disk Location Related
	////////////////////////////////////////////////////////////////////////////
	BOOL ScreenToDisk(CPoint point, ULONG& lZone, ULONG& lCylinder, ULONG& lSector) const; // converts a a screen coordinates into a disk location
	BOOL DiskToScreen(CPoint& point, ULONG lCylinder, ULONG lSector) const; // converts a disk location into a screen coordinates
	void SetBoundary(POINT ptTopLeft, POINT ptBottomRight);
	void ZoomIn(POINT ptTopLeft, POINT ptBottomRight);
	BOOL IsZoomedIn() const;
	static CRect RectFromPts(POINT pt1, POINT pt2, BOOL bMustSquare, BOOL bUseMin);

	////////////////////////////////////////////////////////////////////////////
	// Disk Configuration
	////////////////////////////////////////////////////////////////////////////
	
	// Drive serial No
	void SetSN(LPCTSTR lpSN);
	void GetSN(LPTSTR lpBuffer, int nBufSize) const;

	// Drive cylinder number
	ULONG GetCylinderCount() const; // Retrieve the number of total tracks on this surface
	BOOL IsCylinderValid(ULONG lCylinder) const; // Check if a track is valid
	
	// Zones on disk surface
	BOOL AddZone(ULONG lStartCylinder, ULONG lEndCylinder, ULONG lSectorPerCylinder); // Add a new zone
	ULONG GetZoneCount() const;
	BOOL IsZoneValid(ULONG lZone) const;

	// Sectors on cylinders
	ULONG GetSectorCount(ULONG lTrack) const; // Get number of total sectors on a given track
	BOOL IsSectorValid(ULONG lTrack, ULONG lSector) const; // Check if a sector is valid

	// Drive heads
	void SetHeadCount(ULONG lCount);
	ULONG GetHeadCount() const;
	BOOL IsHeadValid(ULONG lHead) const;
	
	// Defects on disks
	BOOL GetDefect(ULONG lHead, int nIndex, CPoint& defect) const;
	BOOL AddDefect(ULONG lHead, ULONG lTrack, ULONG lSector); // Add a defection
	ULONG GetDefectCount(ULONG lHead) const;
	ULONG GetTotaldefectCount() const;
	BOOL RemoveDefects(ULONG lHead); // Clean the defection on a particular head	
	void RemoveAllDefects();

	BOOL IsDiskReady() const;
	BOOL IsHeadEnabled(ULONG lHead) const;
	void EnableHead(ULONG lHead, BOOL bEnable);
	BOOL SetHeadColor(ULONG lHead, COLORREF color);
	COLORREF GetHeadColor(ULONG lHead) const;

protected:

	// Data struct to represent a "zone" on a disk
	struct DISKZONE
	{
		ULONG lSectorsPerTrk; // Sectors on each cylinder 
		ULONG lOutterTrack; // The beginning cylinder of this zone
		ULONG lTrackNumber; // Total cylinders in this zone
	};
	
	///////////////////////////////////////////////////////////////////////////
	// Helper Functions
	///////////////////////////////////////////////////////////////////////////
	int _TrackInZone(ULONG lTrack) const; // determines which zone a given track is belong to
	int _FindZoneIndex(const DISKZONE& dz) const; // Searches for a zone
	POINT _GetPointOnTrack(ULONG lTrack, double fAngle) const; // Calculates a point on a given track
	ULONG _GetTrackScrRadius(ULONG lTrack) const; // Calculates the screen radius of a given track
	ULONG _GetAlteredTrack(ULONG lTrack) const;

	///////////////////////////////////////////////////////////////////////////
	// Member Data
	///////////////////////////////////////////////////////////////////////////
	CArrayEx<DISKZONE, const DISKZONE&> m_aZones; // Store zones on a disk
	CArrayEx<CHead, const CHead&> m_aHeads; // Store info for each head
	ULONG m_lTotalTrks; // Total track(cylinders) other than inner reserved tracks on this disk
	CPoint m_ptOrigin; // The origin of this disk(screen coordinates)
	ULONG m_lScrRadius; // Disk radius on screen
	CString m_sSN; // Drive Serial No.
	BOOL m_bZoomedIn;
	CRect m_rcZoom;
};

#endif // __DISK_H__
