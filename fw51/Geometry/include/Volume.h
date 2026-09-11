 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Volume.h: interface for the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUME_H__44504D3B_51A2_4216_AE89_8F2EBB5946D9__INCLUDED_)
#define AFX_VOLUME_H__44504D3B_51A2_4216_AE89_8F2EBB5946D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "IVolume.h"
#include "NearestNeighbour.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  CVolume : public IVolume  
{
  typedef std::set<const IBody*> TBodySet;
  typedef std::vector<const IBody*> TBodyVec;
  typedef std::map<const IPoint*, TBodyVec, ICoordinate::CCoordinateLess> TPointMap;

  mutable TPointMap m_mpPointMap;
  TBodyVec m_vcBody;
  mutable geo::CNearestNeighbour m_NearestNeighbour;


  void UpdatePointMap() const;
  void UpdateFaceVector() const;
protected:
  virtual IPoint& PointAt(int nIndex);
  virtual void InvalidateCache();
public:
  CVolume();
  CVolume(const IVolume &rhs);
  virtual ~CVolume();

  void Clear();
  void ClearEdgeFaces();
  void PushBack(const IBody &body);

  virtual const IBody &Body(int nIndex) const;
  virtual int BodySize() const;

  virtual const IPoint &Point(int nIndex) const;
  virtual int PointSize() const;

  virtual void AssertValid() const;

  // Point is member of the mesh
  // Return value : Bodies adjacent to the point
  const std::vector<const IBody*>& PointToBody(const IPoint &point) const;

  virtual std::vector<int> Nodes(const IElement &element) const;
};

}

#endif // !defined(AFX_VOLUME_H__44504D3B_51A2_4216_AE89_8F2EBB5946D9__INCLUDED_)
