 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Surface.h: interface for the CSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACE_H__8A0B320F_1D3D_48A4_A21D_24DA01A99B21__INCLUDED_)
#define AFX_SURFACE_H__8A0B320F_1D3D_48A4_A21D_24DA01A99B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISurface.h"
#include "IFace.h"

#include "GeometryExports.h"

namespace geo {
class GEOMETRY_EXPORT  CSurface : public ISurface
{
  class GEOMETRY_EXPORT CFace : public IFace
  {
  std::vector<int> m_vcPoint;
  CSurface& m_surface;
  int m_nIndex;
  public:
  // Construction
  CFace(CSurface& surface, int nIndex, const std::vector<int>& vcPoint);

  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);//Substitute point at nIndex with pt.....
  virtual int NrOfPoints() const;
  virtual double Size() const; // returns volume, area or length (depending on type)
  using IFace::InterpolateValue;
  virtual CValue InterpolateValue(const IPoint &point, const std::vector<CValue> &values) const;
  virtual const IElementSet* IndexingElementSet() const;
  virtual int Index() const;
  virtual int PointIndex(int nIndex) const;
  virtual size_t Order() const;
  };

  typedef std::map<const geo::IPoint*, int, geo::ICoordinate::CCoordinateLess> TPointMap;
  TPointMap m_mpPoint;
  std::vector<const geo::IPoint*> m_vcPoint;
  std::vector<IFace*> m_vcFace;
  std::vector<std::vector<int> > m_vcFaceAtPoint;
public:
  // Construction / Destruction
  CSurface();
  CSurface(const ISurface& rhs);
  CSurface(const std::vector<geo::CPoint>& vcPoint,
       const std::vector<std::vector<int> >& vcTriangle);
  virtual ~CSurface();

  int RegisterPoint(const geo::IPoint& point);
  int AddTriangle(const IPoint& p0, const IPoint& p1, const IPoint& p2);
  int AddEmbeddedTriangle(const IBody& parent, const IPoint& p0, const IPoint& p1, const IPoint& p2);
  int AddRectangle(const IPoint& p0, const IPoint& p1, const IPoint& p2, const IPoint& p3);
  int AddFace(const IFace& face);
  virtual void reserveFaceSize(size_t size);

  virtual const CPtrArray <IFace> FacesAt(const IPoint &p) const;
  virtual const CPtrArray <IFace> FacesAtNode(const IPoint &p) const;
  virtual const IFace &Face(int nIndex) const;
  virtual int FaceSize() const;
  virtual const IPoint& Point(int nIndex) const;
  virtual void Clear();
  virtual int PointSize() const;
  virtual std::vector<int> Nodes(const IElement &element) const;
protected:
  virtual IPoint &PointAt(int nIndex);
};

}

#endif // !defined(AFX_SURFACE_H__8A0B320F_1D3D_48A4_A21D_24DA01A99B21__INCLUDED_)
