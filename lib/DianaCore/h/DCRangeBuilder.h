#ifndef _RANGEBUILDER_H_
#define _RANGEBUILDER_H_

#include <vector>
#include <set>

#include "dimple.h" // for ftn_int_t

namespace dia {

class CRangeBuilder
{
public:
  CRangeBuilder();
  ~CRangeBuilder();

  void AddIndex(int nIndex);
  std::pair<const ftn_int_t*, int> GetValues() const;

private:
  class CRangeLess
  {
  public:
    bool operator()(const std::pair<int, int>& p1, const std::pair<int, int>& p2) const
    {
      return p1.first < p2.first;
    }
  };

  typedef std::pair<int, int> TRange;
  typedef std::set<TRange, CRangeLess> TRangeSet;

private:
  void ClearValues();
  void MergeRanges();
  int& Lo(TRange& range) const;
  int& Hi(TRange& range) const;
  int Lo(const TRange& range) const;
  int Hi(const TRange& range) const;
  int Size(const TRange& range) const;

private:
  TRangeSet m_stRanges;
//	mutable std::vector<int> m_vcValues;
  mutable ftn_int_t* m_pValues;
  mutable int m_nValues;
};

} // namespace dia

#endif // _RANGEBUILDER_H_
