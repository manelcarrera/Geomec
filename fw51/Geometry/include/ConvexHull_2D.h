/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
 /*                                         Copyright (c) 2008 TNO DIANA BV */
// ConvexHull_2D.h: interface for the CConvexHull_2D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVEXHULL_2D_H__5B97FFFA_D072_48C1_BA3B_5805D31FDEED__INCLUDED_)
#define AFX_CONVEXHULL_2D_H__5B97FFFA_D072_48C1_BA3B_5805D31FDEED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"

#include "GeometryExports.h"

#include "IProgressBase.h"

namespace geo {

class tPointStructure;
typedef tPointStructure* tPointArray;

class tPointStructure : public IPoint {
  int vnum;
  CPoint p;
  bool bDelete;
  int m_coor_disabled;
public:
  tPointStructure();
  tPointStructure(const IPoint& point, int nIndex, int coor_disabled);
  tPointStructure( const tPointStructure& rhs );

  tPointStructure& operator=(const tPointStructure& rhs);

  bool operator<(const tPointStructure& rhs) const { return vnum < rhs.vnum; }

  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;

  virtual void point(const IPoint &point) { p= point; }
  virtual void X(const double &dX) { p.X(dX); }
  virtual void Y(const double &dY) { p.Y(dY); }
  virtual void Z(const double &dZ) { p.Z(dZ); }

  virtual void AssertValid() const { assert(!p.Empty()); }
  virtual bool Empty() const { return p.Empty(); }

  void Delete() { bDelete = true; }
  bool isDeleted() const { return bDelete; }

  const IPoint& originalPoint() const { return p; }

  int disabledCoordinate() const { return m_coor_disabled; }

  void print() const;

  long SaveProgressSize() const;

  template <class STREAM>
  void Save(STREAM& stream, IProgressBase& progress);

  template <class STREAM>
  void Load(STREAM& stream, IProgressBase& progress);
};

template <class STREAM>
void tPointStructure::Save(STREAM& stream, IProgressBase& /*progress*/)
{
  stream << vnum;
  stream << p.X() << p.Y() << p.Z();
  stream << int(bDelete ? 1 : 0);
  stream << m_coor_disabled;
}

template <class STREAM>
void tPointStructure::Load(STREAM& stream, IProgressBase& /*progress*/)
{
  stream >> vnum;
  double x, y, z;
  stream >> x >> y >> z;
  p.Set(x, y, z);
  int iDelete;
  stream >> iDelete;
  bDelete = (iDelete != 0);
  stream >> m_coor_disabled;
}

class GEOMETRY_EXPORT  CConvexHull_2D : public IFace
{
  tPointArray m_aPoint;
  int         m_size;
  std::vector<CPoint> m_vcPoint; // originals, for resizing

  void BuildConvexHull();
  void FindLowest();
  void Swap(int i, int j);
  void Squash();
  void Graham();

  void StoreHull(); // store and load originals
  void LoadHull();

public:
  CConvexHull_2D();
  CConvexHull_2D(const std::vector<const geo::IPoint*>& vcPoint, int coor_disabled = 3);
  CConvexHull_2D(const CPtrArray<IPoint> &points, int coor_disabled = 3);
  virtual ~CConvexHull_2D();

  // Interface
  void ResizeHull(const double& size);
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;
  virtual size_t Order() const;
  virtual double Size() const;

  long SaveProgressSize() const;

  template <class STREAM>
  void Save(STREAM& stream, IProgressBase& progress);

  template <class STREAM>
  void Load(STREAM& stream, IProgressBase& progress);

  //TODO not implemented yet (htg)
  bool ContainsXY(const IPoint &point, bool bIncludeEdge) const;
};

template <class STREAM>
void CConvexHull_2D::Save(STREAM& stream, IProgressBase& progress)
{
  stream << m_size;

  for(int i = 0; i < m_size; ++i)
  m_aPoint[i].Save(stream, progress);

  progress.Step();
}

template <class STREAM>
void CConvexHull_2D::Load(STREAM& stream, IProgressBase& progress)
{
  stream >> m_size;

  m_aPoint = new tPointStructure[m_size];

  for(int i = 0; i < m_size; ++i)
  {
  m_aPoint[i] = tPointStructure();
  m_aPoint[i].Load(stream, progress);
  }

  StoreHull();

  progress.Step();
}

} 

#endif // !defined(AFX_CONVEXHULL_2D_H__5B97FFFA_D072_48C1_BA3B_5805D31FDEED__INCLUDED_)
