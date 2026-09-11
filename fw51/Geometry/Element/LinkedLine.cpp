 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "LinkedLine.h"

namespace geo {

CLinkedLine::CLinkedPoint::CLinkedPoint(CLinkedLine& linked_line, const IPoint& point)
: CPoint(point),
  m_pair( (CLinkedPoint*)0, (CLinkedPoint*)0),
  m_linked_line(linked_line)

{

  linked_line.m_stPoint.insert(this);
}

CLinkedLine::CLinkedPoint* CLinkedLine::CLinkedPoint::First()
{
  return m_pair.first;
}

CLinkedLine::CLinkedPoint* CLinkedLine::CLinkedPoint::Second()
{
  return m_pair.second;
}

bool CLinkedLine::CLinkedPoint::Connect(CLinkedPoint& point)
{
  if ( &point == this ) return false;


  if((!m_pair.first || !m_pair.second) && (!point.First() || !point.Second()))
  {
    // don't link to the same point twice
    if ( m_pair.first == &point || m_pair.second == &point )
      return false;

    // Link our selves to an empty connection
    // Link the point to ourselves
    if(point.m_pair.first == 0)
    {
      if(m_pair.second)
        Swap();
      assert(m_pair.second == 0);
      assert(point.m_pair.first == 0);
      point.m_pair.first = this;
      m_pair.second = &point;
    }
    else
    {
      if(m_pair.first)
        Swap();
      assert(m_pair.first == 0);
      assert(point.m_pair.second == 0);
      point.m_pair.second = this;
      m_pair.first = &point ;
    }
    return true;
  }
  return false;
}

void CLinkedLine::CLinkedPoint::Swap()
{
  // We swap the points first
  CLinkedPoint* pTemp = m_pair.first;
  m_pair.first = m_pair.second;
  m_pair.second = pTemp;

  // More to swap?
  if(m_pair.first)
  {
    if(m_pair.first->m_pair.second != this)
    {
      assert(m_pair.first->m_pair.first == this);
      m_pair.first->Swap();
    }
  }
  if(m_pair.second)
  {
    if(m_pair.second->m_pair.first != this)
    {
      assert(m_pair.second->m_pair.second == this);
      m_pair.second->Swap();
    }
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinkedLine::CLinkedLine()
: m_pBegin(0)
{

}

CLinkedLine::~CLinkedLine()
{
  Clear();
}

bool CLinkedLine::AddLine(const IPoint& first, const IPoint& second)
{
  m_vcPoint.clear();

  // When we have no begin initialise
  if(m_pBegin == 0)
  {
    assert(m_stPoint.size() == 0);
    m_pBegin = new CLinkedPoint(*this, first);
  }

  // Create linked points
  CLinkedPoint *pFirst = 0;
  CLinkedPoint *pSecond = 0;

  TPointSet::iterator it = m_stPoint.find((IPoint*)&first);
  if(it == m_stPoint.end())
    pFirst = new CLinkedPoint(*this, first);
  else
    pFirst = dynamic_cast<CLinkedPoint*>(*it);
  it = m_stPoint.find((IPoint*)&second);
  if(it == m_stPoint.end())
    pSecond = new CLinkedPoint(*this, second);
  else
    pSecond = dynamic_cast<CLinkedPoint*>(*it);
  assert(pFirst && pSecond);
  return pFirst->Connect(*pSecond);
}

void CLinkedLine::Clear()
{
  for(TPointSet::iterator it = m_stPoint.begin(); it != m_stPoint.end(); it++)
    delete *it;
  m_stPoint.clear();
  m_pBegin = 0;
  m_vcPoint.clear();
}

bool CLinkedLine::CreateCache() const
{
  assert(m_vcPoint.size() == 0);
  
  if(m_pBegin == 0)
    return false;

  // Try to create a circle
  int sz = m_stPoint.size();
  CLinkedPoint *pPoint = m_pBegin;
  while(pPoint)
  {
    if ( m_vcPoint.size() >= sz )
    {
      m_vcPoint.clear();
      return false;
    }

    m_vcPoint.push_back(pPoint);
    pPoint = pPoint->Second();
    if(pPoint == m_pBegin)
    {
      // Stop criterium
      if(m_vcPoint.size() > 2)
        return true;
      else
      {
        m_vcPoint.clear();
        return false;
      }
    }

    if(pPoint == 0)
    {
      m_vcPoint.clear();
      return false;
    }

  }
  return false;
}

bool CLinkedLine::Valid() const
{
  if(m_vcPoint.size() > 2)
    return true;

  return CreateCache();
}

CValue CLinkedLine::InterpolateValue(const IPoint &/*point*/, const std::vector<CValue> &/*values*/) const
{
  assert(false);
  return CValue();
}

const IPoint &CLinkedLine::Point(int nIndex) const
{
  if(m_vcPoint.size() < 3)
    CreateCache();
  
  return *m_vcPoint[nIndex];
}

void CLinkedLine::Point(int /*nIndex*/, const IPoint &/*pt*/)
{
  assert(false);  // No point patching
}

int CLinkedLine::NrOfPoints() const
{
  if(m_vcPoint.size() < 3)
    CreateCache();
  
  return (int)m_vcPoint.size(); 
}

size_t CLinkedLine::Order() const
{
  return 1;
}

}
