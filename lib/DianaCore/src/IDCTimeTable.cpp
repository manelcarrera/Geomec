#include "IDCTimeTable.h"
#include "IDCCase.h"

#include <cassert>
#include "lbcx.h"
#include "lbfl.h"

namespace dia {

ITimeTable::ITimeTable(ICase& icase)
: m_case(icase)
{
  icase.AddTimeTable(*this);
}

ITimeTable::~ITimeTable()
{
}

void ITimeTable::Insert(double dTime, double dFactor)
{
  m_mpTable.insert(TTimeFactorMap::value_type(dTime, dFactor));
}

const ICase& ITimeTable::Case() const
{
  return m_case;
}

bool ITimeTable::WriteFilos() const
{
  PushDir();

  ChangeToTableDir();

  ftn_int_t sz = m_mpTable.size();

  ftn_double_t* pTimes = (ftn_double_t*) DIMALLOC(sz * sizeof(ftn_double_t));
  ftn_double_t* pFactors = (ftn_double_t*) DIMALLOC(sz * sizeof(ftn_double_t));

  TTimeFactorMap::const_iterator it;
  int i;
  for(i = 0, it = m_mpTable.begin(); it != m_mpTable.end(); ++it, ++i)
  {
  pTimes[i] = it->first;
  pFactors[i] = it->second;
  }

  PutItemLength("TIMES", pTimes, sz);
  PutItemLength("FACTOR", pFactors, sz);

  DIFREE(pFactors);
  DIFREE(pTimes);

  PopDir();

  return true;
}

}
