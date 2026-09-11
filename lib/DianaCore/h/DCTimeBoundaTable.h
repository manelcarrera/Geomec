#ifndef _DCTIMEBOUNDATABLE_H_
#define _DCTIMEBOUNDATABLE_H_

#include "IDCTimeTable.h"

namespace dia {

class CBoundaCase;

// instances of this class must be created on the heap (with new)
// ownership is transferred to the provided boundacase
class CTimeBoundaTable : public ITimeTable
{
protected:
  virtual void ChangeToTableDir() const;

public:
  CTimeBoundaTable(CBoundaCase& boundacase);
};

}

#endif // _DCTIMEBOUNDATABLE_H_
