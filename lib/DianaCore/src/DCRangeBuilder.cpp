
#include "DCRangeBuilder.h"

//#include "lbci.h"

static const int SR_RANGE_MAGIC = -1; // originally from lbci.h

namespace dia {

CRangeBuilder::CRangeBuilder()
: m_pValues(0),
  m_nValues(0)
{
}

CRangeBuilder::~CRangeBuilder()
{
  ClearValues();
}

void CRangeBuilder::AddIndex(int nIndex)
{
  TRangeSet::iterator it;
  for(it = m_stRanges.begin(); it != m_stRanges.end(); ++it)
  {
    if(nIndex >= Lo(*it) && nIndex <= Hi(*it))
    {
      // already part of this range
      return;
    }

    bool bInserted = false;
    if(nIndex - 1 == Hi(*it))
    {
      Hi(const_cast <TRange&> (*it)) = nIndex;
      bInserted = true;
    }
    else if(nIndex + 1 == Lo(*it))
    {
      Lo(const_cast <TRange&> (*it)) = nIndex;
      bInserted = true;
    }

    if(bInserted)
    {
      MergeRanges();
      ClearValues();
      return;
    }
  }

  // not part of any range, create a new one
  TRange range;
  Lo(range) = Hi(range) = nIndex;
  m_stRanges.insert(range);
  ClearValues();
}

std::pair<const ftn_int_t*, int> CRangeBuilder::GetValues() const
{
  if(!m_pValues)
  {
    m_pValues = new ftn_int_t[4 * m_stRanges.size()];

    int iVal = 0;

    // build DIANA ranges
    TRangeSet::const_iterator it;
    for(it = m_stRanges.begin(); it != m_stRanges.end(); ++it)
    {
      const TRange& range = *it;

      if(Size(range) < 4)
      {
        // append single values
        int i;
        for(i = Lo(range); i <= Hi(range); ++i)
          m_pValues[iVal++] = i;
      }
      else
      {
        m_pValues[iVal++] = SR_RANGE_MAGIC; // range magic
        m_pValues[iVal++] = Lo(range);      // lo value
        m_pValues[iVal++] = Hi(range);      // hi value
        m_pValues[iVal++] = 1;              // step
      }
    }

    m_nValues = iVal;
  }

  return std::make_pair(m_pValues, m_nValues);
}

void CRangeBuilder::ClearValues()
{
  if(m_pValues)
  {
    delete[] m_pValues;
    m_pValues = 0;
    m_nValues = 0;
  }
}

void CRangeBuilder::MergeRanges()
{
  TRangeSet::iterator it = m_stRanges.begin();
  while(it != m_stRanges.end())
  {
    TRangeSet::iterator itnext = it;
    ++itnext;
    if(itnext == m_stRanges.end())
      break;

    if(Hi(*it) + 1 == Lo(*itnext))
    {
      // need to merge these
      Hi(const_cast <TRange&> (*it)) = Hi(*itnext);
      m_stRanges.erase(itnext);
    }

    ++it;
  }
}

int& CRangeBuilder::Lo(TRange& range) const
{
  return range.first;
}

int& CRangeBuilder::Hi(TRange& range) const
{
  return range.second;
}

int CRangeBuilder::Lo(const TRange& range) const
{
  return range.first;
}

int CRangeBuilder::Hi(const TRange& range) const
{
  return range.second;
}

int CRangeBuilder::Size(const TRange& range) const
{
  return range.second - range.first + 1;
}

} // namespace dia
