/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// SpringLineElement.h: interface for the CSpringLineElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRINGLINEELEMENT_H__35B218DA_3B0D_42D9_9A85_CA5D01D0FEB0__INCLUDED_)
#define AFX_SPRINGLINEELEMENT_H__35B218DA_3B0D_42D9_9A85_CA5D01D0FEB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ILine.h"

#include "GeometryExports.h"

namespace geo {

class IMesh;

class GEOMETRY_EXPORT CSpringLineElement : public ILine {
  IMesh &m_mesh;
  int m_nodes[2];
  int m_nIndex;

public:
  CSpringLineElement(IMesh &mesh, int nFirst, int nSecond);
  CSpringLineElement(IMesh &mesh, const IPoint &first, const IPoint &second, bool bUnique);
  virtual std::string Type() const;

  // IElement interface
  const IElementSet *IndexingElementSet() const;
  virtual size_t NrOfNodes() const;
  virtual const INode &Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint &point);
  virtual int PointIndex(int nIndex) const;
  virtual int Index() const;
};

} // namespace geo

#endif // !defined(AFX_SPRINGLINEELEMENT_H__35B218DA_3B0D_42D9_9A85_CA5D01D0FEB0__INCLUDED_)
