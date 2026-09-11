// DCasingPointResult.cpp: implementation of the CDCasingPointResult class.
//
//////////////////////////////////////////////////////////////////////

#include "DCasingPointResult.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCasingPointResult::CDCasingPointResult()
{

}

//////////////////////////////////////////////////////////////////////

CDCasingPointResult::~CDCasingPointResult()
{

}

CDCasingPointResult& CDCasingPointResult::operator=(const CDCasingPointResult& right)
{
  m_DCasingPointSupportedResultMap   = right.m_DCasingPointSupportedResultMap;
  m_DCasingPointUnsupportedResultMap = right.m_DCasingPointUnsupportedResultMap;
  return *this;
}

//////////////////////////////////////////////////////////////////////

DCasingPointSupportedResultMap & CDCasingPointResult::GetDCasingPointSupportedResultMap()
{
  return m_DCasingPointSupportedResultMap;
}

//////////////////////////////////////////////////////////////////////

DCasingPointUnsupportedResultMap & CDCasingPointResult::GetDCasingPointUnsupportedResultMap()
{
  return m_DCasingPointUnsupportedResultMap;
}

//////////////////////////////////////////////////////////////////////

const DCasingPointSupportedResultMap & CDCasingPointResult::GetDCasingPointSupportedResultMap() const
{
  return m_DCasingPointSupportedResultMap;
}

//////////////////////////////////////////////////////////////////////

const DCasingPointUnsupportedResultMap & CDCasingPointResult::GetDCasingPointUnsupportedResultMap() const
{
  return m_DCasingPointUnsupportedResultMap;
}