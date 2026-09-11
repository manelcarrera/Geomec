/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Reservoir.h: interface for the CReservoir class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESERVOIR_H__A88F940D_417E_4D72_9769_98E8C7837DD9__INCLUDED_)
#define AFX_RESERVOIR_H__A88F940D_417E_4D72_9769_98E8C7837DD9__INCLUDED_

#include "FvEclipseCell.h"

#include "FieldValueFileExports.h"

class FIELDVALUEFILE_EXPORT CEclipseReservoir {
  array_temp<int> *m_pEnable;       // Enable Voxet
  array_temp<geo::CPoint> *m_pGrid; // Boundary
  array_temp<double> *m_pDepth;     // Depth
  int m_nUpperBoundary;             // Upper boundary of the reservoir
  int m_nLowerBoundary;             // Lower boundary of the reservoir
  bool m_bCircleCompleted;          // Circle completed
  bool m_bJoin;                     // Enabling Joining of reservoirs
  CEclipseReservoir *m_pLowerBound; // Lowerbound of combined reservoirs
  CEclipseReservoir *m_pUpperBound; // Upperbound of combined reservoirs
  int m_nGridX, m_nGridY;           // Grid Size
  mutable std::set<geo::CPoint *> m_stPoints;

public:
  void CellEnable(int x, int y, int nDepth, bool bEnable);
  bool CellEnable(int x, int y, int nDepth) const;
  //  virtual void AssertValid() const;

  void Depth(const int x, const int y, const int z, const double dDepth);
  double Depth(const int x, const int y, const int z) const;
  void UpperGrid(int x, int y, const geo::IPoint &point);
  void LowerGrid(int x, int y, const geo::IPoint &point);
  const geo::IPoint &UpperGrid(int x, int y) const;
  const geo::IPoint &LowerGrid(int x, int y) const;
  CEclipseCell Cell(int x, int y, int nDepth) const;

  void Boundary(const int nLower, const int nUpper);
  int UpperBoundary() const;
  int LowerBoundary() const;
  bool CircleCompleted() const;
  void CircleCompleted(const bool bCompleted);
  bool Join() const;
  void Join(const bool bJoin);
  void ReservoirBoundary(const CEclipseReservoir &lower, const CEclipseReservoir &upper);
  CEclipseReservoir &LowerReservoirBoundary() const;
  CEclipseReservoir &UpperReservoirBoundary() const;

  int SizeX() const;
  int SizeY() const;
  int DepthSizeX() const { return m_pDepth->size_x(); }
  int DepthSizeY() const { return m_pDepth->size_y(); }
  int DepthSizeZ() const { return m_pDepth->size_z(); }
  int EnableSizeX() const { return m_pEnable->size_x(); }
  int EnableSizeY() const { return m_pEnable->size_y(); }
  int EnableSizeZ() const { return m_pEnable->size_z(); }
  int GridSizeX() const { return m_pGrid->size_x(); }
  int GridSizeY() const { return m_pGrid->size_y(); }
  int GridSizeZ() const { return m_pGrid->size_z(); }

  CEclipseReservoir(int x, int y);
  CEclipseReservoir(const CEclipseReservoir &rhs);
  virtual ~CEclipseReservoir();
};

#endif // !defined(AFX_RESERVOIR_H__A88F940D_417E_4D72_9769_98E8C7837DD9__INCLUDED_)
