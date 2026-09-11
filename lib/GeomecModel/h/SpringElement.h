// Spring.h: interface for the CSpringElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRING_H__74E1F443_B458_49C6_8A61_8C1D52409314__INCLUDED_)
#define AFX_SPRING_H__74E1F443_B458_49C6_8A61_8C1D52409314__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
  class IMesh;
}

#include "IPointElement.h"

class CSpringElement : public geo::IPointElement
{
  geo::IMesh &m_mesh;
  int m_nNodeIndex;
public:
  CSpringElement(geo::IMesh &mesh, int nNodeIndex);
  virtual ~CSpringElement();

  virtual const geo::INode &Node(int nIndex) const;
  virtual size_t NrOfNodes() const;
  virtual std::string Type() const;

  virtual const geo::IElementSet*	IndexingElementSet() const;

  virtual int	PointIndex(int nIndex) const;

//	virtual const geo::IPoint &Point(int nIndex) const;
//	virtual void Point(int nIndex, const geo::IPoint &pt);

};


#endif // !defined(AFX_SPRING_H__74E1F443_B458_49C6_8A61_8C1D52409314__INCLUDED_)
