#ifndef _FaultFractureList_h_
#define _FaultFractureList_h_

class CFemAppModel;

#include <vector>

#include "FaultFracture.h"

class CFaultFractureList
{
  public:
  CFaultFractureList(const CFemAppModel* femAppModel, CDoubleQuantity::UNIT unit);

  static bool hasFaultFractures(const CFemAppModel* femAppModel);

  size_t size() const;

  CFaultFracture& operator [] (size_t index);
  const CFaultFracture& operator [] (size_t index) const;

  private:
  CFaultFractureList(const CFaultFractureList& rhs);
  CFaultFractureList& operator = (const CFaultFractureList& rhs);

  static std::vector <CFaultFracture>
      retrieveFaultFractureList(const CFemAppModel* femAppModel, CDoubleQuantity::UNIT unit);

  std::vector <CFaultFracture> m_faultFractureList;
};

#endif  // _FaultFractureList_h_
