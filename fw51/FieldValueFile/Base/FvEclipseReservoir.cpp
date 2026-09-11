 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Reservoir.cpp: implementation of the CEclipseReservoir class.
//
//////////////////////////////////////////////////////////////////////
#include <cstdlib>  // for size_t
#include "FvEclipseReservoir.h"
#include "Plane.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEclipseReservoir::CEclipseReservoir(int x, int y)
{
  assert(x > 0);
  assert(y > 0);
  m_nGridX = x + 1;
  m_nGridY = y + 1;

  m_nUpperBoundary = 0;
  m_nLowerBoundary = 0;
  m_bCircleCompleted = false;
  m_bJoin = false;
  m_pLowerBound = this;
  m_pUpperBound = this;

  m_pDepth = new array_temp<double>(2 * SizeX(), 2 * SizeY(), 2);
  m_pEnable = new array_temp<int>(SizeX(), SizeY(), 2);
  m_pGrid = new array_temp<geo::CPoint>(m_nGridX, m_nGridY, 2);
}

CEclipseReservoir::CEclipseReservoir(const CEclipseReservoir& rhs)
: m_nUpperBoundary(rhs.m_nUpperBoundary),
  m_nLowerBoundary(rhs.m_nLowerBoundary),
  m_bCircleCompleted(rhs.m_bCircleCompleted),
  m_bJoin(rhs.m_bJoin),
  m_nGridX(rhs.m_nGridX),
  m_nGridY(rhs.m_nGridY)
{
  m_pLowerBound = this;
  m_pUpperBound = this;

  m_pDepth = new array_temp<double>(*rhs.m_pDepth);
  m_pEnable = new array_temp<int>(*rhs.m_pEnable);
  m_pGrid = new array_temp<geo::CPoint>(*rhs.m_pGrid);
}

CEclipseReservoir::~CEclipseReservoir()
{
  if(m_pDepth)
    delete m_pDepth;

  if(m_pGrid)
    delete m_pGrid;
}


void CEclipseReservoir::Boundary(const int nLower, const int nUpper)
{
  assert(nLower >= 0);
  assert(nUpper >= 0);
  assert(nUpper >= nLower);

  m_nLowerBoundary = nLower;
  m_nUpperBoundary = nUpper;

  // Delete old array if any
  if(m_pDepth)
    delete m_pDepth;

  if(m_pGrid)
    delete m_pGrid;

  if(m_pEnable)
    delete m_pEnable;

  // Make new array
  m_pDepth = new array_temp<double>(2 * SizeX(), 2 * SizeY(), (1 + nUpper - nLower) * 2);
  m_pGrid = new array_temp<geo::CPoint>(m_nGridX, m_nGridY, 2);
  m_pEnable = new array_temp<int>(SizeX(), SizeY(), 1 + nUpper - nLower);
}

int CEclipseReservoir::UpperBoundary() const
{
  assert(m_nUpperBoundary >= 0);
  return m_nUpperBoundary;
}

int CEclipseReservoir::LowerBoundary() const
{
  assert(m_nLowerBoundary >= 0);
  return m_nLowerBoundary;
}

bool CEclipseReservoir::CircleCompleted() const
{
  return m_bCircleCompleted;
}

void CEclipseReservoir::CircleCompleted(const bool bCompleted)
{
  m_bCircleCompleted = bCompleted;
}

bool CEclipseReservoir::Join() const
{
  return m_bJoin;
}

void CEclipseReservoir::Join(const bool bJoin)
{
  m_bJoin = bJoin;
}

void CEclipseReservoir::ReservoirBoundary(const CEclipseReservoir &lower, const CEclipseReservoir &upper)
{
  m_pUpperBound = const_cast<CEclipseReservoir*> (&upper);
  m_pLowerBound = const_cast<CEclipseReservoir*> (&lower);
}

CEclipseReservoir& CEclipseReservoir::LowerReservoirBoundary() const
{
  return *m_pLowerBound;
}

CEclipseReservoir& CEclipseReservoir::UpperReservoirBoundary() const
{
  return *m_pUpperBound;
}

