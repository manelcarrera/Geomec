#ifndef _cora_GetSetInfo_h_
#define _cora_GetSetInfo_h_

class IPointSet;

namespace cora
{

class CGetSetInfo
{
  public:
  CGetSetInfo();

  bool isElementSet(const IPointSet& pointSet) const;

  private:
  CGetSetInfo(const CGetSetInfo& rhs);
  CGetSetInfo& operator = (CGetSetInfo rhs);
};

} // namespace cora

#endif  // _cora_GetSetInfo_h_
