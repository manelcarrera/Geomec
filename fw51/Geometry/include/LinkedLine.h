/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
/*                                         Copyright (c) 2009 TNO DIANA BV */
////////////////////////////////////////////////////////////////////////
// CLinkedLine is helper class for the IBody intersection calculation so
// the class is not exported to client applications in the geometry.h.
// The client can add lines to the CLinkedLine and the CLinkedLine
// constructs a polygon.
////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKEDLINE_H__A1B463D1_94EE_49E0_8ED8_E9005AAD26D8__INCLUDED_)
#define AFX_LINKEDLINE_H__A1B463D1_94EE_49E0_8ED8_E9005AAD26D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CoordinateSet.h"
#include "IFace.h"

namespace geo {

class CLinkedLine : public IFace {
  class CLinkedPoint : public CPoint {
    typedef std::pair<CLinkedPoint *, CLinkedPoint *> TPointPair;
    TPointPair m_pair;
    CLinkedLine &m_linked_line;
    void Swap();

  public:
    CLinkedPoint(CLinkedLine &linked_line, const IPoint &point);
    CLinkedPoint *First();
    CLinkedPoint *Second();
    bool Connect(CLinkedPoint &point);
  };
  friend class CLinkedPoint;
  typedef CCoordinateSet<IPoint *> TPointSet;
  // std::set<IPoint*, ICoordinate::CCoordinateLess> m_stPoint;
  TPointSet m_stPoint;
  CLinkedPoint *m_pBegin;
  mutable std::vector<IPoint *> m_vcPoint;
  bool CreateCache() const;

public:
  CLinkedLine();
  virtual ~CLinkedLine();
  bool AddLine(const IPoint &first, const IPoint &second);
  void Clear();
  bool Valid() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;
  using IFace::InterpolateValue;
  virtual CValue InterpolateValue(const IPoint &point, const std::vector<CValue> &values) const;
  virtual size_t Order() const;
};

} // namespace geo

#endif // !defined(AFX_LINKEDLINE_H__A1B463D1_94EE_49E0_8ED8_E9005AAD26D8__INCLUDED_)
