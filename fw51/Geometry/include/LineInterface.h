 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// LineInterface.h: interface for the CLineInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEINTERFACE_H__9AE95BE4_9B12_4336_BADD_48B6A6AAD915__INCLUDED_)
#define AFX_LINEINTERFACE_H__9AE95BE4_9B12_4336_BADD_48B6A6AAD915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IInterfaceElement.h"

#include "GeometryExports.h"

namespace geo {

/*!
  \class CLineInterface
  \brief Line interface used in Geomec

 IMPORTANT:
 - the order of the nodes is NOT cyclic
 - DON'T swap the sides, the face connected to 1-2 should be at the left, the other at the right
   (the z-axis must point outward)

 y

 ^
 |

 1     2
 :=====: --> x
 3     4

 n1 through n4 are the mesh node indices in mesh this interface is connected to
 f1 is the face connected at the 1-2 side
 f1n1 and f1n2 are the local node numbers of f1 connected to 1 and 2
 f2 is the face connected at the 3-4 side
 f2n1 and f2n2 are the local node numbers of f2 connected to 3 and 4
*/

class IFace;
class IMesh;

// simple 1st order line interface element
class GEOMETRY_EXPORT  CLineInterface : public IInterfaceElement
{
  const IFace &m_front;
  const IFace &m_back;
  int m_nodes[4];
  IMesh& m_mesh;
  int m_nIndex;
  const ILine& getLine(int n0, int n1, const geo::IFace& face) const;
public:
  CLineInterface(IMesh &mesh,
                 int n1, int n2, const geo::IFace &front,
                 int n3, int n4, const geo::IFace &back);
  virtual ~CLineInterface();

// IElement interface
  virtual int PointIndex(int nIndex) const;
  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint& point);
  virtual const IElementSet* IndexingElementSet() const;
  virtual int Index() const;

  virtual const IElement& Front() const;
  const ILine& FrontLine() const;
  const IFace& FrontFace() const;
  
  virtual const IElement& Back() const;
  const ILine& BackLine() const;
  const IFace& BackFace() const;

  int FaceNode(int nIndex) const;

  virtual std::string Type() const;

  virtual size_t Order() const { return 1; }

  virtual void AssertValid() const;

  using IInterfaceElement::InterpolateValue;
  virtual CValue InterpolateValue(const IPoint& point, const std::vector<CValue>& values) const;
  virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;
};

} // namespace geo

#endif // !defined(AFX_LINEINTERFACE_H__9AE95BE4_9B12_4336_BADD_48B6A6AAD915__INCLUDED_)
