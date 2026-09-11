#include "DCTimeLoadTable.h"

#include "DCLoadCase.h"

#include "lbfl.h"

namespace dia {

CTimeLoadTable::CTimeLoadTable(CLoadCase& loadcase)
: ITimeTable(loadcase)
{
}

void CTimeLoadTable::ChangeToTableDir() const
{
  ftn_int_t idx = ftn_int_t(Case().Index() + 1);

  ChangeIndexedDir("/TIMELO/LOAD/", &idx);
}

}
