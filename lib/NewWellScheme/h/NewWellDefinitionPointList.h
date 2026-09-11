#ifndef _WellDefinitionPointList_h_
#define _WellDefinitionPointList_h_

#include "NewWellPointList.h"

class CNewWellDefinitionPointList : public CNewWellPointList
{
  public:
  //returns the number of removed points
  long RemoveRedundantPoints();

  private:
  friend class CNewWellPathBase;

  CNewWellDefinitionPointList(CNewWellPathBase& WellPath);
  virtual ~CNewWellDefinitionPointList();
};

#endif  // _WellDefinitionPointList_h_
