 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MeshInterfaceBody.cpp: implementation of the CInterfaceElement class.
//
//////////////////////////////////////////////////////////////////////
#include "IMesh.h"
#include "IBody.h"
#include "BodyQuadrilateral.h"
#include "BodyTriangle.h"
#include "InterfaceElement.h"
#include "INode.h"

namespace geo {

//////////////////////////////////////////////////////////////////////
// Implementation of CInterfaceElement
//////////////////////////////////////////////////////////////////////

CInterfaceElement::CInterfaceElement(IMesh& mesh,
                                     const IFace* front,
                                     const IFace* back,
                                     const std::vector<int>& vcPoint)
: m_front(front),
  m_back(back),
  m_nFrontStartIndex(-1),
  m_nBackStartIndex(-1),
  m_vcNode(vcPoint),
  m_mesh(mesh)
{
  m_nIndex = m_mesh.RegisterElement(*this);

  setFront(front);
  setBack(back);
}


CInterfaceElement::~CInterfaceElement()
{
}

const IElementSet* CInterfaceElement::IndexingElementSet() const
{
  return &m_mesh;
}

int CInterfaceElement::PointIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_vcNode[nIndex];
}

void CInterfaceElement::setFront(const geo::IFace* pFront)
{
  m_front = pFront;

  // Determine m_nFrontStartIndex if there is a front triangle
  if(pFront)
  {
    assert(m_vcNode.size()/2 == pFront->NrOfPoints());
    for(int i = 0; i < pFront->NrOfNodes(); i++)
    {
      if(m_mesh.Point(m_vcNode[0]) == pFront->Node(i))
      {
        m_nFrontStartIndex = i;
        break;
      }
    }
    assert(m_nFrontStartIndex >= 0);
    assert(m_mesh.Point(m_vcNode[0]) == pFront->Node(m_nFrontStartIndex)); 
  }
  else
  {
    m_nFrontStartIndex = -1;
  }
}

void CInterfaceElement::setBack(const geo::IFace* pBack)
{
  m_back = pBack;

  // Determine m_nBackStartIndex if there is a back triangle
  if(pBack)
  {
    assert(m_vcNode.size()/2 == pBack->NrOfPoints());
    for(int i = 0; i < pBack->NrOfNodes(); i++)
    {
      if(m_mesh.Point(m_vcNode[m_vcNode.size()/2]) == pBack->Node(i))
      {
        m_nBackStartIndex = i;
        break;
      }
    }
    assert(m_nBackStartIndex >= 0);
    assert(m_mesh.Point(m_vcNode[m_vcNode.size()/2]) == pBack->Node(m_nBackStartIndex));    
  }
  else
  {
    m_nBackStartIndex = -1;
  }
}

const IElement& CInterfaceElement::Front() const
{
  assert(m_front);
  return *m_front;
}

const IElement& CInterfaceElement::Back() const
{
  assert(m_front);
  return *m_back;
}

const IFace* CInterfaceElement::FrontFace() const
{
  return m_front;
}

const IFace* CInterfaceElement::BackFace() const
{
  return m_back;
}

int CInterfaceElement::NrOfPoints() const
{
  return NrOfNodes();
}

const IPoint &CInterfaceElement::Point(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return Node(nIndex);
}

void CInterfaceElement::Point(int /*nIndex*/, const IPoint &/*pt*/)
{
  assert(false);
}

size_t CInterfaceElement::NrOfNodes() const
{
  return (int)m_vcNode.size();
}

const INode& CInterfaceElement::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_mesh.Node(m_vcNode[nIndex]);
}

void CInterfaceElement::Node(int /*nIndex*/, const IPoint &/*pt*/)
{
  assert(false);
}

int CInterfaceElement::Index() const
{
  return m_nIndex;
}

std::string CInterfaceElement::Type() const
{
  if(NrOfNodes() == 6) return std::string("T18IF");
  return std::string("Q24IF");
}

// gives the (local) face node number given the (local) node number
// for node 0 to 3 a node number of Front is returned
// for node 4 to 7 a node number of Back is returned
int CInterfaceElement::BodyFaceNode(int nIndex) const
{
  if( nIndex >= 0 && nIndex < NrOfNodes()/2 ) {
    // front
    assert(m_front);
    return (m_nFrontStartIndex + nIndex) % m_front->NrOfPoints();
  } 

  // back
  assert(m_back);
  return (m_nBackStartIndex + m_back->NrOfPoints() - (nIndex - m_front->NrOfPoints())) % m_back->NrOfPoints();
}

int CInterfaceElement::FrontStartIndex() const
{
  return m_nFrontStartIndex;
}

int CInterfaceElement::BackStartIndex() const
{
  return m_nBackStartIndex;
}

int CInterfaceElement::BodyNode( int nIndex ) const
{
  int nFaceNode = BodyFaceNode( nIndex );
  
  const CBodyQuadrilateral* pQuad = 0;
  const CBodyTriangle* pTri = 0;  
  if( 0 <= nIndex && nIndex < NrOfPoints()/2 ) {
    pQuad = dynamic_cast<const CBodyQuadrilateral*>( &Front() );
    pTri  = dynamic_cast<const CBodyTriangle*>( &Front() );
  } else {
    pQuad = dynamic_cast<const CBodyQuadrilateral*>( &Back() );
    pTri  = dynamic_cast<const CBodyTriangle*>( &Back() );
  }
  if(pQuad) return pQuad->BodyNode( nFaceNode );
  if(pTri ) return pTri->BodyNode( nFaceNode );

  assert(false);
  
  return -1;
}

/*!
   Use the front or back for interface polation
*/
CValue CInterfaceElement::InterpolateValue(const IPoint& point, const std::vector<CValue>& values) const
{
  if( FrontFace() ) {
     std::vector<CValue> vcValue(values);
     vcValue.resize( FrontFace()->NrOfPoints() );
     return FrontFace()->InterpolateValue( point, vcValue );
  }

  assert(BackFace());
  std::vector<CValue> vcValue(values);
  vcValue.resize( BackFace()->NrOfPoints() );
  return BackFace()->InterpolateValue( point, vcValue );  
}

/*!
   Use the front or back for contains
*/
bool CInterfaceElement::Contains(const IPoint &point, bool bIncludeEdge) const
{
  if( FrontFace() ) {
     return FrontFace()->Contains(point, bIncludeEdge);
  }

  assert(BackFace());
  return BackFace()->Contains(point, bIncludeEdge);
}

}
