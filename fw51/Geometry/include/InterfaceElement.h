 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(INTERFACE_ELEMENT_H__INCLUDED_)
#define INTERFACE_ELEMENT_H__INCLUDED_

#include "IInterfaceElement.h"

#include "GeometryExports.h"

namespace geo {

class IFace;

// simple 1st order quadrilateral interface element
class GEOMETRY_EXPORT  CInterfaceElement : public IInterfaceElement
{
  const IFace* m_front;
  const IFace* m_back;
  int m_nFrontStartIndex;
  int m_nBackStartIndex;
  std::vector<int> m_vcNode;
  IMesh& m_mesh;
  int m_nIndex;

public:
  // Construction

  // front should be the bodyface at the side with nodes 0 to 3
  // back should be the bodyface at the side with nodes 4 to 7
  // FrontStartIndex should be the local node number of bodyface front corresponding with vcPoint[0]
  // BackStartIndex should be the local node number of bodyface back corresponding with vcPoint[4]
  CInterfaceElement(IMesh& mesh,
          const IFace* front,
          const IFace* back,
          const std::vector<int>& vcPoint);
  virtual ~CInterfaceElement();

  virtual const IElementSet* IndexingElementSet() const;

  virtual int NrOfPoints() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int PointIndex(int nIndex) const;

  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint &pt);

  virtual int Index() const;

  virtual size_t Order() const { return 1; }

  void setFront(const geo::IFace* pFront);
  void setBack(const geo::IFace* pBack);

  virtual const IElement& Front() const;
  virtual const IElement& Back() const;

  const IFace* FrontFace() const;
  const IFace* BackFace() const;

  int FrontStartIndex() const;
  int BackStartIndex() const;

  virtual std::string Type() const;

  // gives the (local) bodyface node number given the (local) node number
  // for node 0 to 3 a node number of Front is returned
  // for node 4 to 7 a node number of Back is returned
  int BodyFaceNode(int nIndex) const;

  // gives the (local) body node number given the (local) node number
  // for node 0 to 3 a local node number of Front body node is returned
  // for node 4 to 7 a local node number of Back body node is returned
  int BodyNode(int nIndex) const;

  using IInterfaceElement::InterpolateValue;
  virtual CValue InterpolateValue(const IPoint& point, const std::vector<CValue>& values) const;
  virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;
};
}

#endif // !defined(AFX_QUADINTERFACE_H__55A68EB0_A3AB_4C6A_9EC1_29B618148EDC__INCLUDED_)