int CEclipseReservoir::SizeX() const
{
  return m_nGridX - 1;
}

int CEclipseReservoir::SizeY() const
{
  return m_nGridY - 1;
}

CEclipseCell CEclipseReservoir::Cell(int x, int y, int nDepth) const
{
  assert((x > -1) && (x < SizeX()));
  assert((y > -1) && (y < SizeY()));
  assert((nDepth >= LowerBoundary()) && (nDepth <= UpperBoundary()));

  CEclipseCell cell;

  for(int dx = 0; dx < 2; dx++)
  {
    for(int dy = 0; dy < 2; dy++)
    {
      for(int dz = 0; dz < 2; dz++)
      {
        double dDepth = m_pDepth->get_at(x * 2 + dx, y * 2 + dy, 2 * (nDepth - LowerBoundary()) + dz);
        geo::CLine line(m_pGrid->get_at(x + dx, y + dy, 0), m_pGrid->get_at(x + dx, y + dy, 1));
        geo::CPlane plane(geo::CPoint(0,0, dDepth), geo::CVector::Zaxis);
        geo::CPoint *pPoint = new geo::CPoint(plane.Intersection(line));
        //cell.SetAt(dx, dy, dz, plane.Intersection(line));
        m_stPoints.insert(pPoint);
        cell.SetAt(dx, dy, dz, pPoint);

      }// Z Loop
    }// Y Loop
  }// X Loop

  return cell;
}

void CEclipseReservoir::Depth(const int x, const int y, const int z, const double dDepth)
{
  assert((x > -1) && (x < 2 * SizeX()));
  assert((y > -1) && (y < 2 * SizeY()));
  assert((z >= 2 * LowerBoundary()) && (z < 2 * (UpperBoundary() + 1)));

  m_pDepth->set_at(x, y, z - 2 * LowerBoundary(), dDepth);
}

double CEclipseReservoir::Depth(const int x, const int y, const int z) const
{
  assert((x > -1) && (x < 2 * SizeX()));
  assert((y > -1) && (y < 2 * SizeY()));
  assert((z >= 2 * LowerBoundary()) && (z < 2 * (UpperBoundary() + 1)));

  return m_pDepth->get_at(x, y, z - 2 * LowerBoundary());
}

void CEclipseReservoir::UpperGrid(int x, int y, const geo::IPoint& point)
{
  assert((x > -1) && (x < m_nGridX));
  assert((y > -1) && (y < m_nGridY));
  
  m_pGrid->set_at(x, y, 1, point);
}

void CEclipseReservoir::LowerGrid(int x, int y, const geo::IPoint &point)
{
  assert((x > -1) && (x < m_nGridX));
  assert((y > -1) && (y < m_nGridY));
  
  m_pGrid->set_at(x, y, 0, point);
}

const geo::IPoint& CEclipseReservoir::UpperGrid(int x, int y) const
{
  assert((x > -1) && (x < m_nGridX));
  assert((y > -1) && (y < m_nGridY));
  
  return m_pGrid->get_at(x, y, 1);
}

const geo::IPoint& CEclipseReservoir::LowerGrid(int x, int y) const
{
  assert((x > -1) && (x < m_nGridX));
  assert((y > -1) && (y < m_nGridY));
  
  return m_pGrid->get_at(x, y, 0);
}

bool CEclipseReservoir::CellEnable(int x, int y, int nDepth) const
{
  assert((x > -1) && (x < SizeX()));
  assert((y > -1) && (y < SizeY()));
  assert((nDepth >= LowerBoundary()) && (nDepth <= UpperBoundary()));

  return m_pEnable->get_at(x, y, nDepth);		
}

void CEclipseReservoir::CellEnable(int x, int y, int nDepth, bool bEnable)
{
  assert((x > -1) && (x < SizeX()));
  assert((y > -1) && (y < SizeY()));
  assert((nDepth >= LowerBoundary()) && (nDepth <= UpperBoundary()));

  m_pEnable->set_at(x, y, nDepth, bEnable);
}
