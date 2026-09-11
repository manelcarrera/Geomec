#include "DCTimeBoundaTable.h"

#include "DCBoundaCase.h"

#include "lbfl.h"

namespace dia {

CTimeBoundaTable::CTimeBoundaTable(CBoundaCase &boundacase) : ITimeTable(boundacase) {}

void CTimeBoundaTable::ChangeToTableDir() const {
  ftn_int_t idx = ftn_int_t(Case().Index() + 1);

  ChangeIndexedDir("/TIMEBO/BOUNDA/", &idx);
}

} // namespace dia
