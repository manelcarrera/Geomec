 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Line.cpp: implementation of the CLine class.
// 
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "Line.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

//// CLine implementation

// Default Point construction is enough
CLine::CLine()
{
}

CLine::CLine(const ILine &rhs)
{
  if(rhs.Empty())
  {
    m_first=CNode();
    m_second=CNode();
    assert(Empty());
  }
  else
  {
    rhs.AssertValid();
    m_first=rhs.First();
    m_second=rhs.Second();
    assert(!Empty());
  }
}

CLine::CLine(const CLine &rhs)
{
  if(rhs.Empty())
  {
    m_first=CNode();
    m_second=CNode();
    assert(Empty());
  }
  else
  {
    rhs.AssertValid();
    m_first=rhs.First();
    m_second=rhs.Second();
    assert(!Empty());
  }
}

CLine::CLine(const IPoint &first, const IPoint &second)
{
  m_first=first;
  m_second=second;
  AssertValid();
}

CLine::CLine(const IPoint &p, const IVector &dir)
{
  m_first  = p;
  m_second = p + CPoint(dir);
  AssertValid();
}

CLine& CLine::operator=(const ILine& rhs)
{
  if(rhs.Empty())
  {
    m_first  = CNode();
    m_second = CNode();
    assert(Empty());
  }
  else
  {
    m_first=rhs.First();
    m_second=rhs.Second();
    assert(!Empty());
  }
  return *this;
}

CLine& CLine::operator=(const CLine& rhs)
{
  if(rhs.Empty())
  {
    m_first  = CNode();
    m_second = CNode();
    assert(Empty());
  }
  else
  {
    m_first=rhs.First();
    m_second=rhs.Second();
    assert(!Empty());
  }
  return *this;
}

size_t CLine::Order() const
{
  return 1;
}

size_t CLine::NrOfNodes() const
{
  return 2;
}

const INode& CLine::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  if(nIndex == 0) return m_first;

  return m_second;
}

void CLine::Node(int nIndex, const IPoint& point)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  if(nIndex == 0) m_first = point;
  else m_second = point;
}

}

