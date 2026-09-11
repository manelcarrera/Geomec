#ifndef _WellPathCenterPointSet_h_
#define _WellPathCenterPointSet_h_

#include "PointSet.h"

namespace WellPath
{

class CWellPathCenterPointSet : public CPointSet
{
  public:
  CWellPathCenterPointSet(
      const std::vector <const CFormationBase*>& formations,
      CModelBase& modelBase);

  private:
  CWellPathCenterPointSet(const CWellPathCenterPointSet& rhs);
  CWellPathCenterPointSet& operator = (const CWellPathCenterPointSet& rhs);

  void populatePointSet(
      const std::vector <const CFormationBase*>& formations,
      CModelBase& modelBase);
};

} // namespace WellPath

#endif  // _WellPathCenterPointSet_h_
