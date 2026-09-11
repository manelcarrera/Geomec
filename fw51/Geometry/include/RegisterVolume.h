 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IRegisterVolume.h: interface for the IRegisterVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTERVOLUME_H__FCF779DD_6E28_40FE_82DE_7D59158FA27F__INCLUDED_)
#define AFX_REGISTERVOLUME_H__FCF779DD_6E28_40FE_82DE_7D59158FA27F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "IVolume.h"
#include "NearestNeighbour.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  CRegisterVolume : public IVolume
{
  typedef std::map<const IPoint*, int, ICoordinate::CCoordinateLess> TPointMap;
  typedef std::set<int> TBodySet;
  typedef std::vector<const IBody*> TBodyVec;
  
  std::vector<const IBody*> m_vcBody;
  std::vector<TBodySet> m_vcPointToBody;
  TPointMap m_mpPoint;
  CNearestNeighbour m_vcPoint;
protected:
  virtual IPoint& PointAt(int nIndex);
public:
  CRegisterVolume();
  virtual ~CRegisterVolume();

  // Registration of points
  virtual int RegisterPoint(IPoint& point);

  // Registration of elements
  virtual int RegisterBody(IBody& body);

  virtual const IBody &Body(int nIndex) const;
  virtual int BodySize() const;

  virtual const IPoint &Point(int nIndex) const;
  
  virtual int PointSize() const;

  virtual void AssertValid() const;
  std::vector<const IBody*> PointToBody(const IPoint &point) const;

  virtual std::vector<int> Nodes(const IElement &element) const;	// Clears point register
  
  virtual void Clear();
};

}

#endif // !defined(AFX_IREGISTERVOLUME_H__FCF779DD_6E28_40FE_82DE_7D59158FA27F__INCLUDED_)
