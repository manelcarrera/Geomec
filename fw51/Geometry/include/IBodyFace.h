 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IBodyFace.h: interface for the IBodyFace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBODYFACE_H__69B4D902_A58D_42DC_AA05_95A24F873C16__INCLUDED_)
#define AFX_IBODYFACE_H__69B4D902_A58D_42DC_AA05_95A24F873C16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"

#include "GeometryExports.h"

namespace geo {

class IBody;

class GEOMETRY_EXPORT  IBodyFace : public IFace  
{
  std::vector<int> m_vcPoint;
  const IBody& m_body;
  const char *m_Name;

public:
  IBodyFace(const IBody& body, const std::vector<int>& vcPoint, const char *name);
  IBodyFace(const IBody& body, int p0, int p1, int p2, const char *name);
  IBodyFace(const IBody& body, int p0, int p1, int p2, int p3, const char *name);

  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;

  // returns the 'parent' body
  virtual const IBody &Body() const;

  // given the face's node number, returns the corresponding node number of the parent body
  virtual int BodyNode(int nIndex) const;

  const char *Name() const;
};

}

#endif // !defined(AFX_IBODYFACE_H__69B4D902_A58D_42DC_AA05_95A24F873C16__INCLUDED_)
