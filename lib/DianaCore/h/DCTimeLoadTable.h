#ifndef _DCTIMELOADTABLE_H_
#define _DCTIMELOADTABLE_H_

#include "IDCTimeTable.h"

namespace dia {

class CLoadCase;

// instances of this class must be created on the heap (with new)
// ownership is transferred to the provided loadcase
class CTimeLoadTable : public ITimeTable
{
protected:
  virtual void ChangeToTableDir() const;

public:
  CTimeLoadTable(CLoadCase& loadcase);
};

}

#endif // _DCTIMELOADTABLE_H_
