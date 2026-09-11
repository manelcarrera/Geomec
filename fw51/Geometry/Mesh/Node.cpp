 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "Node.h"
#include <cassert>

namespace geo {

//// CNode implementation
CNode::CNode() : m_pCoord(0), m_pMesh(0)
{
}

CNode::CNode(const IMesh& mesh) : m_pCoord(0), m_pMesh(&mesh)
{
}

CNode::CNode(const CNode& rhs) : m_pMesh(rhs.m_pMesh)
{
  if(!rhs.Empty())
  {
    m_pCoord = new double[3];
    m_pCoord[0] = rhs.X();
    m_pCoord[1] = rhs.Y();
    m_pCoord[2] = rhs.Z();
  }
}

CNode::CNode(const INode& rhs) : m_pMesh(&rhs.Mesh())
{
  if(!rhs.Empty())
  {
    m_pCoord = new double[3];
    m_pCoord[0] = rhs.X();
    m_pCoord[1] = rhs.Y();
    m_pCoord[2] = rhs.Z();
  }
}

CNode::CNode(const IPoint& rhs, const IMesh& mesh) : m_pMesh(&mesh)
{
  if(!rhs.Empty())
  {
    m_pCoord = new double[3];
    m_pCoord[0] = rhs.X();
    m_pCoord[1] = rhs.Y();
    m_pCoord[2] = rhs.Z();
  }
}

CNode::CNode(const IPoint& rhs) : m_pMesh(0)
{
  if(!rhs.Empty())
  {
    m_pCoord = new double[3];
    m_pCoord[0] = rhs.X();
    m_pCoord[1] = rhs.Y();
    m_pCoord[2] = rhs.Z();
  }
}

CNode::~CNode()
{
  if(m_pCoord) delete[] m_pCoord;
}

const CNode& CNode::operator=(const CNode& rhs)
{
  m_pMesh = rhs.m_pMesh;

  if(!rhs.Empty())
  {
    X(rhs.X());
    Y(rhs.Y());
    Z(rhs.Z());
  }

  return *this;
}

const CNode& CNode::operator=(const INode& rhs)
{
  m_pMesh = &rhs.Mesh();

  if(!rhs.Empty())
  {
    X(rhs.X());
    Y(rhs.Y());
    Z(rhs.Z());
  }

  return *this;
}

const CNode& CNode::operator=(const ICoordinate& rhs)
{
  if(!rhs.Empty())
  {
    X(rhs.X());
    Y(rhs.Y());
    Z(rhs.Z());
  }

  return *this;
}

const IMesh &CNode::Mesh() const
{
  assert(m_pMesh);
  return *m_pMesh;
}

int CNode::Index() const
{
  return -1;
}

int CNode::AttachedElementSize() const
{
  return 0;
}

const IElement& CNode::AttachedElement(int /*nIndex*/) const
{
  assert(false);
  const IElement* pBogus = 0;
  return *pBogus;
}

int CNode::RegisterElement(IElement& /*element*/)
{
  assert(false);
  return -1;
}

bool CNode::UnregisterElement(IElement& /*element*/)
{
  assert(false);
  return false;
}

void CNode::AssertValid() const
{
}

bool CNode::Empty() const
{
  return (m_pCoord == 0);
}

const double& CNode::X() const
{
  assert(!Empty());
  return m_pCoord[0];
}

const double& CNode::Y() const
{
  assert(!Empty());
  return m_pCoord[1];
}

const double& CNode::Z() const
{
  assert(!Empty());
  return m_pCoord[2];
}

void CNode::X(const double& x)
{
  if(Empty()) m_pCoord = new double[3];
  m_pCoord[0] = x;
}

void CNode::Y(const double& y)
{
  if(Empty()) m_pCoord = new double[3];
  m_pCoord[1] = y;
}

void CNode::Z(const double& z)
{
  if(Empty()) m_pCoord = new double[3];
  m_pCoord[2] = z;
}

} // namespace geo

