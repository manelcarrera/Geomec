// Tying.cpp: implementation of the CHexaTying class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "HexaTying.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHexaTying::CHexaTying()
{

}

CHexaTying::CHexaTying(geo::CPoint &slave, geo::CPoint &master1, double dWeightFactor1, geo::CPoint &master2, double dWeightFactor2)
{
  assert(&slave != 0);
  assert(&master1 != 0);
  assert(&master2 != 0);

  m_pSlave = &slave;
  m_pMaster1 = &master1;
  m_dWeightFactor1 = dWeightFactor1;
  m_pMaster2 = &master2;
  m_dWeightFactor2 = dWeightFactor2;

  double x = m_pSlave->X();
}

CHexaTying::CHexaTying(const CHexaTying &tying)
{
  m_pSlave = tying.m_pSlave;
  m_pMaster1 = tying.m_pMaster1;
  m_dWeightFactor1 = tying.m_dWeightFactor1;
  m_pMaster2 = tying.m_pMaster2;
  m_dWeightFactor2 = tying.m_dWeightFactor2;
}

CHexaTying::~CHexaTying()
{

}

void CHexaTying::SetSlave(geo::CPoint &slave)
{
  assert(&slave != 0);
  m_pSlave = &slave;
}

void CHexaTying::SetMaster(int nr, geo::CPoint &master, double dWeightFactor)
{
  assert((nr == 1) || (nr == 2));
  if (nr == 1)
  {
    assert(&master != 0);
    m_pMaster1 = &master;
    m_dWeightFactor1 = dWeightFactor;
  }
  else
  {
    assert(&master != 0);
    m_pMaster2 = &master;
    m_dWeightFactor2 = dWeightFactor;
  }
}

geo::CPoint *CHexaTying::GetSlave() const
{
  double x = m_pSlave->X();
  return m_pSlave;
}

geo::CPoint *CHexaTying::GetMaster(int nr) const
{
  assert((nr == 1) || (nr == 2));
  if (nr == 1)
    return m_pMaster1;
  else
    return m_pMaster2;
}

double CHexaTying::GetWeightFactor(int nr) const
{
  assert((nr == 1) || (nr == 2));
  if (nr==1)
    return m_dWeightFactor1;
  else
    return m_dWeightFactor2;
}