 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriSurface.h: interface for the CTriSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRISURFACE_H__D1614F07_365B_44DF_A886_FDC76A3FCABD__INCLUDED_)
#define AFX_TRISURFACE_H__D1614F07_365B_44DF_A886_FDC76A3FCABD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"
#include "ISurface.h"

#include "GeometryExports.h"

namespace geo {
class CTriangle;
class GEOMETRY_EXPORT  CTriSurface : public ISurface
{
  std::vector<geo::CPoint> m_vcPoint;
  std::vector<CTriangle*> m_vcFace;
  typedef std::map<const IPoint *, CPtrArray<IFace> > TPointToFacesMap;
  TPointToFacesMap m_mpPointToFaces;

  void InsertFaceIntoPointMap(CTriangle &newface);
public:
  // Construction / Destruction
  CTriSurface();
  CTriSurface(const ISurface& rhs);
  CTriSurface(const std::vector<geo::CPoint>& vcPoint,
       const std::vector<std::vector<int> >& vcTriangle);
  virtual ~CTriSurface();

  int AddPoint(const IPoint& point);
  int AddTriangle(int P0, int P1, int P2);
  virtual void reserveFaceSize(size_t size);

  virtual const CPtrArray <IFace> FacesAt(const IPoint &p) const;
  virtual const CPtrArray <IFace> FacesAtNode(const IPoint &p) const;
  virtual const IFace &Face(int nIndex) const;
  virtual int FaceSize() const;
  virtual const IPoint& Point(int nIndex) const;
  virtual void Clear();
  virtual int PointSize() const;
  void Point(int nIndex, const IPoint& point);
  virtual std::vector<int> Nodes(const IElement &element) const;
protected:
  virtual IPoint &PointAt(int nIndex);
};

}

#endif // !defined(AFX_TRISURFACE_H__D1614F07_365B_44DF_A886_FDC76A3FCABD__INCLUDED_)
